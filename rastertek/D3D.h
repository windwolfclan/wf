#pragma once

namespace wf
{
	class D3D
	{
	public:
		D3D();
		D3D( const D3D& );
		~D3D();

		bool Initialize( int _width, int _height, bool _vsync, HWND _hwnd, bool _fullscreen, float _far, float _near );
		void Shutdown();

		void BeginScene( float _r, float _g, float _b, float _a );
		void EndScene();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void GetProjectionMatrix( XMMATRIX& _m );
		void GetWorldMatrix( XMMATRIX& _m );
		void GetOrthoMatrix( XMMATRIX& _m );

		void GetVideoCardInfo( char* _buffer, int& _memory );

		void TurnOnZBuffer();
		void TurnOffZBuffer();

		void TurnOnAlphaBlending();
		void TurnOffAlphaBlending();

		int GetWidth() const;
		int GetHeight() const;

		ID3D11DepthStencilView* GetDepthStencilView();
		void SetBackBufferRenderTarget();
		void ResetViewport();

		void SetRasterizerStateSolid();
		void SetRasterizerStateWireframe();

	private:
		bool m_vsync{ false };
		
		int m_vga_memory{ 0 };
		char m_vga_name[ MAX_PATH ];

		int m_width{ 0 };
		int m_height{ 0 };

		IDXGISwapChain* m_swapchain{ nullptr };
		ID3D11Device* m_device{ nullptr };
		ID3D11DeviceContext* m_device_context{ nullptr };
		ID3D11RenderTargetView* m_render_target_view{ nullptr };
		ID3D11Texture2D* m_depth_stencil_buffer{ nullptr };
		ID3D11DepthStencilState* m_depth_stencil_state{ nullptr };
		ID3D11DepthStencilView* m_depth_stencil_view{ nullptr };
		ID3D11RasterizerState* m_raster_state{ nullptr };
		ID3D11RasterizerState* m_wire_frame{ nullptr };
		ID3D11DepthStencilState* m_depth_disabled_stencil_state{ nullptr };
		ID3D11BlendState* m_alpha_enable_blend_state{ nullptr };
		ID3D11BlendState* m_alpha_disable_blend_state{ nullptr };
	public:
		ID3D11BlendState* m_multiply_blend_state{ nullptr };

		D3D11_VIEWPORT m_viewport;
		XMMATRIX m_projection_matrix{};
		XMMATRIX m_world_matrix{};
		XMMATRIX m_ortho_matrix{};
	};
}