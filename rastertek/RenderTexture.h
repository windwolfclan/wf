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

		void SetRenderTarget( ID3D11DeviceContext* _context );
		void ClearRenderTarget( ID3D11DeviceContext* _context, float _r, float _g, float _b, float _a );
		
		ID3D11ShaderResourceView* GetShaderResourceView() const;
		XMMATRIX GetProjectionMatrix() const;
		XMMATRIX GetOrthoMatrix() const;
		int GetWidth() const;
		int GetHeight() const;

	private:
		ID3D11Texture2D* m_render_target_texture{ nullptr };
		ID3D11RenderTargetView* m_render_target_view{ nullptr };
		ID3D11ShaderResourceView* m_shader_resource_view{ nullptr };

		ID3D11Texture2D* m_depth_stencil_texture{ nullptr };
		ID3D11DepthStencilView* m_depth_stencil_view{ nullptr };
		D3D11_VIEWPORT m_viewport{};

		int m_width{ 0 };
		int m_height{ 0 };
		XMMATRIX m_projection{};
		XMMATRIX m_ortho{};
	};
}