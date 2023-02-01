#pragma once

#include "d3dx12.h"

NAMESPACE_WF_S

__forceinline void D3DSetDebugName( IDXGIObject* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}
__forceinline void D3DSetDebugName( ID3D12Device* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}
__forceinline void D3DSetDebugName( ID3D12DeviceChild* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}

__forceinline std::wstring AnsiToWString( const std::string& str )
{
    WCHAR buffer[ 512 ];
    MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, buffer, 512 );
    return std::wstring( buffer );
}

class DxException
{
public:
    DxException() = default;
    DxException( HRESULT _hr, const std::wstring& _func, const std::wstring& _file, int _line );

    std::wstring ToString() const;

    HRESULT hr{ S_OK };
    std::wstring func;
    std::wstring file;
    int line{ -1 };
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#define CHECK_HR( x )                                                           \
{                                                                               \
    HRESULT hr__ = ( x );                                                       \
    if ( FAILED( hr__ ) )                                                       \
    {                                                                           \
        throw DxException( hr__, L#x, AnsiToWString( __FILE__ ), __LINE__ );    \
	}                                                                           \
}

struct SubmeshGeometry
{
    UINT index_count = 0;
    UINT start_index_location = 0;
    INT base_vertex_location = 0;

    DirectX::BoundingBox bound_box;
};

struct MeshGeometry
{
    ~MeshGeometry();

    std::string name;

    ID3DBlob* cpu_vertex_buffer{ nullptr };
    ID3DBlob* cpu_index_buffer{ nullptr };

    ID3D12Resource* gpu_vertex_buffer{ nullptr };
    ID3D12Resource* gpu_index_buffer{ nullptr };

    ID3D12Resource* vertex_buffer_uploader{ nullptr };
    ID3D12Resource* index_buffer_uploader{ nullptr };

    UINT vertex_byte_stride{ 0 };
    UINT vertex_buffer_byte_size{ 0 };
    UINT index_buffer_byte_size{ 0 };
    DXGI_FORMAT index_format{ DXGI_FORMAT_R16_UINT };

    std::unordered_map<std::string, SubmeshGeometry> draw_arguments;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;
    void DisposeUploaders();
};

namespace utility
{
    UINT CalculateConstantBufferByteSize( UINT _byte_size );
    ID3DBlob* CompileShader( const std::wstring& _filename, const D3D_SHADER_MACRO* _defines, const std::string& _entry, const std::string& _target );
    ID3D12Resource* CreateDefaultBuffer( ID3D12Device* _device, ID3D12GraphicsCommandList* _command_list, const void* _init_data, UINT64 _byte_size, ID3D12Resource*& _upload_buffer );
}

NAMESPACE_WF_E
