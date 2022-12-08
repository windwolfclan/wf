#pragma once

#include <Windows.h>

#include "D3D.h"
#include "Camera.h"
#include "Model.h"


#include "ColorShader.h"
#include "TextureShader.h"

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
		
		
		ColorModel* m_color_model{ nullptr };
		ColorShader* m_color_shader{ nullptr };

		TextureModel* m_texture_model{ nullptr };
		TextureShader* m_texture_shader{ nullptr };
	};
}