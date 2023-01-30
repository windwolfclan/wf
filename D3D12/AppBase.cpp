#include "pch.h"
#include "AppBase.h"

NAMESPACE_WF_S

AppBase* AppBase::m_app = nullptr;
AppBase* AppBase::GetApp()
{
	return m_app;
}

LRESULT CALLBACK MainWindowProc( HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam )
{
	return AppBase::GetApp()->MsgProc( _hwnd, _msg, _wparam, _lparam );
}

AppBase::AppBase( HINSTANCE _instance )
	: m_instance( _instance )
{
	m_app = this;
}

AppBase::~AppBase()
{
	if ( context )
	{
		context->TerminateContext();
	}
}

HINSTANCE AppBase::GetInstance() const
{
	return m_instance;
}

HWND AppBase::GetWindowHandle() const
{
	return m_wnd;
}

float AppBase::GetAspectRatio() const
{
	return static_cast<float>( m_client_width ) / m_client_height;
}

int AppBase::Run()
{
	MSG msg{ 0, };

	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			m_game_timer.Tick();

			if ( m_app_paused )
			{
				Sleep( 100 );
			}
			else
			{
				CalculateFrameStats();
				Update( m_game_timer );
				Render( m_game_timer );
			}
		}
	}

	return (int)msg.wParam;
}

bool AppBase::Initialize()
{
	if ( !m_game_timer.Initialize() )
	{
		return false;
	}

	if ( !InitializeMainWindow() )
	{
		return false;
	}

	context = std::make_shared<D3D12Context>( m_wnd, m_client_width, m_client_height );
	if ( !context )
	{
		return false;
	}

	if ( !context->InitializeContext() )
	{
		return false;
	}
	
	context->OnResize( m_client_width, m_client_height );

	return true;
}

LRESULT AppBase::MsgProc( HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam )
{
	switch ( _msg )
	{
		case WM_ACTIVATE:
		{
			if ( WA_INACTIVE == LOWORD( _wparam ) )
			{
				m_app_paused = true;
				m_game_timer.Stop();
			}
			else
			{
				m_app_paused = false;
				m_game_timer.Start();
			}

			return 0;
		}

		case WM_SIZE:
		{
			m_client_width = LOWORD( _lparam );
			m_client_height = HIWORD( _lparam );

			if ( context )
			{
				switch ( _wparam )
				{
					case SIZE_MINIMIZED:
						m_app_paused = true;
						m_minimized = true;
						m_maximized = false;
					break;

					case SIZE_MAXIMIZED:
						m_app_paused = true;
						m_minimized = false;
						m_maximized = true;

						context->OnResize( m_client_width, m_client_height );
					break;

					case SIZE_RESTORED:
						if ( m_minimized )
						{
							m_app_paused = false;
							m_minimized = false;
							context->OnResize( m_client_width, m_client_height );
						}
						else if ( m_maximized )
						{
							m_app_paused = false;
							m_maximized = false;
							context->OnResize( m_client_width, m_client_height );
						}
						else if ( m_resizing )
						{

						}
						else
						{
							context->OnResize( m_client_width, m_client_height );
						}
					break;
				}
			}

			return 0;
		}

		case WM_ENTERSIZEMOVE:
		{
			m_app_paused = true;
			m_resizing = true;
			m_game_timer.Stop();
			return 0;
		}

		case WM_EXITSIZEMOVE:
		{
			m_app_paused = false;
			m_resizing = false;
			m_game_timer.Start();
			if ( context )
			{
				context->OnResize( m_client_width, m_client_height );
			}
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}

		case WM_GETMINMAXINFO:
		{
			( (MINMAXINFO*)_lparam )->ptMinTrackSize.x = 200;
			( (MINMAXINFO*)_lparam )->ptMinTrackSize.y = 200;
			return 0;
		}

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{

			OnMouseDown( _wparam, GET_X_LPARAM( _lparam ), GET_Y_LPARAM( _lparam ) );
			return 0;
		}

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
			OnMouseUp( _wparam, GET_X_LPARAM( _lparam ), GET_Y_LPARAM( _lparam ) );
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			OnMouseMove( _wparam, GET_X_LPARAM( _lparam ), GET_Y_LPARAM( _lparam ) );
			return 0;
		}

		case WM_KEYUP:
		{
			switch ( _wparam )
			{
				case VK_ESCAPE:
				{
					PostQuitMessage( 0 );
					return 0;
				}
			}

			break;
		}
	}

	return DefWindowProc( _hwnd, _msg, _wparam, _lparam );
}

void AppBase::OnMouseDown( WPARAM _btn, int _x, int _y )
{
	__noop;
}

void AppBase::OnMouseUp( WPARAM _btn, int _x, int _y )
{
	__noop;
}

void AppBase::OnMouseMove( WPARAM _btn, int _x, int _y )
{
	__noop;
}

bool AppBase::InitializeMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_instance;
	wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
	wc.hCursor = LoadCursor( 0, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"WindWolfApp";

	if ( !RegisterClass( &wc ) )
	{
		MessageBox( 0, L"RegisterClass Failed.", 0, 0 );
		return false;
	}


	RECT R = { 0, 0, m_client_width, m_client_height };
	AdjustWindowRect( &R, WS_OVERLAPPEDWINDOW, false );

	int w = R.right - R.left;
	int h = R.bottom - R.top;
	int x = ( GetSystemMetrics( SM_CXSCREEN ) - w ) / 2;
	int y = ( GetSystemMetrics( SM_CYSCREEN ) - h ) / 2;

	m_wnd = CreateWindow(
		L"WindWolfApp",
		m_window_caption.c_str(),
		WS_OVERLAPPEDWINDOW,
		x, y,
		w, h,
		nullptr,
		nullptr,
		m_instance,
		nullptr
	);

	if ( !m_wnd )
	{
		MessageBox( 0, L"CreateWindow Failed.", 0, 0 );
		return false;
	}

	ShowWindow( m_wnd, SW_SHOW );
	UpdateWindow( m_wnd );

	return true;
}

void AppBase::CalculateFrameStats()
{
	static int frame_count{ 0 };
	static float elapsed{ 0.0f };

	elapsed += m_game_timer.GetDeltaTime();

	++frame_count;
	if ( m_game_timer.GetTotalTime() - elapsed >= 1.0f )
	{
		float fps = (float)frame_count; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring( fps );
		std::wstring mspfStr = std::to_wstring( mspf );

		std::wstring text = m_window_caption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText( m_wnd, text.c_str() );

		// Reset for next average.
		frame_count = 0;
		elapsed += 1.0f;
	}
}

NAMESPACE_WF_E