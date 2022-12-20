#pragma once

namespace wf
{
	class RenderTexture
	{
	public:
		RenderTexture();
		RenderTexture( const RenderTexture& );
		~RenderTexture();

		bool Initialize( ID3D11Device* _device, int _width, int _height );
		void Shutdown();

		void SetRenderTarget( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _depth_stencil_view );
		void ClearRenderTarget( ID3D11DeviceContext* _context, ID3D11DepthStencilView* _depth_stencil_view, float _r, float _g, float _b, float _a );
		ID3D11ShaderResourceView* GetShaderResourceView();

	private:
		ID3D11Texture2D* m_render_target_texture{ nullptr };
		ID3D11RenderTargetView* m_render_target_view{ nullptr };
		ID3D11ShaderResourceView* m_shader_resource_view{ nullptr };
	};
}