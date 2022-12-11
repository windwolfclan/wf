#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

#include <dinput.h>

namespace wf
{
	class Input
	{
	public:
		Input();
		Input(const Input&);
		~Input();

		bool Initialize( HINSTANCE hInstance, HWND _hwnd, int _screen_width, int _screen_height );
		void Shutdown();

		bool Frame();

		bool IsEscapePressed();
		void GetMouseLocation( int& _x, int& _y );

	private:
		bool ReadKeyboard();
		bool ReadMouse();
		void ProcessInput();

	private:
		IDirectInput8* m_direct_input{ nullptr };
		IDirectInputDevice8* m_keyboard{ nullptr };
		IDirectInputDevice8* m_mouse{ nullptr };

		bool m_keyboard_state[ 256 ];
		DIMOUSESTATE m_mouse_state{};
		
		int m_screen_width{ 0 };
		int m_screen_height{ 0 };
		int m_mouse_x{ 0 };
		int m_mouse_y{ 0 };
	};
}