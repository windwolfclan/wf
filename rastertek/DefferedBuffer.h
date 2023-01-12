#pragma once

namespace wf
{
	constexpr int BUFFER_COUNT = 2;

	class DefferedBuffer
	{
	public:
		bool Initialize( ID3D11Device* _device, int _width, int _height, float _near_z, float _far_z );
		void Shutdown();

		void SetRenderTargets( ID3D11DeviceContext* _context );
		void ClearRenderTargets( ID3D11DeviceContext* _context, float _r, float _g, float _b, float _a );

		ID3D11ShaderResourceView* GetShaderResourceView( int _index );

	private:
		int m_width{ 0 };
		int m_height{ 0 };

		ID3D11Texture2D* m_render_target_textures[ BUFFER_COUNT ]{ nullptr, nullptr };
		ID3D11RenderTargetView* m_render_target_views[ BUFFER_COUNT ]{ nullptr, nullptr };
		ID3D11ShaderResourceView* m_shader_resource_views[ BUFFER_COUNT ]{ nullptr, nullptr };
	
		ID3D11Texture2D* m_depth_stencil_texture{ nullptr };
		ID3D11DepthStencilView* m_depth_stencil_view{ nullptr };
		D3D11_VIEWPORT m_viewport;
	};
}