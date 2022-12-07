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

		return true;
	}

	void Graphics::Shutdown()
	{
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
		m_directx->BeginScene( 0.5f, 0.5f, 0.5f, 1.0f );

		m_directx->EndScene();

		return true;
	}
}