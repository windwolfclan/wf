#pragma once

#include "Graphics.h"
#include "Input.h"
#include "Sound.h"

#include "Fps.h"
#include "Cpu.h"
#include "Timer.h"

namespace wf
{
	class System
	{
	public:
		System();
		System(const System&);
		~System();

		bool Initialize();
		void Shutdown();
		int Run();

		LRESULT CALLBACK MessageHandler(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	private:
		bool Frame();
		void InitializeWindows(int& _width, int& _height);
		void ShutdownWindows();

	private:
		LPCWSTR m_applicationName;
		HINSTANCE m_hinstance;
		HWND m_hwnd;

		Input* m_input{};
		Graphics* m_graphics{};
		Sound* m_sound{ };
		Fps* m_fps{ nullptr };
		Cpu* m_cpu{ nullptr };
		Timer* m_timer{ nullptr };
	};

	static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	static System* g_app{ nullptr };
}

