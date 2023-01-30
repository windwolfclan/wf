#pragma once

NAMESPACE_WF_S

constexpr size_t SWAP_CHAIN_BUFFER_COUNT = 2;

struct D3D12Context
{
public:
    D3D12Context( HWND _hwnd, int _client_width, int _client_height );

    // function
public:
    bool InitializeContext();
    void TerminateContext();

    // D3D12 Getter
    ID3D12Device* GetDevice() const;
    ID3D12Resource* GetBackBuffer()const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView()const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()const;

    // Environment
    bool Is4xMsaaState() const;
    void Toggle4xMsaaState();

    // Handler
    void OnResize( int _client_width, int _client_height );

    // Render helper
    void SetScreenViewport();
    void SetScissorRect();
    void ClearRenderTargetView( float _r, float _g, float _b, float _a );
    void ClearDepthStencilView();
    void SetBackBufferRenderTarget();
    void Present();
    void FlushCommandQueue();

protected:
    bool CreateCommandObjects();
    bool CreateSwapChain();
    bool CreateRtvAndDsvDescriptorHeaps();

    // variable
protected:
    IDXGIFactory4* factory{ nullptr };
    IDXGISwapChain* swap_chain{ nullptr };
    ID3D12Device* device{ nullptr };

    UINT current_fence{ 0 };
    ID3D12Fence* fence{ nullptr };

public:
    ID3D12CommandQueue* command_queue{ nullptr };
    ID3D12CommandAllocator* command_allocator{ nullptr };
    ID3D12GraphicsCommandList* command_list{ nullptr };

protected:
    size_t current_back_buffer{ 0 };
    ID3D12Resource* swap_chain_buffer[ SWAP_CHAIN_BUFFER_COUNT ]{ nullptr, nullptr };
    ID3D12Resource* depth_stencil_buffer{ nullptr };

    ID3D12DescriptorHeap* rtv_heap{ nullptr };
    ID3D12DescriptorHeap* dsv_heap{ nullptr };

    D3D12_VIEWPORT screen_viewport{};
    D3D12_RECT scissor_rect{};

    UINT rtv_heap_size{ 0 };
    UINT dsv_heap_size{ 0 };

    UINT rtv_descriptor_size{ 0 };
    UINT dsv_descriptor_size{ 0 };
    UINT cbv_srv_uav_descriptor_size{ 0 };

    bool msaa_state{ false };
    UINT msaa_quality{ 0 };

    D3D_DRIVER_TYPE driver_type{ D3D_DRIVER_TYPE_HARDWARE };
    DXGI_FORMAT back_buffer_format{ DXGI_FORMAT_R8G8B8A8_UNORM };
    DXGI_FORMAT depth_stencil_format{ DXGI_FORMAT_D24_UNORM_S8_UINT };

    HWND hwnd{ nullptr };
    int client_width{ 0 };
    int client_height{ 0 };

#ifdef _DEBUG
    ID3D12Debug* debug_controller{ nullptr };
#endif
};


NAMESPACE_WF_E