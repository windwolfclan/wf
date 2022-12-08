#include "Model.h"

namespace wf
{
    Model::Model()
    {
    }

    Model::Model( const Model& )
    {
    }

    Model::~Model()
    {
    }

    bool Model::Initialize( ID3D11Device* _device )
    {
        if ( !InitializeBuffers( _device ) )
        {
            return false;
        }

        return true;
    }

    void Model::Shutdown()
    {
        ShutdownBuffers();
    }

    void Model::Render( ID3D11DeviceContext* _context )
    {
        RenderBuffers( _context );
    }

    int Model::GetIndexCount() const
    {
        return m_index_count;
    }

    bool Model::InitializeBuffers( ID3D11Device* _device )
    {
        m_vertex_count = 3;
        m_index_count = 3;

        VertexType* vertices = new VertexType[ m_vertex_count ];
        UINT* indices = new UINT[ m_index_count ];
        if ( !vertices || !indices )
        {
            if ( vertices )
            {
                delete[] vertices;
            }

            if ( indices )
            {
                delete[] indices;
            }

            return false;
        }

        vertices[ 0 ].pos = XMFLOAT3( -1.0f, -1.0f, 0.0f );
        vertices[ 0 ].color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

        vertices[ 1 ].pos = XMFLOAT3( 0.0f, 1.0f, 0.0f );
        vertices[ 1 ].color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
        
        vertices[ 2 ].pos = XMFLOAT3( 1.0f, -1.0f, 0.0f );
        vertices[ 2 ].color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );

        indices[ 0 ] = 0;
        indices[ 1 ] = 1;
        indices[ 2 ] = 2;


        D3D11_BUFFER_DESC desc{};
        D3D11_SUBRESOURCE_DATA data{};

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( VertexType ) * m_vertex_count;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        ZeroMemory( &data, sizeof( data ) );
        data.pSysMem = vertices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        HRESULT hr = _device->CreateBuffer( &desc, &data, &m_vertex_buffer );
        if ( FAILED( hr ) )
        {
            return false;
        }

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( UINT ) * m_index_count;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = _device->CreateBuffer( &desc, &data, &m_index_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		delete[] vertices;
		vertices = nullptr;
		delete[] indices;
		indices = nullptr;

		return true;
	}

	void Model::ShutdownBuffers()
    {
        if ( m_index_buffer )
        {
            m_index_buffer->Release();
            m_index_buffer = nullptr;
        }

        if ( m_vertex_buffer )
        {
            m_vertex_buffer->Release();
            m_vertex_buffer = nullptr;
        }
    }

    void Model::RenderBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );
        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );
        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    }
}