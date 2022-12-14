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
#include "Viewpoint.h"

namespace wf
{
	class Texture;

	class ColorShader;
	class TessellationColorShader;
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
	class MultiLightShader;
	class FireShader;
	class HorizontalBlurShader;
	class VerticalBlurShader;
	class WaterShader;
	class RefractShader;
	class DepthShader;
	class GlassShader;
	class InstanceTextureShader;
	class ProjectionShader;
	class LightProjectionShader;
	class ShadowShader;
	class GlowmapShader;
	class GlowShader;

	class Quad;
	class TextureQuad;
	class TangentSpaceQuad;
}

namespace wf
{
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 1000.0f;
	const float SCREEN_NEAR = 1.0f;

	const int SHADOWMAP_WIDTH = 1024;
	const int SHADOWMAP_HEIGHT = 1024;

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

	constexpr int QUAD_COUNT = 22;

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
		bool InitializeRenderTexture( ID3D11Device*& _device, int _width, int _height );
		void ShutdownRenderTexture();
		bool InitializeShader( ID3D11Device*& _device, HWND _hwnd );
		void ShutdownShader();

		void FrameFade( float _time );

		void Draw2DResult( XMMATRIX& w, XMMATRIX& v, XMMATRIX& o, float _delta );
		void DrawCubeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawFogScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawTransparencyScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawReflectScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawFadeScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawMultiLightScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawFireScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawBlurScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawTessellationScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawWaterScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawDepthScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawGlassScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawIceScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawInstanceScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawProjectiveTextureScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawLightProjectiveTextureScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );
		void DrawGlowScene( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _dsv, const XMMATRIX& _w, const XMMATRIX& _v, const XMMATRIX& _p );

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
		RasterTekModel* m_plane{ nullptr };
		RasterTekModel* m_fire{ nullptr };
		InstanceModel* m_triangles{ nullptr };

		ColorShader* m_color_shader{ nullptr };
		TessellationColorShader* m_tessellation_color_shader{ nullptr };
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
		MultiLightShader* m_multi_light_shader{ nullptr };
		FireShader* m_fire_shader{ nullptr };
		HorizontalBlurShader* m_horizontal_blur_shader{ nullptr };
		VerticalBlurShader* m_vertical_blur_shader{ nullptr };
		WaterShader* m_water_shader{ nullptr };
		RefractShader* m_refract_shader{ nullptr };
		DepthShader* m_depth_shader{ nullptr };
		GlassShader* m_glass_shader{ nullptr };
		InstanceTextureShader* m_instance_texture_shader{ nullptr };
		ProjectionShader* m_projection_shader{ nullptr };
		LightProjectionShader* m_light_projection_shader{ nullptr };
		ShadowShader* m_shadow_shader{ nullptr };
		GlowmapShader* m_glowmap_shader{ nullptr };
		GlowShader* m_glow_shader{ nullptr };
		
		RenderTexture* m_rt1{ nullptr };
		RenderTexture* m_rt2{ nullptr };
		RenderTexture* m_rt3{ nullptr };
		RenderTexture* m_rt4{ nullptr };
		RenderTexture* m_rt5{ nullptr };
		RenderTexture* m_rt6{ nullptr };
		RenderTexture* m_rt7{ nullptr };
		RenderTexture* m_rt8{ nullptr };
		RenderTexture* m_rt9{ nullptr };
		RenderTexture* m_rt10{ nullptr };
		RenderTexture* m_rt11{ nullptr };
		RenderTexture* m_rt12{ nullptr };
		RenderTexture* m_rt13{ nullptr };
		RenderTexture* m_rt14{ nullptr };
		RenderTexture* m_rt15{ nullptr };
		RenderTexture* m_rt16{ nullptr };
		RenderTexture* m_rt17{ nullptr };
		
		std::array<TextureArray*, TEXTURE_ARRAY_COUNT> m_texture_arrays;
		std::array<Quad*, QUAD_COUNT> m_quads;

		// fire
		Texture* m_blue_texture{ nullptr };
		Texture* m_seafloor_texture{ nullptr };
		Texture* m_fire_texture{ nullptr };
		Texture* m_fire_noise_texture{ nullptr };
		Texture* m_fire_alpha_texture{ nullptr };
		Texture* m_glass_texture{ nullptr };
		Texture* m_glass_bump_texture{ nullptr };
		Texture* m_ice_texture{ nullptr };
		Texture* m_ice_bump_texture{ nullptr };
		Texture* m_dx11_texture{ nullptr };
		Texture* m_grate_texture{ nullptr };

		Text* m_text{ nullptr };

		Bitmap* m_blur_bitmap{ nullptr };

		// water
		RasterTekModel* m_water_ground{ nullptr };
		RasterTekModel* m_water_wall{ nullptr };
		RasterTekModel* m_water_bath{ nullptr };
		RasterTekModel* m_water{ nullptr };
		Light m_water_light;
		RenderTexture* m_water_refract_texture{ nullptr };
		RenderTexture* m_water_reflect_texture{ nullptr };
		float m_water_height{};
		float m_water_translation{};

		// blur
		RenderTexture* m_down_sample{ nullptr };
		RenderTexture* m_up_sample{ nullptr };
		RenderTexture* m_h_blur{ nullptr };
		RenderTexture* m_v_blur{ nullptr };
		RenderTexture* m_blur_render_texture{ nullptr };

		// glass and ice
		RenderTexture* m_glass_render_texture{ nullptr };

		RenderTargetBitmap* m_blur_size_bitmap{ nullptr };
		RenderTargetBitmap* m_screen_size_bitmap{ nullptr };
		RenderTargetBitmap* m_glass_bitmap{ nullptr };

		// fade
		float m_fade_time{ 0.0f };
		float m_fade_rate{ 0.0f };
		bool m_fade_finish{ false };

		// Projection
		Light m_projection_light;

		// LightProjection
		Light m_projection_light2;

		// mouse cursor
		Bitmap* m_cursor{ nullptr };
		int m_mouse_x{ 0 };
		int m_mouse_y{ 0 };
		int m_width{ 0 };
		int m_height{ 0 };

		// Shadow
		RasterTekModel* m_sphere{ nullptr };
		Light m_shadow_light;
		RenderTexture* m_shadow_depth_texture{ nullptr };

		// Glow
		RenderTexture* m_glow_render_texture{ nullptr };
		Bitmap* m_glow_bitmap{ nullptr };
		Texture* m_glow_texture{ nullptr };
		Texture* m_glowmap_texture{ nullptr };


		Viewpoint m_viewpoint;
	};
}