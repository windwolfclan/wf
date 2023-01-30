#include "pch.h"
#include "D3D12Context.h"

NAMESPACE_WF_S

D3D12Context::D3D12Context( HWND _hwnd, int _client_width, int _client_height )
	: hwnd( _hwnd ), client_width( _client_width ), client_height( _client_height )
{
}

bool D3D12Context::InitializeContext()
{
#ifdef _DEBUG
	CHECK_HR( D3D12GetDebugInterface( IID_PPV_ARGS( &debug_controller ) ) );
	debug_controller->EnableDebugLayer();
#endif

	CHECK_HR( CreateDXGIFactory1( IID_PPV_ARGS( &factory ) ) );

	CHECK_HR( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &device ) ) );

	CHECK_HR( device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &fence ) ) );

	rtv_descriptor_size = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	dsv_descriptor_size = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
	cbv_srv_uav_descriptor_size = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multisample_quality_levels;
	ZeroMemory( &multisample_quality_levels, sizeof( multisample_quality_levels ) );
	multisample_quality_levels.Format = back_buffer_format;
	multisample_quality_levels.SampleCount = 4;
	multisample_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	multisample_quality_levels.NumQualityLevels = 0;

	ThrowIfFailed( device->CheckFeatureSupport( 
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, 
		&multisample_quality_levels, 
		sizeof( multisample_quality_levels ) 
	) );
	
	msaa_quality = multisample_quality_levels.NumQualityLevels;

	if ( !CreateCommandObjects() )
	{
		return false;
	}

	if ( !CreateSwapChain() )
	{
		return false;
	}

	if ( !CreateRtvAndDsvDescriptorHeaps() )
	{
		return false;
	}

	return true;
}

void D3D12Context::TerminateContext()
{
	SAFE_RELEASE( depth_stencil_buffer );
	for ( size_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i )
		SAFE_RELEASE( swap_chain_buffer[ i ] );

	SAFE_RELEASE( dsv_heap );
	SAFE_RELEASE( rtv_heap );
	SAFE_RELEASE( swap_chain );
	SAFE_RELEASE( command_list );
	SAFE_RELEASE( command_allocator );
	SAFE_RELEASE( command_queue );
	SAFE_RELEASE( fence );
	SAFE_RELEASE( device );
	SAFE_RELEASE( factory );

#ifdef _DEBUG
	SAFE_RELEASE( debug_controller );
#endif 
}

ID3D12Device* D3D12Context::GetDevice() const
{
	return device;
}

ID3D12Resource* D3D12Context::GetBackBuffer() const
{
	return swap_chain_buffer[ current_back_buffer ];
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::GetBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtv_heap->GetCPUDescriptorHandleForHeapStart(),
		(UINT)current_back_buffer,
		rtv_descriptor_size
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Context::GetDepthStencilView() const
{
	return dsv_heap->GetCPUDescriptorHandleForHeapStart();
}

bool D3D12Context::Is4xMsaaState() const
{
	return msaa_state;
}

void D3D12Context::Toggle4xMsaaState()
{
	msaa_state = !msaa_state;
	CreateSwapChain();
	OnResize( client_width, client_height );
}

void D3D12Context::OnResize( int _client_width, int _client_height )
{
	client_width = _client_width;
	client_height = _client_height;

	assert( device );
	assert( swap_chain );
	assert( command_allocator );

	FlushCommandQueue();

	CHECK_HR( command_list->Reset( command_allocator, nullptr ) );

	for ( size_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i )
	{
		SAFE_RELEASE( swap_chain_buffer[ i ] );
	}
	SAFE_RELEASE( depth_stencil_buffer );

	// Resize the swap chain.
	CHECK_HR( swap_chain->ResizeBuffers(
		SWAP_CHAIN_BUFFER_COUNT,
		client_width, client_height,
		back_buffer_format,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ) );

	current_back_buffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle( rtv_heap->GetCPUDescriptorHandleForHeapStart() );
	for ( size_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i )
	{
		CHECK_HR( swap_chain->GetBuffer( (UINT)i, IID_PPV_ARGS( &swap_chain_buffer[ i ] ) ) );
		device->CreateRenderTargetView( swap_chain_buffer[ i ], nullptr, rtv_heap_handle );
		rtv_heap_handle.Offset( 1, rtv_descriptor_size );
	}

	D3D12_RESOURCE_DESC depth_stencil_desc;
	ZeroMemory( &depth_stencil_desc, sizeof( depth_stencil_desc ) );
	depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Alignment = 0;
	depth_stencil_desc.Width = client_width;
	depth_stencil_desc.Height = client_height;
	depth_stencil_desc.DepthOrArraySize = 1;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.Format = depth_stencil_format;
	depth_stencil_desc.SampleDesc.Count = msaa_state ? 4 : 1;
	depth_stencil_desc.SampleDesc.Quality = msaa_state ? ( msaa_quality - 1 ) : 0;
	depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE opt_clear;
	ZeroMemory( &opt_clear, sizeof( opt_clear ) );
	opt_clear.Format = depth_stencil_format;
	opt_clear.DepthStencil.Depth = 1.0f;
	opt_clear.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES properties{ D3D12_HEAP_TYPE_DEFAULT };

	CHECK_HR( device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&depth_stencil_desc,
		D3D12_RESOURCE_STATE_COMMON,
		&opt_clear,
		IID_PPV_ARGS( &depth_stencil_buffer )
	) );

	device->CreateDepthStencilView( depth_stencil_buffer, nullptr, GetDepthStencilView() );

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		depth_stencil_buffer,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	command_list->ResourceBarrier( 1, &barrier );

	CHECK_HR( command_list->Close() );

	ID3D12CommandList* lists[]{ command_list };
	command_queue->ExecuteCommandLists( _countof( lists ), lists );

	FlushCommandQueue();

	screen_viewport.TopLeftX = 0.0f;
	screen_viewport.TopLeftY = 0.0f;
	screen_viewport.Width = (FLOAT)client_width;
	screen_viewport.Height = (FLOAT)client_height;
	screen_viewport.MinDepth = 0.0f;
	screen_viewport.MaxDepth = 1.0f;

	scissor_rect = { 0, 0, client_width, client_height };
}

void D3D12Context::SetScreenViewport()
{
	command_list->RSSetViewports( 1, &screen_viewport );
}

void D3D12Context::SetScissorRect()
{
	command_list->RSSetScissorRects( 1, &scissor_rect );
}

void D3D12Context::ClearRenderTargetView( float _r, float _g, float _b, float _a )
{
	const float color[ 4 ]{ _r, _g, _b, _a };
	command_list->ClearRenderTargetView( GetBackBufferView(), color, 0, nullptr );
}

void D3D12Context::ClearDepthStencilView()
{
	command_list->ClearDepthStencilView( GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );
}

void D3D12Context::SetBackBufferRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = GetDepthStencilView();

	command_list->OMSetRenderTargets( 1, &rtv, true, &dsv );
}

void D3D12Context::Present()
{
	CHECK_HR( swap_chain->Present( 0, 0 ) );
	current_back_buffer = ( current_back_buffer + 1 ) % SWAP_CHAIN_BUFFER_COUNT;
}

void D3D12Context::FlushCommandQueue()
{
	++current_fence;

	CHECK_HR( command_queue->Signal( fence, current_fence ) );

	if ( fence->GetCompletedValue() < current_fence )
	{
		HANDLE handle = CreateEvent( nullptr, false, false, nullptr );
		CHECK_HR( fence->SetEventOnCompletion( current_fence, handle ) );

		WaitForSingleObject( handle, INFINITE );
		CloseHandle( handle );
	}
}

bool D3D12Context::CreateCommandObjects()
{
	D3D12_COMMAND_LIST_TYPE type{ D3D12_COMMAND_LIST_TYPE_DIRECT };

	D3D12_COMMAND_QUEUE_DESC queue_desc;
	ZeroMemory( &queue_desc, sizeof( queue_desc ) );
	queue_desc.Type = type;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	CHECK_HR( device->CreateCommandQueue( &queue_desc, IID_PPV_ARGS( &command_queue ) ) );

	CHECK_HR( device->CreateCommandAllocator( type, IID_PPV_ARGS( &command_allocator ) ) );

	CHECK_HR( device->CreateCommandList( 0, type, command_allocator, nullptr, IID_PPV_ARGS( &command_list ) ) );

	command_list->Close();

	return true;
}

bool D3D12Context::CreateSwapChain()
{
	SAFE_RELEASE( swap_chain );

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.BufferDesc.Width = (UINT)client_width;
	desc.BufferDesc.Height = (UINT)client_height;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = back_buffer_format;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = msaa_state ? 4 : 1;
	desc.SampleDesc.Quality = msaa_state ? ( msaa_quality - 1 ) : 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
 	desc.OutputWindow = hwnd;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	CHECK_HR( factory->CreateSwapChain( command_queue, &desc, &swap_chain ) );

	return true;
}

bool D3D12Context::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
	ZeroMemory( &rtv_heap_desc, sizeof( rtv_heap_desc ) );
	rtv_heap_desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0;
	CHECK_HR( device->CreateDescriptorHeap( &rtv_heap_desc, IID_PPV_ARGS( &rtv_heap ) ) );

	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
	ZeroMemory( &dsv_heap_desc, sizeof( dsv_heap_desc ) );
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;
	CHECK_HR( device->CreateDescriptorHeap( &dsv_heap_desc, IID_PPV_ARGS( &dsv_heap ) ) );

	return true;
}


NAMESPACE_WF_E

