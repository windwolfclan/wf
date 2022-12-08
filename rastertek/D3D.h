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


	private:
		bool m_vsync{ false };
		
		int m_vga_memory{ 0 };
		char m_vga_name[ MAX_PATH ];

		IDXGISwapChain* m_swapchain{ nullptr };
		ID3D11Device* m_device{ nullptr };
		ID3D11DeviceContext* m_device_context{ nullptr };
		ID3D11RenderTargetView* m_render_target_view{ nullptr };
		ID3D11Texture2D* m_depth_stencil_buffer{ nullptr };
		ID3D11DepthStencilState* m_depth_stencil_state{ nullptr };
		ID3D11DepthStencilView* m_depth_stencil_view{ nullptr };
		ID3D11RasterizerState* m_raster_state{ nullptr };

		XMMATRIX m_projection_matrix{};
		XMMATRIX m_world_matrix{};
		XMMATRIX m_ortho_matrix{};
	};
}