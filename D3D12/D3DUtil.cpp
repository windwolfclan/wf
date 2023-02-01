#include "pch.h"
#include "D3DUtil.h"
#include <comdef.h>

NAMESPACE_WF_S

DxException::DxException(HRESULT _hr, const std::wstring & _func, const std::wstring & _file, int _line)
	: hr( _hr ), func( _func ), file( _file ), line( _line )
{

}

std::wstring DxException::ToString() const
{
	_com_error err( hr );
	std::wstring msg = err.ErrorMessage();

	return func + L" failed in " + file + L"\nline : " + std::to_wstring( line ) + L"\n Error : " + msg;
}

MeshGeometry::~MeshGeometry()
{
	draw_arguments.clear();

	SAFE_RELEASE( index_buffer_uploader );
	SAFE_RELEASE( vertex_buffer_uploader );
	SAFE_RELEASE( gpu_index_buffer );
	SAFE_RELEASE( gpu_vertex_buffer );
	SAFE_RELEASE( cpu_index_buffer );
	SAFE_RELEASE( cpu_vertex_buffer );
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	ZeroMemory( &vbv, sizeof( vbv ) );
	vbv.BufferLocation = gpu_vertex_buffer->GetGPUVirtualAddress();
	vbv.StrideInBytes = vertex_byte_stride;
	vbv.SizeInBytes = vertex_buffer_byte_size;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW	MeshGeometry::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ZeroMemory( &ibv, sizeof( ibv ) );

	ibv.BufferLocation = gpu_index_buffer->GetGPUVirtualAddress();
	ibv.Format = index_format;
	ibv.SizeInBytes = index_buffer_byte_size;

	return ibv;
}

void MeshGeometry::DisposeUploaders()
{
	vertex_buffer_uploader = nullptr;
	index_buffer_uploader = nullptr;
}

UINT utility::CalculateConstantBufferByteSize( UINT _byte_size )
{
	return ( _byte_size + 255 ) & ~255;
}

ID3DBlob* utility::CompileShader( const std::wstring& _filename, const D3D_SHADER_MACRO* _defines, const std::string& _entry, const std::string& _target )
{
	UINT compile_flags{ 0 };
#if defined(DEBUG) || defined(_DEBUG)  
	compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* byte_code{ nullptr };
	ID3DBlob* errors{ nullptr };
	HRESULT hr = D3DCompileFromFile(
		_filename.c_str(),
		_defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_entry.c_str(),
		_target.c_str(),
		compile_flags,
		0,
		&byte_code,
		&errors
	);

	if ( errors )
	{
		OutputDebugStringA( (char*)errors->GetBufferPointer() );
		return nullptr;
	}

	return byte_code;
}

ID3D12Resource* utility::CreateDefaultBuffer( ID3D12Device* _device, ID3D12GraphicsCommandList* _command_list, const void* _init_data, UINT64 _byte_size, ID3D12Resource*& _upload_buffer )
{
	ID3D12Resource* default_buffer{ nullptr };
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer( _byte_size );

	CD3DX12_HEAP_PROPERTIES default_properties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
	HRESULT hr = _device->CreateCommittedResource(
		&default_properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &default_buffer )
	);

	if ( FAILED( hr ) )
	{
		return nullptr;
	}

	CD3DX12_HEAP_PROPERTIES upload_properties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
	hr = _device->CreateCommittedResource(
		&upload_properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &_upload_buffer )
	);

	if ( FAILED( hr ) )
	{
		return nullptr;
	}

	D3D12_SUBRESOURCE_DATA data;
	ZeroMemory( &data, sizeof( data ) );
	data.pData = _init_data;
	data.RowPitch = _byte_size;
	data.SlicePitch = data.RowPitch;

	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition( default_buffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST );
	CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition( default_buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ );

	_command_list->ResourceBarrier( 1, &barrier1 );
	UpdateSubresources( _command_list, default_buffer, _upload_buffer, 0, 0, 1, &data );
	_command_list->ResourceBarrier( 1, &barrier2 );

	return default_buffer;
}


NAMESPACE_WF_E
