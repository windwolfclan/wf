#include "pch.h"
#include "Quad.h"

namespace wf
{
	int Quad::s_screen_width = 0;
	int Quad::s_screen_height = 0;

	void Quad::SetScreenSize( int _width, int _height )
	{
		s_screen_width = _width;
		s_screen_height = _height;
	}

	Quad* Quad::CreateQuad( quad_type _quad_type )
	{
		Quad* quad{ nullptr };

		switch ( _quad_type )
		{
			case quad_type::texture:
			{
				quad = new TextureQuad;
				break;
			}

			case quad_type::tangent_space:
			{
				quad = new TangentSpaceQuad;
				break;
			}
		}

		return quad;
	}

	Quad::Quad()
	{

	}

	Quad::Quad( const Quad& )
	{

	}
	
	Quad::~Quad()
	{

	}

	bool Quad::Initialize( ID3D11Device* _device, int _quad_width, int _quad_height )
	{
		m_quad_width = _quad_width;
		m_quad_height = _quad_height;

		if ( !InitializeBuffers( _device ) )
		{
			return false;
		}

		return true;
	}
	void Quad::Shutdown()
	{
		ShutdownBuffers();
	}
	bool Quad::Render( ID3D11DeviceContext* _context, int _x, int _y )
	{
		if ( !UpdateBuffers( _context, _x, _y ) )
		{
			return false;
		}

		RenderBuffers( _context );

		return true;
	}

	int Quad::GetIndexCount() const
	{
		return m_index_count;
	}
	
	void Quad::ShutdownBuffers()
	{
		SAFE_RELEASE( m_index_buffer );
		SAFE_RELEASE( m_vertex_buffer );
	}

	bool Quad::UpdateBuffers( ID3D11DeviceContext* _context, int _x, int _y )
	{
		if ( m_previous_x == _x && m_previous_y == _y )
		{
			return true;
		}

		m_previous_x = _x;
		m_previous_y = _y;

		UpdatePosition( _x, _y );

		if ( !UpdateGpuData( _context ) )
		{
			return false;
		}

		return true;
	}

	void Quad::RenderBuffers( ID3D11DeviceContext* _context )
	{
		UINT stride{ m_stride };
		UINT offset{ 0 };

		_context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

		_context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

		_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}


	bool TextureQuad::InitializeBuffers( ID3D11Device* _device )
	{
		m_stride = sizeof( VertexType );
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

	void TextureQuad::UpdatePosition( int _x, int _y ) 
	{
		float left = static_cast<float>( ( -s_screen_width / 2.0f ) + _x );
		float right = left + static_cast<float>( m_quad_width );
		float top = static_cast<float>( ( s_screen_height / 2.0f ) - _y );
		float bottom = top - static_cast<float>( m_quad_height );

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
	}

	bool TextureQuad::UpdateGpuData( ID3D11DeviceContext* _context )
	{
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


	bool TangentSpaceQuad::InitializeBuffers( ID3D11Device* _device )
	{
		m_stride = sizeof( VertexType );
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

	void TangentSpaceQuad::UpdatePosition( int _x, int _y )
	{
		float left = static_cast<float>( ( -s_screen_width / 2.0f ) + _x );
		float right = left + static_cast<float>( m_quad_width );
		float top = static_cast<float>( ( s_screen_height / 2.0f ) - _y );
		float bottom = top - static_cast<float>( m_quad_height );

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

		
		// face count == 2
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 normal;

		auto set_tangent_space = []( VertexType& v, const XMFLOAT3 tangent, const XMFLOAT3 binormal, const XMFLOAT3 normal ) -> void
		{
			v.normal = normal;
			v.tangent = tangent;
			v.binormal = binormal;
		};

		for ( int i = 0; i < 6; i += 3 )
		{
			VertexType& v1 = m_vertices[ i ];
			VertexType& v2 = m_vertices[ i + 1 ];
			VertexType& v3 = m_vertices[ i + 2 ];

			CalculateTangentBinormal( v1, v2, v3, tangent, binormal );

			CalculateNormal( tangent, binormal, normal );

			set_tangent_space( v1, tangent, binormal, normal );
			set_tangent_space( v2, tangent, binormal, normal );
			set_tangent_space( v3, tangent, binormal, normal );
		}
	}

	bool TangentSpaceQuad::UpdateGpuData( ID3D11DeviceContext* _context )
	{
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

	void TangentSpaceQuad::CalculateTangentBinormal( const VertexType& v1, const VertexType& v2, const VertexType& v3, XMFLOAT3& tangent, XMFLOAT3& binormal )
	{
		XMFLOAT3 vector1{ v2.pos.x - v1.pos.x, v2.pos.y - v1.pos.y, v2.pos.z - v1.pos.z };
		XMFLOAT3 vector2{ v3.pos.x - v1.pos.x, v3.pos.y - v1.pos.y, v3.pos.z - v1.pos.z };

		XMFLOAT2 tex1{ v2.tex.x - v1.tex.x, v2.tex.y - v1.tex.y };
		XMFLOAT2 tex2{ v3.tex.x - v1.tex.x, v3.tex.y - v1.tex.y };

		float denominator = 1.0f / ( tex1.x * tex2.y - tex2.x * tex1.y );
		assert( 0 != denominator );

		tangent.x = ( tex2.y * vector1.x - tex1.y * vector2.x ) / denominator;
		tangent.y = ( tex2.y * vector1.y - tex1.y * vector2.y ) / denominator;
		tangent.z = ( tex2.y * vector1.z - tex1.y * vector2.z ) / denominator;

		binormal.x = ( tex1.x * vector2.x - tex2.x * vector1.x ) / denominator;
		binormal.y = ( tex1.x * vector2.y - tex2.x * vector1.y ) / denominator;
		binormal.z = ( tex1.x * vector2.z - tex2.x * vector1.z ) / denominator;

		float length{ 0.0f };

		length = sqrt( ( tangent.x * tangent.x ) + ( tangent.y * tangent.y ) + ( tangent.z * tangent.z ) );
		assert( 0.0 != length );
		tangent.x /= length;
		tangent.y /= length;
		tangent.z /= length;

		length = sqrt( ( binormal.x * binormal.x ) + ( binormal.y * binormal.y ) + ( binormal.z * binormal.z ) );
		assert( 0.0 != length );
		binormal.x /= length;
		binormal.y /= length;
		binormal.z /= length;
	}

	void TangentSpaceQuad::CalculateNormal( const XMFLOAT3& tangent, const XMFLOAT3& binormal, XMFLOAT3& normal )
	{
		normal.x = ( tangent.y * binormal.z ) - ( tangent.z * binormal.y );
		normal.y = ( tangent.z * binormal.x ) - ( tangent.x * binormal.z );
		normal.z = ( tangent.x * binormal.y ) - ( tangent.y * binormal.x );

		float length = sqrt( ( normal.x * normal.x ) + ( normal.y * normal.y ) + ( normal.z * normal.z ) );
		assert( 0.0 != length );
		normal.x /= length;
		normal.y /= length;
		normal.z /= length;
	}
}