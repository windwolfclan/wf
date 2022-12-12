#include "pch.h"
#include "Input.h"

namespace wf
{
	Input::Input()
	{
	}

	Input::Input( const Input& )
	{
	}

	Input::~Input()
	{
	}

	bool Input::Initialize( HINSTANCE hInstance, HWND _hwnd, int _screen_width, int _screen_height )
	{
		m_screen_width = _screen_width;
		m_screen_height = _screen_height;

		HRESULT hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_direct_input->CreateDevice( GUID_SysKeyboard, &m_keyboard, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_keyboard->SetDataFormat( &c_dfDIKeyboard );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_keyboard->SetCooperativeLevel( _hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_keyboard->Acquire();
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_direct_input->CreateDevice( GUID_SysMouse, &m_mouse, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_mouse->SetDataFormat( &c_dfDIMouse );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_mouse->SetCooperativeLevel( _hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_mouse->Acquire();
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void Input::Shutdown()
	{
		if ( m_mouse )
		{
			m_mouse->Unacquire();
			SAFE_RELEASE( m_mouse );
		}

		if ( m_keyboard )
		{
			m_keyboard->Unacquire();
			SAFE_RELEASE( m_keyboard );
		}

		SAFE_RELEASE( m_direct_input );
	}

	bool Input::Frame()
	{
		if ( !ReadKeyboard() )
		{
			return false;
		}

		if ( !ReadMouse() )
		{
			return false;
		}

		ProcessInput();

		return true;
	}

	bool Input::IsEscapePressed()
	{
		return m_keyboard_state[ DIK_ESCAPE ] & 0x80;
	}

	bool Input::IsSpacePressed()
	{
		return m_keyboard_state[ DIK_SPACE ] & 0x80;
	}

	void Input::GetMouseLocation( int& _x, int& _y )
	{
		_x = m_mouse_x;
		_y = m_mouse_y;
	}

	bool Input::ReadKeyboard()
	{
		HRESULT hr = m_keyboard->GetDeviceState( sizeof( m_keyboard_state ), (LPVOID)&m_keyboard_state );
		if ( FAILED( hr ) )
		{
			if ( DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr )
			{
				m_keyboard->Acquire();
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	bool Input::ReadMouse()
	{
		HRESULT hr = m_mouse->GetDeviceState( sizeof( DIMOUSESTATE ), (LPVOID)&m_mouse_state );
		if ( FAILED( hr ) )
		{
			if ( DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr )
			{
				m_mouse->Acquire();
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	void Input::ProcessInput()
	{
		m_mouse_x += m_mouse_state.lX;
		m_mouse_y += m_mouse_state.lY;

		if ( m_mouse_x < 0 )
			m_mouse_x = 0;

		if ( m_mouse_x > m_screen_width )
			m_mouse_x = m_screen_width;

		if ( m_mouse_y < 0 )
			m_mouse_y = 0;

		if ( m_mouse_y > m_screen_height )
			m_mouse_y = m_screen_height;
	}

}