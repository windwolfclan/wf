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

		m_camera = new Camera;
		m_camera->SetPosition( 0.0f, 0.0f, -5.0f );
		
		m_model = new Model;
		if ( !m_model->Initialize( device ) )
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

		return true;
	}

	void Graphics::Shutdown()
	{
		if ( m_color_shader )
		{
			m_color_shader->Shutdown();
			delete m_color_shader;
			m_color_shader = nullptr;
		}

		if ( m_model )
		{
			m_model->Shutdown();
			delete m_model;
			m_model = nullptr;
		}

		if ( m_camera )
		{
			delete m_camera;
			m_camera = nullptr;
		}

		if ( m_directx )
		{
			m_directx->Shutdown();
			delete m_directx;
			m_directx = nullptr;
		}
	}

	bool Graphics::Frame()
	{
		if ( !Render() )
		{
			return false;
		}

		return true;
	}

	bool Graphics::Render()
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
		}
		
		// render
		{
			m_directx->BeginScene( 0.25f, 0.25f, 0.25f, 1.0f );

			m_model->Render( context );

			if ( !m_color_shader->Render( context, m_model->GetIndexCount(), w, v, p ) )
			{
				return false;
			}

			m_directx->EndScene();
		}

		return true;
	}
}