#include "pch.h"
#include "Bitmap.h"

namespace wf
{
	Bitmap::Bitmap()
	{
	}

	Bitmap::Bitmap( const Bitmap& )
	{
	}

	Bitmap::~Bitmap()
	{
	}

	bool Bitmap::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, int _screen_width, int _screen_height, const char* _path, int _bitmap_width, int _bitmap_height )
	{
		m_screen_width = _screen_width;
		m_screen_height = _screen_height;
		m_bitmap_width = _bitmap_width;
		m_bitmap_height = _bitmap_height;

		if ( !InitializeBuffers( _device ) )
		{
			return false;
		}

		if ( !LoadTexture( _device, _context, _path ) )
		{
			return false;
		}

		return true;
	}

	void Bitmap::Shutdown()
	{
		ReleaseTexture();

		ShutdownBuffers();
	}

	bool Bitmap::Render( ID3D11DeviceContext* _context, int _x, int _y )
	{
		if ( !UpdateBuffers( _context, _x, _y ) )
		{
			return false;
		}

		RenderBuffers( _context );

		return true;
	}

	int Bitmap::GetIndexCount()
	{
		return m_index_count;
	}

	ID3D11ShaderResourceView* Bitmap::GetTexture() const
	{
		return m_texture->GetTexture();
	}

	bool Bitmap::InitializeBuffers( ID3D11Device* _device )
	{
		m_vertex_count = 6;
		m_index_count = 6;

		VertexType* vertices = new VertexType[ m_vertex_count ];
		UINT* indices = new UINT[ m_index_count ];
		if ( !vertices || !indices )
		{
			return false;
		}

		memset( vertices, 0, sizeof( VertexType ) * m_vertex_count );

		for ( int i = 0; i < 6; ++i )
		{
			indices[ i ] = i;
		}

		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( VertexType ) * m_vertex_count;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data{};
		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = _device->CreateBuffer( &buffer_desc, &data, &m_vertex_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( vertices );
			SAFE_DELETE_ARRAY( indices );
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof( UINT ) * m_index_count;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = _device->CreateBuffer( &buffer_desc, &data, &m_index_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( vertices );
			SAFE_DELETE_ARRAY( indices );
			return false;
		}

		SAFE_DELETE_ARRAY( vertices );
		SAFE_DELETE_ARRAY( indices );

		return true;
	}

	void Bitmap::ShutdownBuffers()
	{
		SAFE_RELEASE( m_index_buffer );
		SAFE_RELEASE( m_vertex_buffer );
	}

	bool Bitmap::UpdateBuffers( ID3D11DeviceContext* _context, int _x, int _y )
	{
		if ( m_previous_x == _x && m_previous_y == _y )
		{
			return true;
		}

		m_previous_x = _x;
		m_previous_y = _y;

		float left = static_cast<float>( ( -m_screen_width / 2.0f ) + _x );
		float right = left + static_cast<float>( m_bitmap_width );
		float top = static_cast<float>( ( m_screen_height / 2.0f ) - _y );
		float bottom = top - static_cast<float>( m_bitmap_height );

		m_vertices[ 0 ].pos = XMFLOAT3( left, top, 0.0f );
		m_vertices[ 0 ].tex = XMFLOAT2( 0.0f, 0.0f );

		m_vertices[ 1 ].pos = XMFLOAT3( right, bottom, 0.0f );
		m_vertices[ 1 ].tex = XMFLOAT2( 1.0f, 1.0f );

		m_vertices[ 2 ].pos = XMFLOAT3( left, bottom, 0.0f );
		m_vertices[ 2 ].tex = XMFLOAT2( 0.0f, 1.0f );

		m_vertices[ 3 ].pos = XMFLOAT3( left, top, 0.0f );
		m_vertices[ 3 ].tex = XMFLOAT2( 0.0f, 0.0f );

		m_vertices[ 4 ].pos = XMFLOAT3( right, top, 0.0f );
		m_vertices[ 4 ].tex = XMFLOAT2( 1.0f, 0.0f );

		m_vertices[ 5 ].pos = XMFLOAT3( right, bottom, 0.0f );
		m_vertices[ 5 ].tex = XMFLOAT2( 1.0f, 1.0f );

		
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = _context->Map( m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if ( FAILED( hr ) )
		{
			return false;
		}

		VertexType* vertices = (VertexType*)mappedResource.pData;

		memcpy_s( vertices, sizeof( VertexType ) * m_vertex_count, m_vertices.data(), sizeof( VertexType ) * m_vertex_count );

		_context->Unmap( m_vertex_buffer, 0 );

		return true;
	}

	void Bitmap::RenderBuffers( ID3D11DeviceContext* _context )
	{
		UINT stride{ sizeof( VertexType ) };
		UINT offset{ 0 };

		_context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

		_context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

		_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	bool Bitmap::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path )
	{
		m_texture = new Texture;
		if ( !m_texture->Initialize( _device, _context, _path ) )
		{
			return false;
		}

		return true;
	}

	void Bitmap::ReleaseTexture()
	{
		SAFE_SHUTDOWN( m_texture );
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	RenderTargetBitmap::RenderTargetBitmap()
	{
	}

	RenderTargetBitmap::RenderTargetBitmap( const RenderTargetBitmap& )
	{
	}

	RenderTargetBitmap::~RenderTargetBitmap()
	{
	}

	bool RenderTargetBitmap::Initialize( ID3D11Device* _device, int _width, int _height )
	{
		return InitializeBuffers( _device, _width, _height );
	}

	void RenderTargetBitmap::Shutdown()
	{
		ShutdownBuffers();
	}

	void RenderTargetBitmap::Render( ID3D11DeviceContext* _context )
	{
		RenderBuffers( _context );
	}

	int RenderTargetBitmap::GetIndexCount()
	{
		return m_index_count;
	}

	bool RenderTargetBitmap::InitializeBuffers( ID3D11Device* _device, int _width, int _height )
	{
		float l = (float)( _width / 2.0f ) * -1.0f;
		float t = (float)( _height / 2.0f );
		float r = l + (float)_width;
		float b = t - (float)_height;

		m_vertex_count = 6;
		m_index_count = 6;

		VertexType* vertices = new VertexType[ m_vertex_count ];
		UINT* indices = new UINT[ m_index_count ];
		if ( !vertices || !indices )
		{
			SAFE_DELETE_ARRAY( indices );
			SAFE_DELETE_ARRAY( vertices );
			return false;
		}

		vertices[ 0 ].pos = XMFLOAT3( l, t, 0.0f );
		vertices[ 0 ].tex = XMFLOAT2( 0.0f, 0.0f );

		vertices[ 1 ].pos = XMFLOAT3( r, b, 0.0f );
		vertices[ 1 ].tex = XMFLOAT2( 1.0f, 1.0f );

		vertices[ 2 ].pos = XMFLOAT3( l, b, 0.0f );
		vertices[ 2 ].tex = XMFLOAT2( 0.0f, 1.0f );

		vertices[ 3 ].pos = XMFLOAT3( l, t, 0.0f );
		vertices[ 3 ].tex = XMFLOAT2( 0.0f, 0.0f );

		vertices[ 4 ].pos = XMFLOAT3( r, t, 0.0f );
		vertices[ 4 ].tex = XMFLOAT2( 1.0f, 0.0f );

		vertices[ 5 ].pos = XMFLOAT3( r, b, 0.0f );
		vertices[ 5 ].tex = XMFLOAT2( 1.0f, 1.0f );

		for ( int i = 0; i < m_index_count; ++i )
		{
			indices[ i ] = i;
		}

		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof( VertexType ) * m_vertex_count;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data{};
		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = _device->CreateBuffer( &buffer_desc, &data, &m_vertex_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( indices );
			SAFE_DELETE_ARRAY( vertices );
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof( UINT ) * m_index_count;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = _device->CreateBuffer( &buffer_desc, &data, &m_index_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( indices );
			SAFE_DELETE_ARRAY( vertices );
			return false;
		}

		SAFE_DELETE_ARRAY( indices );
		SAFE_DELETE_ARRAY( vertices );

		return true;
	}

	void RenderTargetBitmap::ShutdownBuffers()
	{
		SAFE_RELEASE( m_index_buffer );
		SAFE_RELEASE( m_vertex_buffer );
	}

	void RenderTargetBitmap::RenderBuffers( ID3D11DeviceContext* _context )
	{
		UINT stride{ sizeof( VertexType ) };
		UINT offset{ 0 };

		_context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

		_context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

		_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}


}
