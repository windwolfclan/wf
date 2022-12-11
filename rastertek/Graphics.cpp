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

		m_camera = new Camera;
		m_camera->SetPosition( 0.0f, 0.0f, -10.0f );
		
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


		m_light.SetAmbient( 0.15f, 0.15f, 0.15f, 1.0f );
		m_light.SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
		m_light.SetDirection( 1.0f, 0.0f, 0.0f );

		return true;
	}

	void Graphics::Shutdown()
	{
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

	bool Graphics::Frame()
	{
		static float rotation{ 0.0f };

		rotation += (float)XM_PI * 0.005f;
		if ( rotation > 360.0f )
			rotation -= 360.0f;

		static float b = 0.0f;

		b += (float)XM_PI * 0.005f;
		if ( b > 1.0f )
			b -= 1.0f;

		if ( !Render( rotation ) )
		{
			return false;
		}

		return true;
	}

	bool Graphics::Render( float rotation )
	{
		XMMATRIX w;
		XMMATRIX v;
		XMMATRIX p;
		ID3D11DeviceContext* context = m_directx->GetDeviceContext();

		m_camera->Render();

		// set variable
		{
			m_directx->GetWorldMatrix( w );
			m_camera->GetViewMatrix( v );
			m_directx->GetProjectionMatrix( p );

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
				m_light.GetDirection() ) 
				)
			{
				return false;
			}

			m_directx->EndScene();
		}

		return true;
	}
}