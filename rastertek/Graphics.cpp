#include "pch.h"
#include "Graphics.h"

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
		
		m_color_model = new ColorModel;
		if ( !m_color_model->Initialize( device ) )
		{
			MessageBox( _hwnd, L"Could not initialize the model object", L"Error", MB_OK );
			return false;
		}

		m_color_shader = new ColorShader;
		if ( !m_color_shader->Initialize( device, _hwnd ) )
		{
			MessageBox( _hwnd, L"Could not initialize the color shader", L"Error", MB_OK );
			return false;
		}

		m_texture_model = new TextureModel;
		if ( !m_texture_model->Initialize( device, context, "./resources/StoneFloorTexture.tga" ) )
		{
			return false;
		}

		m_texture_shader = new TextureShader;
		if ( !m_texture_shader->Initialize( device, _hwnd ) )
		{
			return false;
		}

		m_light_model = new LightModel;
		if ( !m_light_model->Initialize( device, context, "./resources/StoneFloorTexture.tga" ) )
		{
			return false;
		}

		m_light_shader = new LightShader;
		if ( !m_light_shader->Initialize( device, _hwnd ) )
		{
			return false;
		}

		m_rastertek_model = new RasterTekModel;
		if ( !m_rastertek_model->Initialize( device, context, "./resources/StoneFloorTexture.tga", "./resources/cube.txt" ) )
		{
			return false;
		}

		m_bitmap = new Bitmap;
		if ( !m_bitmap->Initialize( device, context, _width, _height, "./resources/StoneFloorTexture.tga", 256, 256 ) )
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

		m_light.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_light.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_light.SetDirection( 0.0f, 0.0f, 1.0f );
		m_light.SetSpecular( 0.0f, 1.0f, 1.0f, 1.0f );
		m_light.SetSpecularPower( 1.0f );

		return true;
	}

	void Graphics::Shutdown()
	{
		SAFE_SHUTDOWN( m_cursor );
		SAFE_SHUTDOWN( m_text );
		SAFE_SHUTDOWN( m_bitmap );
		SAFE_SHUTDOWN( m_rastertek_model );
		SAFE_SHUTDOWN( m_light_shader );
		SAFE_SHUTDOWN( m_light_model );
		SAFE_SHUTDOWN( m_texture_shader );
		SAFE_SHUTDOWN( m_texture_model );
		SAFE_SHUTDOWN( m_color_shader );
		SAFE_SHUTDOWN( m_color_model );
		SAFE_DELETE( m_camera );
		SAFE_SHUTDOWN( m_directx );
	}

	bool Graphics::Frame( int _mouse_x, int _mouse_y )
	{
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();
		m_mouse_x = _mouse_x;
		m_mouse_y = _mouse_y;

		if ( !m_text->SetMousePosition( _mouse_x, _mouse_y, context ) )
		{
			return false;
		}

		m_camera->SetPosition( 0.0f, 0.0f, -10.0f );

		return true;
	}

	bool Graphics::Render()
	{
		static float rotation{ 0.0f };
		rotation += (float)XM_PI * 0.005f;
		if ( rotation > 360.0f )
			rotation -= 360.0f;

		XMMATRIX w;
		XMMATRIX v;
		XMMATRIX p;
		XMMATRIX o;
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();

		m_camera->Render();

		// set variable
		{
			m_directx->GetWorldMatrix( w );
			m_camera->GetViewMatrix( v );
			m_directx->GetProjectionMatrix( p );
			m_directx->GetOrthoMatrix( o );

			w = XMMatrixRotationY( rotation );
		}
		
		// render
		{
			m_directx->BeginScene( 0.0f, 0.25f, 0.5f, 1.0f );

			m_rastertek_model->Render( context );

			if ( !m_light_shader->Render( 
				context, 
				m_rastertek_model->GetIndexCount(),
				w, 
				v, 
				p, 
				m_rastertek_model->GetTexture(),
				m_light.GetAmbient(),
				m_light.GetDiffuse(), 
				m_light.GetSpecularPower(),
				m_light.GetSpecular(),
				m_camera->GetPosition(),
				m_light.GetDirection()
			) )
			{
				return false;
			}

			// 2D Draw
			m_directx->TurnOffZBuffer();
			if ( !m_bitmap->Render( context, 100, 100 ) )
			{
				return false;
			}
			
			m_directx->GetWorldMatrix( w );

				
			if ( !m_texture_shader->Render(
				context,
				m_bitmap->GetIndexCount(),
				w,
				v,
				o,
				m_bitmap->GetTexture()
			) )
			{
				return false;
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

		if ( x + CURSOR_SIZE > width )
			x = width - CURSOR_SIZE;

		if ( y + CURSOR_SIZE > height )
			y = height - CURSOR_SIZE;

		m_cursor->Render( context, x, y );
		m_texture_shader->Render( context, m_cursor->GetIndexCount(), w, v, o, m_cursor->GetTexture() );
	}
}