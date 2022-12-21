#include "pch.h"
#include "Graphics.h"

#include "Quad.h"

#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "DualTextureShader.h"
#include "LightmapShader.h"
#include "AlphamapShader.h"
#include "BumpShader.h"
#include "SpecularmapShader.h"
#include "FogShader.h"
#include "TranslateShader.h"

namespace wf
{
	Graphics::Graphics()
	{
	}

	Graphics::Graphics(const Graphics&)
	{
	}

	Graphics::~Graphics()
	{
	}

	bool Graphics::Initialize(int _width, int _height, HWND _hwnd)
	{
		Quad::SetScreenSize( _width, _height );

		m_width = _width;
		m_height = _height;

		m_directx = new D3D;

		if ( !m_directx->Initialize( _width, _height, VSYNC_ENABLED, _hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR ) )
		{
			MessageBox( _hwnd, L"Could not initialzie Direct3D", L"Error", MB_OK );
			return false;
		}

		ID3D11Device* device = m_directx->GetDevice();
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		XMMATRIX view;

		m_camera = new Camera;
		m_camera->SetPosition( 0.0f, 0.0f, -10.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( view );

		m_model_loader = new ModelLoader;
		if ( !m_model_loader )
		{
			return false;
		}

		ModelParam param;
		param.device = device;
		param.path = L"./resources/cube.txt";
		param.key = L"cube1";
		if ( !m_model_loader->LoadTangentSpaceModel( param ) )
		{
			return false;
		}

		param.key = L"cube2";
		if ( !m_model_loader->LoadRastertekModel( param ) )
		{
			return false;
		}

		m_color_model = new ColorModel;
		if ( !m_color_model->Initialize( device ) )
		{
			MessageBox( _hwnd, L"Could not initialize the model object", L"Error", MB_OK );
			return false;
		}
	
		m_texture_model = new TextureModel;
		if ( !m_texture_model->Initialize( device, context, "./resources/StoneFloorTexture.tga" ) )
		{
			return false;
		}

		m_light_model = new LightModel;
		if ( !m_light_model->Initialize( device, context, "./resources/StoneFloorTexture.tga" ) )
		{
			return false;
		}

		m_rastertek_model = new RasterTekModel;
		if ( !m_rastertek_model->Initialize( device, context, "./resources/StoneFloorTexture.tga", "./resources/cube.txt" ) )
		{
			return false;
		}
		
		m_text = new Text;
		if ( !m_text->Initialize( device, context, _hwnd, _width, _height, view ) )
		{
			return false;
		}

		m_cursor = new Bitmap;
		if ( !m_cursor->Initialize( device, context, _width, _height, "./resources/windwolf.tga", 32, 32 ) )
		{
			return false;
		}

#define INITIALIZE_WF_SHADER( p, type )\
p = new type;\
if( !p->Initialize( device, _hwnd ) ) return false;

		INITIALIZE_WF_SHADER( m_color_shader, ColorShader );
		INITIALIZE_WF_SHADER( m_texture_shader, TextureShader );
		INITIALIZE_WF_SHADER( m_light_shader, LightShader );
		INITIALIZE_WF_SHADER( m_dual_texture_shader, DualTextureShader );
		INITIALIZE_WF_SHADER( m_lightmap_shader, LightmapShader );
		INITIALIZE_WF_SHADER( m_alphamap_shader, AlphamapShader );
		INITIALIZE_WF_SHADER( m_bump_shader, BumpShader );
		INITIALIZE_WF_SHADER( m_specular_shader, SpecularShader );
		INITIALIZE_WF_SHADER( m_fog_shader, FogShader );
		INITIALIZE_WF_SHADER( m_translate_shader, TranslateShader );


		m_rt1 = new RenderTexture;
		if ( !m_rt1->Initialize( device, _width, _height ) )
		{
			return false;
		}

		m_rt2 = new RenderTexture;
		if ( !m_rt2->Initialize( device, _width, _height ) )
		{
			return false;
		}
		
		if ( !InitializeTextureArray( device, context ) )
		{
			return false;
		}

		if ( !InitializeQuads( device ) )
		{
			return false;
		}

		m_light.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_light.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_light.SetDirection( 0.0f, 0.0f, 1.0f );
		m_light.SetSpecular( 0.0f, 1.0f, 1.0f, 1.0f );
		m_light.SetSpecularPower( 1.0f );

		return true;
	}

	void Graphics::Shutdown()
	{
		ShutdownQuads();
		ReleaseTextureArray();

		SAFE_SHUTDOWN( m_rt2 );
		SAFE_SHUTDOWN( m_rt1 );
		SAFE_SHUTDOWN( m_translate_shader );
		SAFE_SHUTDOWN( m_fog_shader );
		SAFE_SHUTDOWN( m_specular_shader );
		SAFE_SHUTDOWN( m_bump_shader );
		SAFE_SHUTDOWN( m_alphamap_shader );
		SAFE_SHUTDOWN( m_lightmap_shader );
		SAFE_SHUTDOWN( m_dual_texture_shader );
		SAFE_SHUTDOWN( m_cursor );
		SAFE_SHUTDOWN( m_text );
		SAFE_SHUTDOWN( m_rastertek_model );
		SAFE_SHUTDOWN( m_light_shader );
		SAFE_SHUTDOWN( m_light_model );
		SAFE_SHUTDOWN( m_texture_shader );
		SAFE_SHUTDOWN( m_texture_model );
		SAFE_SHUTDOWN( m_color_shader );
		SAFE_SHUTDOWN( m_color_model );
		SAFE_SHUTDOWN( m_model_loader );
		SAFE_DELETE( m_camera );
		SAFE_SHUTDOWN( m_directx );
	}

	bool Graphics::Frame( const FrameParam& param )
	{
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		m_mouse_x = param.mouse_x;
		m_mouse_y = param.mouse_y;

		if ( !m_text->SetMousePosition( param.mouse_x, param.mouse_y, context ) )
		{
			return false;
		}

		if ( !m_text->SetFps( param.fps, context ) )
		{
			return false;
		}

		if ( !m_text->SetCpu( param.usage, context ) )
		{
			return false;
		}

		m_camera->SetPosition( 0.0f, 0.0f, -10.0f );

		return true;
	}

	bool Graphics::Render( float _delta )
	{
		static float rotation{ 0.0f };
		rotation += (float)XM_PI * 0.005f;
		if ( rotation > 360.0f )
			rotation -= 360.0f;

		float fog_start = 0.0f;
		float fog_end = 10.0f;

		XMMATRIX w;
		XMMATRIX v;
		XMMATRIX p;
		XMMATRIX o;
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		ID3D11DepthStencilView* dsv = m_directx->GetDepthStencilView();

		m_camera->Render();

		// set variable
		{
			m_directx->GetWorldMatrix( w );
			m_camera->GetViewMatrix( v );
			m_directx->GetProjectionMatrix( p );
			m_directx->GetOrthoMatrix( o );

			w = XMMatrixRotationY( rotation );
		}

#pragma region RENDER TO TEXTURE
		{
			m_rt1->SetRenderTarget( context, dsv );

			m_rt1->ClearRenderTarget( context, dsv, 0.0f, 1.0f, 0.0f, 1.0f );

			m_rastertek_model->Render( context );

			if ( !m_texture_shader->Render(
				context,
				m_rastertek_model->GetIndexCount(),
				w,
				v,
				p,
				m_rastertek_model->GetTexture()
			) )
			{
				return false;
			}

			m_rt2->SetRenderTarget( context, dsv );

			m_rt2->ClearRenderTarget( context, dsv, 0.5f, 0.5f, 0.5f, 1.0f );

			m_rastertek_model->Render( context );

			if ( !m_fog_shader->Render(
				context,
				m_rastertek_model->GetIndexCount(),
				w,
				v,
				p,
				m_rastertek_model->GetTexture(),
				fog_start,
				fog_end
			) )
			{
				return false;
			}

			m_directx->SetBackBufferRenderTarget();
		}
#pragma endregion

		// render
		{
			m_directx->BeginScene( 0.0f, 0.25f, 0.5f, 1.0f );

			m_rastertek_model->Render( context );

			if ( !m_fog_shader->Render(
				context,
				m_rastertek_model->GetIndexCount(),
				w, v, p,
				m_rastertek_model->GetTexture(),
				fog_start,
				fog_end
			) )
			{
				return false;
			}

			// 2D Draw
			m_directx->GetWorldMatrix( w );
			m_directx->TurnOffZBuffer();

			POINT lt[] = {
				{ 100, 100 },
				{ 100, 300 },
				{ 100, 500 },
				{ 100, 700 },
				{ 300, 100 },
				{ 300, 300 },
				{ 300, 500 },
				{ 300, 700 },
			};

			for ( int i = 0; i < QUAD_COUNT; ++i )
			{
				if ( !m_quads[ i ]->Render( context, lt[ i ].x, lt[ i ].y ) )
				{
					return false;
				}

				int index_count = m_quads[ i ]->GetIndexCount();
				switch ( i )
				{
					case 0:
					{
						if ( !m_dual_texture_shader->Render( context, index_count, w, v, o, m_texture_arrays[ DUAL_TEXTURE_ARRAY ]->GetTextureArray() ) )
						{
							return false;
						}
						break;
					}

					case 1:
					{
						if ( !m_lightmap_shader->Render( context, index_count, w, v, o, m_texture_arrays[ LIGHTMAP_TEXTURE_ARRAY ]->GetTextureArray() ) )
						{
							return false;
						}
						break;
					}

					case 2:
					{
						if ( !m_alphamap_shader->Render( context, index_count, w, v, o, m_texture_arrays[ ALPHAMAP_TEXTURE_ARRAY ]->GetTextureArray() ) )
						{
							return false;
						}
						break;
					}

					case 3:
					{
						if ( !m_bump_shader->Render( context, index_count, w, v, o, m_texture_arrays[ BUMPMAP_TEXTURE_ARRAY ]->GetTextureArray(), m_light.GetDiffuse(), m_light.GetDirection() ) )
						{
							return false;
						}
						break;
					}

					case 4:
					{
						if ( !m_specular_shader->Render( context, index_count, w, v, o, m_texture_arrays[ SPECULAR_TEXTURE_ARRAY ]->GetTextureArray(), m_camera->GetPosition(), m_light.GetDiffuse(), m_light.GetSpecular(), m_light.GetSpecularPower(), m_light.GetDirection() ) ) 
						{ 
							return false; 
						}					
						break;
					}

					case 5:
					{
						if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt1->GetShaderResourceView() ) )
						{
							return false;
						}
						break;
					}

					case 6:
					{
						if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt2->GetShaderResourceView() ) )
						{
							return false;
						}
						break;
					}

					case 7:
					{
						static float x;
						static float y;

						x += _delta * 0.0001f;
						y += _delta * 0.0001f;

						if ( !m_translate_shader->Render( context, index_count, w, v, o, m_texture_model->GetTexture(), x, y ) )
						{
							return false;
						}
						break;
					}
				}
			}

			// text
			m_directx->TurnOnAlphaBlending();

			if ( !m_text->Render( context, w, o ) )
			{
				return false;
			}

			DrawCursor();

			m_directx->TurnOffAlphaBlending();

			m_directx->TurnOnZBuffer();

			m_directx->EndScene();
		}

		return true;
	}

	void Graphics::DrawCursor()
	{
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		XMMATRIX w;
		XMMATRIX v;
		XMMATRIX o;

		m_directx->GetWorldMatrix( w );
		m_camera->GetViewMatrix( v );
		m_directx->GetOrthoMatrix( o );

		constexpr int CURSOR_SIZE = 32;
		
		int x = m_mouse_x;
		int y = m_mouse_y;
		int width = m_directx->GetWidth();
		int height = m_directx->GetHeight();

		m_cursor->Render( context, x, y );
		m_texture_shader->Render( context, m_cursor->GetIndexCount(), w, v, o, m_cursor->GetTexture() );
	}

	bool Graphics::InitializeTextureArray( ID3D11Device*& _device, ID3D11DeviceContext*& _context )
	{
		const wchar_t* path_1[ TEXTURE_ARRAY_COUNT ] = {
			 L"./resources/dirt.tga",
			 L"./resources/stone.tga",
			 L"./resources/stone.tga",
			 L"./resources/stone.tga",
			 L"./resources/bump2.tga",
			 L"./resources/stone.tga"
		};

		const wchar_t* path_2[ TEXTURE_ARRAY_COUNT ] = {
			L"./resources/stone.tga",
			L"resources/lightmap.tga",
			L"./resources/dirt.tga",
			L"./resources/bump.tga",
			L"./resources/stone2.tga",
			L"./resources/bump.tga"
		};

		const wchar_t* path_3[ TEXTURE_ARRAY_COUNT ] = {
			 nullptr,
			 nullptr,
			 L"resources/alphamap.tga",
			 nullptr,
			 L"resources/specular.tga",
			 nullptr
		};


		for ( int i = 0; i < TEXTURE_ARRAY_COUNT; ++i )
		{
			m_texture_arrays[ i ] = new TextureArray;
			if ( !m_texture_arrays[ i ]->Intialize( _device, _context, path_1[ i ], path_2[ i ], path_3[ i ] ) )
			{
				return false;
			}
		}

		return true;
	}

	void Graphics::ReleaseTextureArray()
	{
		for ( int i = 0; i < TEXTURE_ARRAY_COUNT; ++i )
		{
			SAFE_SHUTDOWN( m_texture_arrays[ i ] );
		}
	}

	bool Graphics::InitializeQuads( ID3D11Device*& _device )
	{
		quad_type types[] = {
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::tangent_space,
			quad_type::tangent_space,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
		};

		for ( int i = 0; i < QUAD_COUNT; ++i )
		{
			m_quads[ i ] = Quad::CreateQuad( types[ i ] );
			if ( !m_quads[ i ] )
			{
				return false;
			}

			if ( !m_quads[ i ]->Initialize( _device, 150, 150 ) )
			{
				return false;
			}
		}

		return true;
	}

	void Graphics::ShutdownQuads()
	{
		for ( int i = 0; i < QUAD_COUNT; ++i )
		{
			SAFE_SHUTDOWN( m_quads[ i ] );
		}

	}

}