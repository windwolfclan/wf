#include "pch.h"
#include "D3D.h"

namespace wf
{
	D3D::D3D()
	{
	}

	D3D::D3D( const D3D& )
	{
	}

	D3D::~D3D()
	{
	}

	bool D3D::Initialize( int _width, int _height, bool _vsync, HWND _hwnd, bool _fullscreen, float _far, float _near )
	{
		m_width = _width;
		m_height = _height;
		m_vsync = _vsync;
		
		HRESULT hr{ S_OK };
		IDXGIFactory* factory{ nullptr };
		hr = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**)&factory );
		if ( FAILED( hr ) )
		{
			return false;
		}

		IDXGIAdapter* adapter{ nullptr };
		hr = factory->EnumAdapters( 0, &adapter );
		if ( FAILED( hr ) )
		{
			return false;
		}
		
		IDXGIOutput* output{ nullptr };
		hr = adapter->EnumOutputs( 0, &output );
		if ( FAILED( hr ) )
		{
			return false;
		}

		unsigned int modes{ 0 };
		hr = output->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		DXGI_MODE_DESC* mode_list = new DXGI_MODE_DESC[ modes ];
		if ( !mode_list )
		{
			return false;
		}

		hr = output->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes, mode_list );
		if ( FAILED( hr ) )
		{
			return false;
		}

		unsigned int numerator{ 0 };
		unsigned int denominator{ 0 };
		for ( unsigned int i = 0; i < modes; ++i )
		{
			if ( mode_list[ i ].Width == (unsigned int)_width && mode_list[ i ].Height == (unsigned int)_height )
			{
				numerator = mode_list[ i ].RefreshRate.Numerator;
				denominator = mode_list[ i ].RefreshRate.Denominator;
			}
		}

		
		DXGI_ADAPTER_DESC adapter_desc{};
		hr = adapter->GetDesc( &adapter_desc );
		if ( FAILED( hr ) )
		{
			return false;
		}

		unsigned long long length{ 0 };
		m_vga_memory = (int)( adapter_desc.DedicatedVideoMemory / 1024 / 1024 );
		int error = wcstombs_s( &length, m_vga_name, MAX_PATH, adapter_desc.Description, 128 );
		if ( 0 != error )
		{
			return false;
		}

		delete[] mode_list;
		mode_list = nullptr;

		output->Release();
		output = nullptr;

		adapter->Release();
		adapter = nullptr;

		factory->Release();
		factory = nullptr;

		DXGI_SWAP_CHAIN_DESC swapchain_desc{ };
		ZeroMemory( &swapchain_desc, sizeof( swapchain_desc ) );
		swapchain_desc.BufferCount = 1;
		swapchain_desc.BufferDesc.Width = _width;
		swapchain_desc.BufferDesc.Height = _height;
		swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchain_desc.BufferDesc.RefreshRate.Numerator = m_vsync ? numerator : 0;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = m_vsync ? denominator : 1;
		swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc.OutputWindow = _hwnd;
		swapchain_desc.SampleDesc.Count = 1;
		swapchain_desc.SampleDesc.Quality = 0;
		swapchain_desc.Windowed = _fullscreen ? false : true;
		swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapchain_desc.Flags = 0;

		D3D_FEATURE_LEVEL feature_level{ D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 };

		UINT create_flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		hr = D3D11CreateDeviceAndSwapChain( 
			nullptr, 
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr, 
			create_flags, 
			&feature_level, 
			1, 
			D3D11_SDK_VERSION, 
			&swapchain_desc,
			&m_swapchain, 
			&m_device,
			nullptr,
			&m_device_context 
		);
		if ( FAILED( hr ) )
		{
			return false;
		}

		ID3D11Texture2D* back_buffer{ nullptr };
		hr = m_swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&back_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_device->CreateRenderTargetView( back_buffer, nullptr, &m_render_target_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		back_buffer->Release();
		back_buffer = nullptr;

		D3D11_TEXTURE2D_DESC depth_buffer_desc{};
		ZeroMemory( &depth_buffer_desc, sizeof( depth_buffer_desc ) );
		depth_buffer_desc.Width = _width;
		depth_buffer_desc.Height = _height;
		depth_buffer_desc.MipLevels = 1;
		depth_buffer_desc.ArraySize = 1;
		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.SampleDesc.Count = 1;
		depth_buffer_desc.SampleDesc.Quality = 0;
		depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_buffer_desc.CPUAccessFlags = 0;
		depth_buffer_desc.MiscFlags = 0;

		hr = m_device->CreateTexture2D( &depth_buffer_desc, nullptr, &m_depth_stencil_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		ZeroMemory( &depth_stencil_desc, sizeof( depth_stencil_desc ) );
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = true;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = m_device->CreateDepthStencilState( &depth_stencil_desc, &m_depth_stencil_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_DESC depth_disabled_stencil_desc{};
		ZeroMemory( &depth_disabled_stencil_desc, sizeof( depth_disabled_stencil_desc ) );
		depth_disabled_stencil_desc.DepthEnable = false;
		depth_disabled_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_disabled_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_disabled_stencil_desc.StencilEnable = true;
		depth_disabled_stencil_desc.StencilReadMask = 0xFF;
		depth_disabled_stencil_desc.StencilWriteMask = 0xFF;
		depth_disabled_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_disabled_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_disabled_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_disabled_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_disabled_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_disabled_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_disabled_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_disabled_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = m_device->CreateDepthStencilState( &depth_disabled_stencil_desc, &m_depth_disabled_stencil_state );
		if ( FAILED( hr ) )
		{
			return false;
		}


		m_device_context->OMSetDepthStencilState( m_depth_stencil_state, 1 );

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		ZeroMemory( &depth_stencil_view_desc, sizeof( depth_stencil_view_desc ) );
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;

		hr = m_device->CreateDepthStencilView( m_depth_stencil_buffer, &depth_stencil_view_desc, &m_depth_stencil_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_device_context->OMSetRenderTargets( 1, &m_render_target_view, m_depth_stencil_view );

		D3D11_RASTERIZER_DESC rasterizer_desc{};
		ZeroMemory( &rasterizer_desc, sizeof( rasterizer_desc ) );
		rasterizer_desc.AntialiasedLineEnable = false;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.0f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.FrontCounterClockwise = false;
		rasterizer_desc.MultisampleEnable = false;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;

		hr = m_device->CreateRasterizerState( &rasterizer_desc, &m_raster_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_device_context->RSSetState( m_raster_state );

		D3D11_BLEND_DESC blend_desc{};
		ZeroMemory( &blend_desc, sizeof( blend_desc ) );
		blend_desc.RenderTarget[ 0 ].BlendEnable = TRUE;
		blend_desc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[ 0 ].RenderTargetWriteMask = 0x0f;

		hr = m_device->CreateBlendState( &blend_desc, &m_alpha_enable_blend_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		blend_desc.RenderTarget[ 0 ].BlendEnable = FALSE;

		hr = m_device->CreateBlendState( &blend_desc, &m_alpha_disable_blend_state );
		if ( FAILED( hr ) )
		{
			return false;
		}


		D3D11_VIEWPORT viewport{};
		ZeroMemory( &viewport, sizeof( viewport ) );
		viewport.Width = (float)_width;
		viewport.Height = (float)_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		m_device_context->RSSetViewports( 1, &viewport );

		float fov = XM_PIDIV4;
		float aspect{ (float)_width / (float)_height };

		m_world_matrix = XMMatrixIdentity();
		m_projection_matrix = XMMatrixPerspectiveFovLH( fov, aspect, _near, _far );
		m_ortho_matrix = XMMatrixOrthographicLH( (float)_width, (float)_height, _near, _far );

		return true;
	}

	void D3D::Shutdown()
	{
		if ( m_swapchain )
		{
			m_swapchain->SetFullscreenState( false, nullptr );
		}

		SAFE_RELEASE( m_alpha_disable_blend_state );
		SAFE_RELEASE( m_alpha_enable_blend_state );
		SAFE_RELEASE( m_raster_state );
		SAFE_RELEASE( m_depth_stencil_view );
		SAFE_RELEASE( m_depth_disabled_stencil_state );
		SAFE_RELEASE( m_depth_stencil_state );
		SAFE_RELEASE( m_depth_stencil_buffer );
		SAFE_RELEASE( m_render_target_view );
		SAFE_RELEASE( m_device_context );
		SAFE_RELEASE( m_device );
		SAFE_RELEASE( m_swapchain );
	}

	void D3D::BeginScene( float _r, float _g, float _b, float _a )
	{
		const float color[ 4 ]{ _r, _g, _b, _a };

		m_device_context->ClearRenderTargetView( m_render_target_view, color );
		m_device_context->ClearDepthStencilView( m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}

	void D3D::EndScene()
	{
		m_vsync ? m_swapchain->Present( 1, 0 ) : m_swapchain->Present( 0, 0 );
	}

	ID3D11Device* D3D::GetDevice()
	{
		return m_device;
	}

	ID3D11DeviceContext* D3D::GetDeviceContext()
	{
		return m_device_context;
	}

	void D3D::GetProjectionMatrix( XMMATRIX& _m )
	{
		_m = m_projection_matrix;
	}

	void D3D::GetWorldMatrix( XMMATRIX& _m )
	{
		_m = m_world_matrix;
	}

	void D3D::GetOrthoMatrix( XMMATRIX& _m )
	{
		_m = m_ortho_matrix;
	}

	void D3D::GetVideoCardInfo( char* _buffer, int& _memory )
	{
		strcpy_s( _buffer, 128, m_vga_name );
		_memory = m_vga_memory;
	}

	void D3D::TurnOnZBuffer()
	{
		m_device_context->OMSetDepthStencilState( m_depth_stencil_state, 1 );
	}

	void D3D::TurnOffZBuffer()
	{
		m_device_context->OMSetDepthStencilState( m_depth_disabled_stencil_state, 1 );
	}

	void D3D::TurnOnAlphaBlending()
	{
		float blend_factor[ 4 ]{ 0.0f, 0.0f, 0.0f, 0.0f };

		m_device_context->OMSetBlendState( m_alpha_enable_blend_state, blend_factor, 0xffffffff );
	}

	void D3D::TurnOffAlphaBlending()
	{
		float blend_factor[ 4 ]{ 0.0f, 0.0f, 0.0f, 0.0f };

		m_device_context->OMSetBlendState( m_alpha_disable_blend_state, blend_factor, 0xffffffff );
	}

	int D3D::GetWidth() const
	{
		return m_width;
	}

	int D3D::GetHeight() const
	{
		return m_height;
	}

}