#pragma once

#include <Windows.h>

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"

namespace wf
{
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;

	class Graphics
	{
	public:
		Graphics();
		Graphics(const Graphics&);
		~Graphics();

		bool Initialize(int _width, int _height, HWND _hwnd);
		void Shutdown();
		bool Frame();

	private:
		bool Render();

	private:
		D3D* m_directx{ nullptr };
		Camera* m_camera{ nullptr };
		Model* m_model{ nullptr };
		ColorShader* m_color_shader{ nullptr };
	};
}