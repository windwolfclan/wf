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

NAMESPACE_WF_E
