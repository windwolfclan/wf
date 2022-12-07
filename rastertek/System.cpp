#include "System.h"

namespace wf
{
	System::System()
	{
	}

	System::System( const System& )
	{
	}

	System::~System()
	{
	}

	bool System::Initialize()
	{
		int width{ 0 };
		int height{ 0 };
		bool result{ false };

		InitializeWindows( width, height );

		m_input = new Input();
		m_input->Initialize();

		m_graphics = new Graphics();
		if ( !m_graphics->Initialize( width, height, m_hwnd ) )
		{
			return false;
		}

		return true;
	}

	void System::Shutdown()
	{
		if ( m_graphics )
		{
			m_graphics->Shutdown();
			delete m_graphics;
			m_graphics = nullptr;
		}

		if ( m_input )
		{
			delete m_input;
			m_input = nullptr;
		}

		ShutdownWindows();
	}

	int System::Run()
	{
		MSG msg{ 0, };
		bool done{ false };

		ZeroMemory( &msg, sizeof( msg ) );

		while ( !done )
		{
			if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			if ( WM_QUIT == msg.message )
			{
				done = true;
			}
			else
			{
				if ( !Frame() )
				{
					done = true;
				}
			}
		}

		return 0;
	}

	LRESULT System::MessageHandler( HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam )
	{
		switch ( _msg )
		{
			case WM_KEYDOWN:
			{
				m_input->KeyDown( (unsigned int)_wparam );
				return 0;
			}

			case WM_KEYUP:
			{
				m_input->KeyUp( (unsigned int)_wparam );
				return 0;
			}

			default:
			{
				return DefWindowProc( _hwnd, _msg, _wparam, _lparam );
			}
		}
	}

	bool System::Frame()
	{
		if ( m_input->IsKeyDown( VK_ESCAPE ) )
		{
			return false;
		}

		if ( !m_graphics->Frame() )
		{
			return false;
		}

		return true;
	}

	void System::InitializeWindows( int& _width, int& _height )
	{
		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;


		// Get an external pointer to this object.	
		g_app = this;

		// Get the instance of this application.
		m_hinstance = GetModuleHandle( NULL );

		// Give the application a name.
		m_applicationName = L"Engine";

		// Setup the windows class with default settings.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hinstance;
		wc.hIcon = LoadIcon( nullptr, IDI_WINLOGO );
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
		wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = m_applicationName;
		wc.cbSize = sizeof( WNDCLASSEX );

		// Register the window class.
		RegisterClassEx( &wc );

		// Determine the resolution of the clients desktop screen.
		_width = GetSystemMetrics( SM_CXSCREEN );
		_height = GetSystemMetrics( SM_CYSCREEN );

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if ( FULL_SCREEN )
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
			dmScreenSettings.dmSize = sizeof( dmScreenSettings );
			dmScreenSettings.dmPelsWidth = (unsigned long)_width;
			dmScreenSettings.dmPelsHeight = (unsigned long)_height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen.
			ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );

			// Set the position of the window to the top left corner.
			posX = posY = 0;
		}
		else
		{
			// If windowed then set it to 800x600 resolution.
			_width = 1600;
			_height = 900;

			// Place the window in the middle of the screen.
			posX = ( GetSystemMetrics( SM_CXSCREEN ) - _width ) / 2;
			posY = ( GetSystemMetrics( SM_CYSCREEN ) - _height ) / 2;
		}

		// Create the window with the screen settings and get the handle to it.
		m_hwnd = CreateWindowEx( WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, _width, _height, nullptr, nullptr, m_hinstance, nullptr );

		// Bring the window up on the screen and set it as main focus.
		ShowWindow( m_hwnd, SW_SHOW );
		SetForegroundWindow( m_hwnd );
		SetFocus( m_hwnd );

	}

	void System::ShutdownWindows()
	{
		if ( FULL_SCREEN )
		{
			ChangeDisplaySettings( nullptr, 0 );
		}

		DestroyWindow( m_hwnd );
		m_hwnd = nullptr;

		UnregisterClass( m_applicationName, m_hinstance );
		m_hinstance = nullptr;

		g_app = nullptr;

	}

	LRESULT WndProc( HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam )
	{
		switch ( _msg )
		{
			case WM_DESTROY:
			{
				PostQuitMessage( 0 );
				return 0;
			}

			case WM_CLOSE:
			{
				PostQuitMessage( 0 );
				return 0;
			}

			default:
			{
				return g_app->MessageHandler( _hwnd, _msg, _wparam, _lparam );
			}
		}
	}
}
