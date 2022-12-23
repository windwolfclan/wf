#pragma once

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Bitmap.h"
#include "Text.h"
#include "TextureArray.h"
#include "ModelLoader.h"
#include "RenderTexture.h"

namespace wf
{
	class Texture;

	class ColorShader;
	class TextureShader;
	class LightShader;
	class DualTextureShader;
	class LightmapShader;
	class AlphamapShader;
	class BumpShader;
	class SpecularShader;
	class FogShader;
	class TranslateShader;
	class TransparentShader;
	class FadeShader;
	class ReflectionShader;

	class Quad;
	class TextureQuad;
	class TangentSpaceQuad;
}

namespace wf
{
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 0.1f;

	struct FrameParam
	{
		int mouse_x{ 0 };
		int mouse_y{ 0 };
		int fps{ 0 };
		int usage{ 0 };
		float time{ 0.0f };
	};

	constexpr int DUAL_TEXTURE_ARRAY = 0;
	constexpr int LIGHTMAP_TEXTURE_ARRAY = 1;
	constexpr int ALPHAMAP_TEXTURE_ARRAY = 2;
	constexpr int BUMPMAP_TEXTURE_ARRAY = 3;
	constexpr int SPECULAR_TEXTURE_ARRAY = 4;
	constexpr int RENDER_TEXTURE_ARRAY = 5;
	constexpr int TEXTURE_ARRAY_COUNT = 6;

	constexpr int QUAD_COUNT = 11;

	class Graphics
	{
	public:
		Graphics();
		Graphics(const Graphics&);
		~Graphics();

		bool Initialize(int _width, int _height, HWND _hwnd);
		void Shutdown();
		bool Frame( const FrameParam& param );
		bool Render( float _delta );

	private:
		void DrawCursor();

		bool InitializeTextureArray( ID3D11Device*& _device, ID3D11DeviceContext*& _context );
		void ReleaseTextureArray();

		bool InitializeQuads( ID3D11Device*& _device );
		void ShutdownQuads();

		void FrameFade( float _time );

		void Draw2DResult( XMMATRIX& w, XMMATRIX& v, XMMATRIX& o, float _delta );
		void DrawCubeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawFogScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawTransparencyScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawReflectScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawFadeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );

	private:
		D3D* m_directx{ nullptr };
		Camera* m_camera{ nullptr };
		Light m_light{};

		ModelLoader* m_model_loader{ nullptr };
		
		ColorModel* m_color_model{ nullptr };
		TextureModel* m_texture_model{ nullptr };
		LightModel* m_light_model{ nullptr };
		RasterTekModel* m_rastertek_model{ nullptr };
		RasterTekModel* m_floor{ nullptr };

		ColorShader* m_color_shader{ nullptr };
		TextureShader* m_texture_shader{ nullptr };
		LightShader* m_light_shader{ nullptr };
		DualTextureShader* m_dual_texture_shader{ nullptr };
		LightmapShader* m_lightmap_shader{ nullptr };
		AlphamapShader* m_alphamap_shader{ nullptr };
		BumpShader* m_bump_shader{ nullptr };
		SpecularShader* m_specular_shader{ nullptr };
		FogShader* m_fog_shader{ nullptr };
		TranslateShader* m_translate_shader{ nullptr };
		TransparentShader* m_transparent_shader{ nullptr };
		FadeShader* m_fade_shader{ nullptr };
		ReflectionShader* m_reflection_shader{ nullptr };
		
		RenderTexture* m_rt1{ nullptr };
		RenderTexture* m_rt2{ nullptr };
		RenderTexture* m_rt3{ nullptr };
		RenderTexture* m_rt6{ nullptr };
		RenderTexture* m_rt4{ nullptr };
		RenderTexture* m_rt5{ nullptr };
		std::array<TextureArray*, TEXTURE_ARRAY_COUNT> m_texture_arrays;
		std::array<Quad*, QUAD_COUNT> m_quads;

		Texture* m_blue_texture{ nullptr };
		Texture* m_seafloor_texture{ nullptr };

		Text* m_text{ nullptr };

		// fade
		float m_fade_time{ 0.0f };
		float m_fade_rate{ 0.0f };
		bool m_fade_finish{ false };
		

		// mouse cursor
		Bitmap* m_cursor{ nullptr };
		int m_mouse_x{ 0 };
		int m_mouse_y{ 0 };
		int m_width{ 0 };
		int m_height{ 0 };
	};
}