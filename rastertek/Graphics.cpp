#include "pch.h"
#include "Graphics.h"

#include "Quad.h"
#include "Texture.h"

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
#include "TransparentShader.h"
#include "FadeShader.h"
#include "ReflectionShader.h"
#include "MultiLightShader.h"
#include "FireShader.h"
#include "BlurShader.h"
#include "WaterShader.h"
#include "RefractShader.h"
#include "DepthShader.h"
#include "GlassShader.h"
#include "ProjectionShader.h"

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

		int half_width = int(_width / 2.0f );
		int half_height = int( _height / 2.0f );

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

		m_fire = new RasterTekModel;
		if ( !m_fire->Initialize( device, context, "./resources/stone.tga", "./resources/quad.txt" ) )
		{
			return false;
		}

		m_triangles = new InstanceModel;
		if ( !m_triangles->Initialize( device, context, "./resources/ice.tga" ) )
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

		m_blur_bitmap = new Bitmap;
		if ( !m_blur_bitmap->Initialize( device, context, 400, 400, "./resources/seafloor.tga", 400, 400 ) )
		{
			return false;
		}

#define INITIALIZE_RASTERTEK_MODEL( p, tga, txt )\
p = new RasterTekModel;\
if( !p->Initialize( device, context, tga, txt ) ) return false;

		INITIALIZE_RASTERTEK_MODEL( m_rastertek_model, "./resources/seafloor.tga", "./resources/cube.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_floor, "./resources/stone.tga", "./resources/floor.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_plane, "./resources/stone.tga", "./resources/plane01.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_water_ground, "./resources/water_ground.tga", "./resources/ground.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_water_bath, "./resources/water_bath.tga", "./resources/bath.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_water_wall, "./resources/water_wall.tga", "./resources/wall.txt" );
		INITIALIZE_RASTERTEK_MODEL( m_water, "./resources/water.tga", "./resources/water.txt" );

		
		if ( !InitializeShader( device, _hwnd ) )
		{
			return false;
		}

		if ( !InitializeRenderTexture( device, _width, _height ) )
		{
			return false;
		}


#define INITIALIZE_RENDER_TARGET_BITMAP( p, w, h )\
p = new RenderTargetBitmap;\
if( !p->Initialize( device, w, h ) ) return false;

		INITIALIZE_RENDER_TARGET_BITMAP( m_blur_size_bitmap, half_width, half_height );
		INITIALIZE_RENDER_TARGET_BITMAP( m_screen_size_bitmap, _width, _height );
		INITIALIZE_RENDER_TARGET_BITMAP( m_glass_bitmap, _width, _height );

#define INITIALIZE_TEXTURE( p, path )\
p = new Texture;\
if( !p->Initialize( device, context, path ) ) return false;

#define LOAD_DDS_TEXTURE( p, path )\
p = new Texture;\
if( !p->LoadDDS( device, context, path ) ) return false;

		INITIALIZE_TEXTURE( m_blue_texture, "./resources/blue.tga" );
		INITIALIZE_TEXTURE( m_seafloor_texture, "./resources/seafloor.tga" );
		LOAD_DDS_TEXTURE( m_fire_texture, L"./resources/fire01.dds" );
		LOAD_DDS_TEXTURE( m_fire_alpha_texture, L"./resources/alpha01.dds" );
		LOAD_DDS_TEXTURE( m_fire_noise_texture, L"./resources/noise01.dds" );
		INITIALIZE_TEXTURE( m_glass_texture, "./resources/glass.tga" );
		INITIALIZE_TEXTURE( m_glass_bump_texture, "./resources/glass_bump.tga" );
		INITIALIZE_TEXTURE( m_ice_texture, "./resources/ice.tga" );
		INITIALIZE_TEXTURE( m_ice_bump_texture, "./resources/ice_bump.tga" );
		LOAD_DDS_TEXTURE( m_dx11_texture, L"./resources/dx11.dds" );
		LOAD_DDS_TEXTURE( m_grate_texture, L"./resources/grate.dds" );
		
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

		m_projection_light.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_projection_light.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_projection_light.SetDirection( 0.0f, -0.75f, 0.5f );

		m_projection_light2.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_projection_light2.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_projection_light2.SetPosition( 2.0f, 5.0f, -2.0f );

		m_water_light.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_water_light.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_water_light.SetDirection( 0.0f, -1.0f, 0.5f );
		m_water_height = 2.75f;
		m_water_translation = 0.0f;

		m_viewpoint.SetPosition( 2.0f, 5.0f, -2.0f );
		m_viewpoint.SetLookAt( 0.0f, 0.0f, 0.0f );
		m_viewpoint.SetProjectionParameters( XM_PIDIV2, 1.0f, 0.1f, 100.0f );
		m_viewpoint.GenerateViewMatrix();
		m_viewpoint.GenerateProjectionMatrix();


		return true;
	}

	void Graphics::Shutdown()
	{
		ShutdownQuads();
		ReleaseTextureArray();
		
		// Texture
		SAFE_SHUTDOWN( m_grate_texture );
		SAFE_SHUTDOWN( m_dx11_texture );
		SAFE_SHUTDOWN( m_ice_bump_texture );
		SAFE_SHUTDOWN( m_ice_texture );
		SAFE_SHUTDOWN( m_glass_bump_texture );
		SAFE_SHUTDOWN( m_glass_texture );
		SAFE_SHUTDOWN( m_fire_noise_texture );
		SAFE_SHUTDOWN( m_fire_alpha_texture );
		SAFE_SHUTDOWN( m_fire_texture );
		SAFE_SHUTDOWN( m_seafloor_texture );
		SAFE_SHUTDOWN( m_blue_texture );

		// RednerTargetBitmap
		SAFE_SHUTDOWN( m_glass_bitmap );
		SAFE_SHUTDOWN( m_screen_size_bitmap );
		SAFE_SHUTDOWN( m_blur_size_bitmap );

		// RenderTexture
		ShutdownRenderTexture();
		
		ShutdownShader();

		// 
		SAFE_SHUTDOWN( m_blur_bitmap );
		SAFE_SHUTDOWN( m_cursor );
		SAFE_SHUTDOWN( m_text );
		SAFE_SHUTDOWN( m_fire );
		SAFE_SHUTDOWN( m_water_ground );
		SAFE_SHUTDOWN( m_water_bath );
		SAFE_SHUTDOWN( m_water_wall );
		SAFE_SHUTDOWN( m_water );
		SAFE_SHUTDOWN( m_plane );
		SAFE_SHUTDOWN( m_floor );
		SAFE_SHUTDOWN( m_triangles );
		SAFE_SHUTDOWN( m_rastertek_model );
		SAFE_SHUTDOWN( m_light_shader );
		SAFE_SHUTDOWN( m_light_model );
		SAFE_SHUTDOWN( m_texture_shader );
		SAFE_SHUTDOWN( m_texture_model );
		SAFE_SHUTDOWN( m_tessellation_color_shader );
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

		m_camera->SetPosition( 0.0f, 7.0f, -10.0f );
		m_camera->SetRotation( 35.0f, 0.0f, 0.0f );

		FrameFade( param.time );

		m_water_translation += 0.001f;
		if ( m_water_translation > 1.0f )
		{
			m_water_translation -= 1.0f;
		}

		return true;
	}

	bool Graphics::Render( float _delta )
	{
		static float rotation{ 0.0f };
		rotation += (float)XM_PI * 0.005f;
		if ( rotation > 360.0f )
			rotation -= 360.0f;

		XMMATRIX w;
		XMMATRIX v;
		XMMATRIX p;
		XMMATRIX o;
		XMMATRIX rotate;
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		ID3D11DepthStencilView* dsv = m_directx->GetDepthStencilView();

		m_camera->Render();

		// set variable
		{
			m_directx->GetWorldMatrix( w );
			m_camera->GetViewMatrix( v );
			m_directx->GetProjectionMatrix( p );
			m_directx->GetOrthoMatrix( o );

			rotate = XMMatrixRotationY( rotation );
		}

		static bool draw_2d{ true };
		/// static bool draw_2d{ false };

#pragma region RENDER TO TEXTURE
		if( draw_2d )
		{
			DrawCubeScene( context, dsv, w, v, p);
			DrawFogScene( context, dsv, w, v, p );
			DrawFadeScene( context, dsv, w, v, p );
			DrawTransparencyScene( context, dsv, w, v, p );
			DrawReflectScene( context, dsv, w, v, p );
			DrawMultiLightScene( context, dsv, w, v, p );
			DrawFireScene( context, dsv, w, v, p );
			DrawBlurScene( context, dsv, w, v, p );
			DrawTessellationScene( context, dsv, w, v, p );
			DrawWaterScene( context, dsv, w, v, p );
			DrawDepthScene( context, dsv, w, v, p );
			DrawGlassScene( context, dsv, w, v, p );
			DrawIceScene( context, dsv, w, v, p );
			DrawInstanceScene( context, dsv, w, v, p );
			DrawProjectiveTextureScene( context, dsv, w, v, p );
			DrawLightProjectiveTextureScene( context, dsv, w, v, p );

			m_directx->SetBackBufferRenderTarget();
		}
#pragma endregion

		// render
		{
			draw_2d ? m_directx->BeginScene( 0.0f, 0.0f, 1.0f, 1.0f ) : m_directx->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

			if ( !draw_2d )
			{
				XMMATRIX v2 = m_viewpoint.GetViewMatrix();
				XMMATRIX p2 = m_viewpoint.GetProjectionMatrix();
				XMMATRIX t;

				t = XMMatrixTranslation( 0.0f, 1.0f, 0.0f );
				m_floor->Render( context );
				m_light_projection_shader->Render(
					context, m_floor->GetIndexCount(),
					t, v, p,
					m_floor->GetTexture(),
					m_projection_light2.GetAmbient(),
					m_projection_light2.GetDiffuse(),
					m_projection_light2.GetPosition(),
					v2, p2,
					m_grate_texture->GetTexture()
				);

				t = XMMatrixTranslation( 0.0f, 2.0f, 0.0f );
				m_rastertek_model->Render( context );
				m_light_projection_shader->Render(
					context, m_rastertek_model->GetIndexCount(),
					t, v, p,
					m_rastertek_model->GetTexture(),
					m_projection_light2.GetAmbient(),
					m_projection_light2.GetDiffuse(),
					m_projection_light2.GetPosition(),
					v2, p2,
					m_grate_texture->GetTexture()
				);
			}


			m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
			m_camera->SetRotation( 0.0f, 0.0f, 0.0f );
			m_camera->Render();
			m_camera->GetViewMatrix( v );

			// 2D Draw
			m_directx->GetWorldMatrix( w );
			m_directx->TurnOffZBuffer();

			if ( draw_2d )
			{
				Draw2DResult( w, v, o, _delta );
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
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
			quad_type::texture,
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

	bool Graphics::InitializeRenderTexture( ID3D11Device*& _device, int _width, int _height )
	{
		int half_width = int( _width / 2.0f );
		int half_height = int( _height / 2.0f );

#define INITIALIZE_RENDER_TEXTURE( p )\
p = new RenderTexture;\
if( !p->Initialize( _device, _width, _height ) ) return false;

#define INITIALIZE_RENDER_TEXTURE_HALF( p )\
p = new RenderTexture;\
if( !p->Initialize( _device, half_width, half_height ) ) return false;

		INITIALIZE_RENDER_TEXTURE( m_rt1 );
		INITIALIZE_RENDER_TEXTURE( m_rt2 );
		INITIALIZE_RENDER_TEXTURE( m_rt3 );
		INITIALIZE_RENDER_TEXTURE( m_rt4 );
		INITIALIZE_RENDER_TEXTURE( m_rt5 );
		INITIALIZE_RENDER_TEXTURE( m_rt6 );
		INITIALIZE_RENDER_TEXTURE( m_rt7 );
		INITIALIZE_RENDER_TEXTURE( m_rt8 );
		INITIALIZE_RENDER_TEXTURE( m_rt9 );
		INITIALIZE_RENDER_TEXTURE( m_rt10 );
		INITIALIZE_RENDER_TEXTURE( m_rt11 );
		INITIALIZE_RENDER_TEXTURE( m_rt12 );
		INITIALIZE_RENDER_TEXTURE( m_rt13 );
		INITIALIZE_RENDER_TEXTURE( m_rt14 );
		INITIALIZE_RENDER_TEXTURE( m_rt15 );
		INITIALIZE_RENDER_TEXTURE( m_rt16 );
		INITIALIZE_RENDER_TEXTURE( m_rt17 );
		INITIALIZE_RENDER_TEXTURE( m_blur_render_texture );
		INITIALIZE_RENDER_TEXTURE( m_up_sample );
		INITIALIZE_RENDER_TEXTURE( m_water_refract_texture );
		INITIALIZE_RENDER_TEXTURE( m_water_reflect_texture );
		INITIALIZE_RENDER_TEXTURE( m_glass_render_texture );
		INITIALIZE_RENDER_TEXTURE_HALF( m_down_sample );
		INITIALIZE_RENDER_TEXTURE_HALF( m_h_blur );
		INITIALIZE_RENDER_TEXTURE_HALF( m_v_blur );


		return true;
	}

	void Graphics::ShutdownRenderTexture()
	{
		SAFE_SHUTDOWN( m_glass_render_texture );
		SAFE_SHUTDOWN( m_water_refract_texture );
		SAFE_SHUTDOWN( m_water_reflect_texture );
		SAFE_SHUTDOWN( m_v_blur );
		SAFE_SHUTDOWN( m_h_blur );
		SAFE_SHUTDOWN( m_down_sample );
		SAFE_SHUTDOWN( m_up_sample );
		SAFE_SHUTDOWN( m_blur_render_texture );
		SAFE_SHUTDOWN( m_rt17 );
		SAFE_SHUTDOWN( m_rt16 );
		SAFE_SHUTDOWN( m_rt15 );
		SAFE_SHUTDOWN( m_rt14 );
		SAFE_SHUTDOWN( m_rt13 );
		SAFE_SHUTDOWN( m_rt12 );
		SAFE_SHUTDOWN( m_rt11 );
		SAFE_SHUTDOWN( m_rt10 );
		SAFE_SHUTDOWN( m_rt9 );
		SAFE_SHUTDOWN( m_rt8 );
		SAFE_SHUTDOWN( m_rt7 );
		SAFE_SHUTDOWN( m_rt6 );
		SAFE_SHUTDOWN( m_rt5 );
		SAFE_SHUTDOWN( m_rt4 );
		SAFE_SHUTDOWN( m_rt3 );
		SAFE_SHUTDOWN( m_rt2 );
		SAFE_SHUTDOWN( m_rt1 );
	}

	bool Graphics::InitializeShader( ID3D11Device*& _device, HWND _hwnd )
	{
#define INITIALIZE_WF_SHADER( p, type )\
p = new type;\
if( !p->Initialize( _device, _hwnd ) ) return false;

		INITIALIZE_WF_SHADER( m_color_shader, ColorShader );
		INITIALIZE_WF_SHADER( m_tessellation_color_shader, TessellationColorShader );
		INITIALIZE_WF_SHADER( m_texture_shader, TextureShader );
		INITIALIZE_WF_SHADER( m_light_shader, LightShader );
		INITIALIZE_WF_SHADER( m_dual_texture_shader, DualTextureShader );
		INITIALIZE_WF_SHADER( m_lightmap_shader, LightmapShader );
		INITIALIZE_WF_SHADER( m_alphamap_shader, AlphamapShader );
		INITIALIZE_WF_SHADER( m_bump_shader, BumpShader );
		INITIALIZE_WF_SHADER( m_specular_shader, SpecularShader );
		INITIALIZE_WF_SHADER( m_fog_shader, FogShader );
		INITIALIZE_WF_SHADER( m_translate_shader, TranslateShader );
		INITIALIZE_WF_SHADER( m_transparent_shader, TransparentShader );
		INITIALIZE_WF_SHADER( m_fade_shader, FadeShader );
		INITIALIZE_WF_SHADER( m_reflection_shader, ReflectionShader );
		INITIALIZE_WF_SHADER( m_multi_light_shader, MultiLightShader );
		INITIALIZE_WF_SHADER( m_fire_shader, FireShader );
		INITIALIZE_WF_SHADER( m_horizontal_blur_shader, HorizontalBlurShader );
		INITIALIZE_WF_SHADER( m_vertical_blur_shader, VerticalBlurShader );
		INITIALIZE_WF_SHADER( m_water_shader, WaterShader );
		INITIALIZE_WF_SHADER( m_refract_shader, RefractShader );
		INITIALIZE_WF_SHADER( m_depth_shader, DepthShader );
		INITIALIZE_WF_SHADER( m_glass_shader, GlassShader );
		INITIALIZE_WF_SHADER( m_instance_texture_shader, InstanceTextureShader );
		INITIALIZE_WF_SHADER( m_projection_shader, ProjectionShader );
		INITIALIZE_WF_SHADER( m_light_projection_shader, LightProjectionShader );

		return true;
	}

	void Graphics::ShutdownShader()
	{
		SAFE_SHUTDOWN( m_light_projection_shader );
		SAFE_SHUTDOWN( m_projection_shader );
		SAFE_SHUTDOWN( m_instance_texture_shader );
		SAFE_SHUTDOWN( m_glass_shader );
		SAFE_SHUTDOWN( m_depth_shader );
		SAFE_SHUTDOWN( m_refract_shader );
		SAFE_SHUTDOWN( m_water_shader );
		SAFE_SHUTDOWN( m_vertical_blur_shader );
		SAFE_SHUTDOWN( m_horizontal_blur_shader );
		SAFE_SHUTDOWN( m_fire_shader );
		SAFE_SHUTDOWN( m_multi_light_shader );
		SAFE_SHUTDOWN( m_fade_shader );
		SAFE_SHUTDOWN( m_reflection_shader );
		SAFE_SHUTDOWN( m_transparent_shader );
		SAFE_SHUTDOWN( m_translate_shader );
		SAFE_SHUTDOWN( m_fog_shader );
		SAFE_SHUTDOWN( m_specular_shader );
		SAFE_SHUTDOWN( m_bump_shader );
		SAFE_SHUTDOWN( m_alphamap_shader );
		SAFE_SHUTDOWN( m_lightmap_shader );
		SAFE_SHUTDOWN( m_dual_texture_shader );
	}

	void Graphics::FrameFade( float _time )
	{
		float duration = 2000.0f;

		if ( m_fade_time > duration )
		{
			m_fade_time = 0.0f;
		}
		else
		{
			m_fade_time += _time;

			m_fade_rate = m_fade_time / duration;
		}
	}

	void Graphics::Draw2DResult( XMMATRIX& w, XMMATRIX& v, XMMATRIX& o, float _delta )
	{
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();

		POINT lt[] = {
		{ 100, 100 },
		{ 100, 300 },
		{ 100, 500 },
		{ 100, 700 },
		{ 300, 100 },
		{ 300, 300 },
		{ 300, 500 },
		{ 300, 700 },
		{ 500, 100 },
		{ 500, 300 },
		{ 500, 500 },
		{ 500, 700 },
		{ 700, 100 },
		{ 700, 300 },
		{ 700, 500 },
		{ 700, 700 },
		{ 900, 100 },
		{ 900, 300 },
		{ 900, 500 },
		{ 900, 700 },
		{ 1100, 100 },
		{ 1100, 300 },
		};

		for ( int i = 0; i < QUAD_COUNT; ++i )
		{
			if ( !m_quads[ i ]->Render( context, lt[ i ].x, lt[ i ].y ) )
			{
				return;
			}

			int index_count = m_quads[ i ]->GetIndexCount();
			switch ( i )
			{
			case 0:
			{
				if ( !m_dual_texture_shader->Render( context, index_count, w, v, o, m_texture_arrays[ DUAL_TEXTURE_ARRAY ]->GetTextureArray() ) )
				{
					return;
				}
				break;
			}

			case 1:
			{
				if ( !m_lightmap_shader->Render( context, index_count, w, v, o, m_texture_arrays[ LIGHTMAP_TEXTURE_ARRAY ]->GetTextureArray() ) )
				{
					return;
				}
				break;
			}

			case 2:
			{
				if ( !m_alphamap_shader->Render( context, index_count, w, v, o, m_texture_arrays[ ALPHAMAP_TEXTURE_ARRAY ]->GetTextureArray() ) )
				{
					return;
				}
				break;
			}

			case 3:
			{
				if ( !m_bump_shader->Render( context, index_count, w, v, o, m_texture_arrays[ BUMPMAP_TEXTURE_ARRAY ]->GetTextureArray(), m_light.GetDiffuse(), m_light.GetDirection() ) )
				{
					return;
				}
				break;
			}

			case 4:
			{
				if ( !m_specular_shader->Render( context, index_count, w, v, o, m_texture_arrays[ SPECULAR_TEXTURE_ARRAY ]->GetTextureArray(), m_camera->GetPosition(), m_light.GetDiffuse(), m_light.GetSpecular(), m_light.GetSpecularPower(), m_light.GetDirection() ) )
				{
					return;
				}
				break;
			}

			case 5:
			{	// cube
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt1->GetShaderResourceView() ) )
				{
					return;
				}
				break;
			}

			case 6:
			{	// fog
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt2->GetShaderResourceView() ) )
				{
					return;
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
					return;
				}
				break;
			}

			case 8:
			{	// transparency
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt3->GetShaderResourceView() ) )
				{
					return;
				}
				break;
			}

			case 9:
			{	// reflect
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt5->GetShaderResourceView() ) )
				{
					return;
				}
				break;
			}

			case 10:
			{
				// fade
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt4->GetShaderResourceView() ) ) { return; }
			}

			case 11:
			{
				// multi light
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt7->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 12:
			{
				// fire
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt8->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 13:
			{
				// blur
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_up_sample->GetShaderResourceView() ) ) { return; }
				break;
			}
			
			case 14:
			{
				// tessellation
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt10->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 15:
			{
				// water
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt11->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 16:
			{
				// depth
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt12->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 17:
			{
				// glass
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt13->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 18:
			{
				// glass
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt14->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 19:
			{
				// instance
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt15->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 20:
			{
				// projective texture
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt16->GetShaderResourceView() ) ) { return; }
				break;
			}

			case 21:
			{
				// light projective texture
				if ( !m_texture_shader->Render( context, index_count, w, v, o, m_rt17->GetShaderResourceView() ) ) { return; }
				break;
			}

			}
		}
	}

	void Graphics::DrawCubeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& w, const XMMATRIX& v, const XMMATRIX& p )
	{
		m_rt1->SetRenderTarget( _context );
		m_rt1->ClearRenderTarget( _context, 0.0f, 1.0f, 0.0f, 1.0f );
		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), w, v, p, m_rastertek_model->GetTexture() );
	}

	void Graphics::DrawFogScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& w, const XMMATRIX& v, const XMMATRIX& p )
	{
		float fog_start = 0.0f;
		float fog_end = 10.0f;

		m_rt2->SetRenderTarget( _context );
		m_rt2->ClearRenderTarget( _context, 0.5f, 0.5f, 0.5f, 1.0f );
		m_rastertek_model->Render( _context );
		m_fog_shader->Render( _context, m_rastertek_model->GetIndexCount(), w, v, p, m_rastertek_model->GetTexture(), fog_start, fog_end );
	}

	void Graphics::DrawTransparencyScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		m_directx->TurnOnAlphaBlending();

		m_rt3->SetRenderTarget( _context );
		m_rt3->ClearRenderTarget( _context, 0.3f, 0.3f, 1.0f, 1.0f );
		m_rastertek_model->Render( _context );
		m_transparent_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, _v, _p, m_rastertek_model->GetTexture(), 0.5f );

		m_directx->TurnOffAlphaBlending();
	}

	void Graphics::DrawReflectScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX r;

		m_camera->RenderReflect( -1.5f );
		m_camera->GetReflectionMatrix( r );

		m_rt6->SetRenderTarget( _context );
		m_rt6->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, r, _p, m_seafloor_texture->GetTexture() );

		m_rt5->SetRenderTarget( _context );
		m_rt5->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, _v, _p, m_seafloor_texture->GetTexture() );

		XMMATRIX w2 = XMMatrixTranslation( 0.0f, -1.5f, 0.0f );
		 m_floor->Render( _context );
		 m_reflection_shader->Render( _context, m_floor->GetIndexCount(), w2, _v, _p, m_floor->GetTexture(), m_rt6->GetShaderResourceView(), r );
	}

	void Graphics::DrawFadeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& w, const XMMATRIX& v, const XMMATRIX& p )
	{
		m_rt4->SetRenderTarget( _context );
		m_rt4->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
		m_rastertek_model->Render( _context );
		m_fade_shader->Render( _context, m_rastertek_model->GetIndexCount(), w, v, p, m_rastertek_model->GetTexture(), m_fade_rate );
	}

	void Graphics::DrawMultiLightScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		Light light1;
		light1.SetPosition( -3.0f, 1.0f, 3.0f );
		light1.SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );

		Light light2;
		light2.SetPosition( 3.0f, 1.0f, 3.0f );
		light2.SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );

		Light light3;
		light3.SetPosition( -3.0f, 1.0f, -3.0f );
		light3.SetDiffuse( 0.0f, 0.0f, 1.0f, 1.0f );

		Light light4;
		light4.SetPosition( 3.0f, 1.0f, -3.0f );
		light4.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );

		MultiLightShader::LightPositionBuffer position_buffer;
		position_buffer.position[ 0 ] = XMFLOAT4( light1.GetPosition().x, light1.GetPosition().y, light1.GetPosition().z, 1.0f );
		position_buffer.position[ 1 ] = XMFLOAT4( light2.GetPosition().x, light2.GetPosition().y, light2.GetPosition().z, 1.0f );
		position_buffer.position[ 2 ] = XMFLOAT4( light3.GetPosition().x, light3.GetPosition().y, light3.GetPosition().z, 1.0f );
		position_buffer.position[ 3 ] = XMFLOAT4( light4.GetPosition().x, light4.GetPosition().y, light4.GetPosition().z, 1.0f );

		MultiLightShader::LightColorBuffer color_buffer;
		color_buffer.diffuse[ 0 ] = light1.GetDiffuse();
		color_buffer.diffuse[ 1 ] = light2.GetDiffuse();
		color_buffer.diffuse[ 2 ] = light3.GetDiffuse();
		color_buffer.diffuse[ 3 ] = light4.GetDiffuse();

		m_camera->SetPosition( 0.0f, 3.0f, -15.0f );
		m_camera->Render();

		XMMATRIX v;
		m_camera->GetViewMatrix( v );

		m_rt7->SetRenderTarget( _context );
		m_rt7->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
		m_plane->Render( _context );
		m_multi_light_shader->Render( _context, m_plane->GetIndexCount(), _w, v, _p, m_plane->GetTexture(), position_buffer, color_buffer );

		m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
		m_camera->Render();
	}

	void Graphics::DrawFireScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		static float frame_time = 0.0f;

		frame_time += 0.02f;
		if ( frame_time > 1000.0f )
		{
			frame_time = 0.0f;
		}

		XMFLOAT3 scroll_speed{ 1.3f, 2.1f, 2.3f };
		XMFLOAT3 scales{ 1.0f, 2.0f, 3.0f };
		XMFLOAT2 distortion1{ 0.1f, 0.2f };
		XMFLOAT2 distortion2{ 0.1f, 0.3f };
		XMFLOAT2 distortion3{ 0.1f, 0.1f };
		float distortion_scale{ 0.8f };
		float distortion_bias{ 0.5f };


		m_rt8->SetRenderTarget( _context );
		m_rt8->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		XMMATRIX w;
		m_directx->GetWorldMatrix( w );

		m_directx->TurnOnAlphaBlending();

		m_fire->Render( _context );

		m_fire_shader->Render(
			_context,
			m_fire->GetIndexCount(),
			w, _v, _p,
			m_fire_texture->GetTexture(),
			m_fire_noise_texture->GetTexture(),
			m_fire_alpha_texture->GetTexture(),
			frame_time,
			scroll_speed,
			scales,
			distortion1,
			distortion2,
			distortion3,
			distortion_scale,
			distortion_bias
		);

		m_directx->TurnOffAlphaBlending();
	}

	void Graphics::DrawBlurScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		m_directx->TurnOnAlphaBlending();
		
		std::vector<float> weights = utility::CreateGaussianKernel( 23 );

		XMMATRIX w;
		XMMATRIX p;
		XMMATRIX o;
		XMFLOAT2 resolution;
		m_directx->GetWorldMatrix( w );

		// RenderSceneToTexture
		{
			m_blur_render_texture->SetRenderTarget( _context );
			m_blur_render_texture->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

			p = m_blur_render_texture->GetProjectionMatrix();

			m_rastertek_model->Render( _context );
			m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, _v, _p, m_rastertek_model->GetTexture() );

			m_directx->SetBackBufferRenderTarget();
			m_directx->ResetViewport();
		}
		
		m_directx->TurnOffZBuffer();

		// DownSampleTexture
		{
			m_down_sample->SetRenderTarget( _context );
			m_down_sample->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
			o = m_down_sample->GetOrthoMatrix();

			m_blur_size_bitmap->Render( _context );
			m_texture_shader->Render( _context, m_blur_size_bitmap->GetIndexCount(), w, _v, o, m_blur_render_texture->GetShaderResourceView() );

			m_directx->SetBackBufferRenderTarget();
			m_directx->ResetViewport();
		}

		// RenderHorizontalBlurToTexture
		{
			m_h_blur->SetRenderTarget( _context );
			m_h_blur->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
			o = m_h_blur->GetOrthoMatrix();
			resolution.x = (float)m_h_blur->GetWidth();
			resolution.y = (float)m_h_blur->GetHeight();

			m_blur_size_bitmap->Render( _context );
			m_horizontal_blur_shader->Render( _context, m_blur_size_bitmap->GetIndexCount(), w, _v, o, m_down_sample->GetShaderResourceView(), resolution, 23, weights );

			m_directx->SetBackBufferRenderTarget();
			m_directx->ResetViewport();
		}

		// RenderVerticalBlurToTexture
		{
			m_v_blur->SetRenderTarget( _context );
			m_v_blur->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
			o = m_v_blur->GetOrthoMatrix();
			resolution.x = (float)m_v_blur->GetWidth();
			resolution.y = (float)m_v_blur->GetHeight();

			m_blur_size_bitmap->Render( _context );
			m_vertical_blur_shader->Render( _context, m_blur_size_bitmap->GetIndexCount(), w, _v, o, m_h_blur->GetShaderResourceView(), resolution, 23, weights );

			m_directx->SetBackBufferRenderTarget();
			m_directx->ResetViewport();
		}

		// UpSampleTexture
		{
			m_up_sample->SetRenderTarget( _context );
			m_up_sample->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
			o = m_up_sample->GetOrthoMatrix();
			m_screen_size_bitmap->Render( _context );
			m_texture_shader->Render( _context, m_screen_size_bitmap->GetIndexCount(), w, _v, o, m_v_blur->GetShaderResourceView() );

			m_directx->SetBackBufferRenderTarget();
			m_directx->ResetViewport();
		}

		// Render2DTextureScene
		{
			m_directx->GetOrthoMatrix( o );
			m_screen_size_bitmap->Render( _context );
			m_texture_shader->Render( _context, m_screen_size_bitmap->GetIndexCount(), w, _v, o, m_up_sample->GetShaderResourceView() );
		}

		m_directx->TurnOnZBuffer();

		m_directx->TurnOffAlphaBlending();
	}

	void Graphics::DrawTessellationScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX w;
		m_directx->GetWorldMatrix( w );

		m_rt10->SetRenderTarget( _context );
		m_rt10->ClearRenderTarget( _context, 0.1f, 0.1f, 0.1f, 1.0f );

		m_directx->SetRasterizerStateWireframe();

		m_color_model->RenderTessellation( _context );
		m_tessellation_color_shader->RenderTessellation( _context, m_color_model->GetIndexCount(), w, _v, _p, 6.0f );

		m_directx->SetRasterizerStateSolid();
	}

	void Graphics::DrawWaterScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX w;
		XMMATRIX v;

		m_directx->GetWorldMatrix( w );
		m_camera->SetPosition( -10.0f, 6.0f, -10.0f );
		m_camera->SetRotation( 0.0f, 45.0f, 0.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( v );

		// render refract
		{
			XMFLOAT4 clip_plane{ 0.0f, -1.0f, 0.0f, m_water_height + 0.1f };

			m_water_refract_texture->SetRenderTarget( _context );
			m_water_refract_texture->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

			XMMATRIX t = XMMatrixTranslation( 0.0f, 2.0f, 0.0f );

			m_water_bath->Render( _context );
			m_refract_shader->Render( _context, m_water_bath->GetIndexCount(), t, v, _p, m_water_bath->GetTexture(), m_water_light.GetDirection(), m_water_light.GetAmbient(), m_water_light.GetDiffuse(), clip_plane );
			m_directx->SetBackBufferRenderTarget();
		}

		// render reflect
		{
			XMMATRIX r;

			m_water_reflect_texture->SetRenderTarget( _context );
			m_water_reflect_texture->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

			m_camera->RenderReflect( m_water_height );
			m_camera->GetReflectionMatrix( r );

			XMMATRIX t = XMMatrixTranslation( 0.0f, 6.0f, 8.0f );

			m_water_wall->Render( _context );
			m_light_shader->Render( _context, m_water_wall->GetIndexCount(), t, r, _p, m_water_wall->GetTexture(), m_water_light.GetAmbient(), m_water_light.GetDiffuse(), 1.0f, m_water_light.GetSpecular(), m_camera->GetPosition(), m_water_light.GetDirection() );

			m_directx->SetBackBufferRenderTarget();
		}

		// render scene
		{
			XMMATRIX t;
			XMMATRIX r;

			m_rt11->SetRenderTarget( _context );
			m_rt11->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

			t = XMMatrixTranslation( 0.0f, 1.0f, 0.0f );
			m_water_ground->Render( _context );
			m_light_shader->Render( _context, m_water_ground->GetIndexCount(), t, v, _p, m_water_ground->GetTexture(), m_water_light.GetAmbient(), m_water_light.GetDiffuse(), 1.0f, m_water_light.GetSpecular(), m_camera->GetPosition(), m_water_light.GetDirection() );

			t = XMMatrixTranslation( 0.0f, 6.0f, 8.0f );
			m_water_wall->Render( _context );
			m_light_shader->Render( _context, m_water_wall->GetIndexCount(), t, v, _p, m_water_wall->GetTexture(), m_water_light.GetAmbient(), m_water_light.GetDiffuse(), 1.0f, m_water_light.GetSpecular(), m_camera->GetPosition(), m_water_light.GetDirection() );

			t = XMMatrixTranslation( 0.0f, 2.0f, 0.0f );
			m_water_bath->Render( _context );
			m_light_shader->Render( _context, m_water_bath->GetIndexCount(), t, v, _p, m_water_bath->GetTexture(), m_water_light.GetAmbient(), m_water_light.GetDiffuse(), 1.0f, m_water_light.GetSpecular(), m_camera->GetPosition(), m_water_light.GetDirection() );

			m_camera->GetReflectionMatrix( r );
			t = XMMatrixTranslation( 0.0f, m_water_height, 0.0f );
			m_water_shader->Render( _context, m_water->GetIndexCount(), t, v, _p, r,
				m_water_reflect_texture->GetShaderResourceView(),
				m_water_refract_texture->GetShaderResourceView(),
				m_water->GetTexture(),
				m_water_translation, 0.01f );
		}

		m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
		m_camera->SetRotation( 0.0f, 0.0f, 0.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( v );

		m_directx->SetBackBufferRenderTarget();
	}

	void Graphics::DrawDepthScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		m_rt12->SetRenderTarget( _context );
		m_rt12->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );


		XMMATRIX w;
		m_directx->GetWorldMatrix( w );

		XMMATRIX v;
		m_camera->SetPosition( 0.0f, 2.0f, -10.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( v );

		m_floor->Render( _context );
		m_depth_shader->Render( _context, m_floor->GetIndexCount(), w, v, _p );

		m_directx->SetBackBufferRenderTarget();
	}

	void Graphics::DrawGlassScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX v;

		m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
		m_camera->Render();

		m_camera->GetViewMatrix( v );

		m_glass_render_texture->SetRenderTarget( _context );
		m_glass_render_texture->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, v, _p, m_rastertek_model->GetTexture() );

		m_rt13->SetRenderTarget( _context );
		m_rt13->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, v, _p, m_rastertek_model->GetTexture() );

		XMMATRIX t = XMMatrixTranslation( 0.0f, 0.0f, -1.5f );
		m_rastertek_model->Render( _context );
		m_glass_shader->Render( _context, m_glass_bitmap->GetIndexCount(), t, v, _p,
			m_glass_texture->GetTexture(),
			m_glass_bump_texture->GetTexture(),
			m_glass_render_texture->GetShaderResourceView(),
			0.01f
		);

		m_directx->SetBackBufferRenderTarget();
	}

	void Graphics::DrawIceScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX v;

		m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
		m_camera->Render();

		m_camera->GetViewMatrix( v );

		m_glass_render_texture->SetRenderTarget( _context );
		m_glass_render_texture->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, v, _p, m_rastertek_model->GetTexture() );

		m_rt14->SetRenderTarget( _context );
		m_rt14->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		m_rastertek_model->Render( _context );
		m_texture_shader->Render( _context, m_rastertek_model->GetIndexCount(), _w, v, _p, m_rastertek_model->GetTexture() );

		XMMATRIX t = XMMatrixTranslation( 0.0f, 0.0f, -1.5f );
		m_rastertek_model->Render( _context );
		m_glass_shader->Render( _context, m_glass_bitmap->GetIndexCount(), t, v, _p,
			m_ice_texture->GetTexture(),
			m_ice_bump_texture->GetTexture(),
			m_glass_render_texture->GetShaderResourceView(),
			0.1f
		);

		m_directx->SetBackBufferRenderTarget();
	}

	void Graphics::DrawInstanceScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		m_rt15->SetRenderTarget( _context );
		m_rt15->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		m_triangles->Render( _context );
		m_instance_texture_shader->Render( _context, m_triangles->GetVertexCount(), m_triangles->GetInstanceCount(), _w, _v, _p, m_triangles->GetTexture() );

		m_directx->SetBackBufferRenderTarget();
	}

	void Graphics::DrawProjectiveTextureScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		XMMATRIX v;
		m_camera->SetPosition( 0.0f, 7.0f, -10.0f );
		m_camera->SetRotation( 35.0f, 0.0f, 0.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( v );

		m_rt16->SetRenderTarget( _context );
		m_rt16->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );
		XMMATRIX v2 = m_viewpoint.GetViewMatrix();
		XMMATRIX p2 = m_viewpoint.GetProjectionMatrix();

		XMMATRIX t = XMMatrixTranslation( 0.0f, 1.0f, 0.0f );

		m_floor->Render( _context );
		m_projection_shader->Render(
			_context,
			m_floor->GetIndexCount(),
			t, v, _p,
			m_floor->GetTexture(),
			m_projection_light.GetAmbient(),
			m_projection_light.GetDiffuse(),
			m_projection_light.GetDirection(),
			v2, p2,
			m_dx11_texture->GetTexture()
		);

		t = XMMatrixTranslation( 0.0f, 2.0f, 0.0f );

		m_rastertek_model->Render( _context );
		m_projection_shader->Render(
			_context,
			m_rastertek_model->GetIndexCount(),
			t, v, _p,
			m_water_ground->GetTexture(),
			m_projection_light.GetAmbient(),
			m_projection_light.GetDiffuse(),
			m_projection_light.GetDirection(),
			v2, p2,
			m_dx11_texture->GetTexture()
		);

		m_directx->SetBackBufferRenderTarget();

		m_camera->SetPosition( 0.0f, 0.0f, 0.0f );
		m_camera->SetRotation( 0.0f, 0.0f, 0.0f );
		m_camera->Render();
	}

	void Graphics::DrawLightProjectiveTextureScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p )
	{
		m_rt17->SetRenderTarget( _context );
		m_rt17->ClearRenderTarget( _context, 0.0f, 0.0f, 0.0f, 1.0f );

		XMMATRIX v;
		m_camera->SetPosition( 0.0f, 7.0f, -10.0f );
		m_camera->SetRotation( 35.0f, 0.0f, 0.0f );
		m_camera->Render();
		m_camera->GetViewMatrix( v );

		XMMATRIX v2 = m_viewpoint.GetViewMatrix();
		XMMATRIX p2 = m_viewpoint.GetProjectionMatrix();
		XMMATRIX t;

		t = XMMatrixTranslation( 0.0f, 1.0f, 0.0f );
		m_floor->Render( _context );
		m_light_projection_shader->Render(
			_context, m_floor->GetIndexCount(),
			t, _v, _p,
			m_floor->GetTexture(),
			m_projection_light2.GetAmbient(),
			m_projection_light2.GetDiffuse(),
			m_projection_light2.GetPosition(),
			v2, p2,
			m_grate_texture->GetTexture()
		);

		t = XMMatrixTranslation( 0.0f, 2.0f, 0.0f );
		m_rastertek_model->Render( _context );
		m_light_projection_shader->Render(
			_context, m_rastertek_model->GetIndexCount(),
			t, _v, _p,
			m_rastertek_model->GetTexture(),
			m_projection_light2.GetAmbient(),
			m_projection_light2.GetDiffuse(),
			m_projection_light2.GetPosition(),
			v2, p2,
			m_grate_texture->GetTexture()
		);

		m_directx->SetBackBufferRenderTarget();

		m_camera->SetPosition( 0.0f, 0.0f, 0.0f );
		m_camera->SetRotation( 0.0f, 0.0f, 0.0f );
		m_camera->Render();
	}
}