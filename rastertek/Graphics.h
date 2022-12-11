#pragma once


#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Bitmap.h"

#include "ColorShader.h"
#include "TextureShader.h"
#include "LightShader.h"

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
		bool Render( float );

	private:
		D3D* m_directx{ nullptr };
		Camera* m_camera{ nullptr };
		Light m_light{};
		
		ColorModel* m_color_model{ nullptr };
		ColorShader* m_color_shader{ nullptr };

		TextureModel* m_texture_model{ nullptr };
		TextureShader* m_texture_shader{ nullptr };

		LightModel* m_light_model{ nullptr };
		LightShader* m_light_shader{ nullptr };

		RasterTekModel* m_rastertek_model{ nullptr };

		Bitmap* m_bitmap{};
	};
}