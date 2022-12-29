#include "pch.h"
#include "Model.h"

namespace wf
{
#pragma region COLOR MODEL
    ColorModel::ColorModel()
    {
    }

    ColorModel::ColorModel( const ColorModel& )
    {
    }

    ColorModel::~ColorModel()
    {
    }

    bool ColorModel::Initialize( ID3D11Device* _device )
    {
        if ( !InitializeBuffers( _device ) )
        {
            return false;
        }

        return true;
    }

    void ColorModel::Shutdown()
    {
        ShutdownBuffers();
    }

    void ColorModel::Render( ID3D11DeviceContext* _context )
    {
        RenderBuffers( _context );
    }

    void ColorModel::RenderTessellation( ID3D11DeviceContext* _context )
    {
        RenderTessellationBuffers( _context );
    }

    int ColorModel::GetIndexCount() const
    {
        return m_index_count;
    }

    bool ColorModel::InitializeBuffers( ID3D11Device* _device )
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

	void ColorModel::ShutdownBuffers()
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

    void ColorModel::RenderBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );
        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );
        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    }

    void ColorModel::RenderTessellationBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );
        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );
        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
    }

#pragma endregion

#pragma region TEXTURE MODEL

    TextureModel::TextureModel()
    {
    }

    TextureModel::TextureModel( const TextureModel& )
    {
    }

    TextureModel::~TextureModel()
    {
    }


    bool TextureModel::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path )
    {
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

    void TextureModel::Shutdown()
    {
        ReleaseTexture();

        ShutdownBuffers();
    }

    void TextureModel::Render( ID3D11DeviceContext* _context )
    {
        RenderBuffers( _context );
    }

    int TextureModel::GetIndexCount() const
    {
        return m_index_count;
    }

    ID3D11ShaderResourceView* TextureModel::GetTexture()
    {
        return m_texture->GetTexture();;
    }

    bool TextureModel::InitializeBuffers( ID3D11Device* _device )
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
        vertices[ 0 ].tex = XMFLOAT2( 0.0f, 1.0f );

        vertices[ 1 ].pos = XMFLOAT3( 0.0f, 1.0f, 0.0f );
        vertices[ 1 ].tex = XMFLOAT2( 0.5f, 0.0f );

        vertices[ 2 ].pos = XMFLOAT3( 1.0f, -1.0f, 0.0f );
        vertices[ 2 ].tex = XMFLOAT2( 1.0f, 1.0f );

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

    void TextureModel::ShutdownBuffers()
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

    void TextureModel::RenderBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    }

    bool TextureModel::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const  char* _path )
    {
        m_texture = new Texture();

        if ( !m_texture->Initialize( _device, _context, _path ) )
        {
            return false;
        }

        return true;
    }

    void TextureModel::ReleaseTexture()
    {
        if ( m_texture )
        {
            m_texture->Shutdown();
            delete m_texture;
            m_texture = nullptr;
        }
    }

#pragma endregion

#pragma region LIGHT MODEL
    LightModel::LightModel()
    {
    }

    LightModel::LightModel( const LightModel& )
    {
    }

    LightModel::~LightModel()
    {
    }

    bool LightModel::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path )
    {
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

    void LightModel::Shutdown()
    {
        ReleaseTexture();

        ShutdownBuffers();
    }

    void LightModel::Render( ID3D11DeviceContext* _context )
    {
        RenderBuffers( _context );
    }

    int LightModel::GetIndexCount() const
    {
        return m_index_count;
    }

    ID3D11ShaderResourceView* LightModel::GetTexture()
    {
        return m_texture->GetTexture();
    }

    bool LightModel::InitializeBuffers( ID3D11Device* _device )
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
        vertices[ 0 ].tex = XMFLOAT2( 0.0f, 1.0f );
        vertices[ 0 ].normal = XMFLOAT3( 0.0f, 0.0f, -1.0f );

        vertices[ 1 ].pos = XMFLOAT3( 0.0f, 1.0f, 0.0f );
        vertices[ 1 ].tex = XMFLOAT2( 0.5f, 0.0f );
        vertices[ 1 ].normal = XMFLOAT3( 0.0f, 0.0f, -1.0f );

        vertices[ 2 ].pos = XMFLOAT3( 1.0f, -1.0f, 0.0f );
        vertices[ 2 ].tex = XMFLOAT2( 1.0f, 1.0f );
        vertices[ 2 ].normal = XMFLOAT3( 0.0f, 0.0f, -1.0f );

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

    void LightModel::ShutdownBuffers()
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

    void LightModel::RenderBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    }

    bool LightModel::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path )
    {
        m_texture = new Texture();

        if ( !m_texture->Initialize( _device, _context, _path ) )
        {
            return false;
        }

        return true;
    }

    void LightModel::ReleaseTexture()
    {
        if ( m_texture )
        {
            m_texture->Shutdown();
            delete m_texture;
            m_texture = nullptr;
        }
    }

#pragma endregion

#pragma region RASTERTEK MODEL
    RasterTekModel::RasterTekModel()
    {
    }

    RasterTekModel::RasterTekModel( const LightModel& )
    {
    }

    RasterTekModel::~RasterTekModel()
    {
    }

    bool RasterTekModel::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _texture_path, const char* _model_path )
    {
        if ( !LoadModel( _model_path ) )
        {
            return false;
        }

        if ( !InitializeBuffers( _device ) )
        {
            return false;
        }

        if ( !LoadTexture( _device, _context, _texture_path ) )
        {
            return false;
        }

        return true;
    }

    void RasterTekModel::Shutdown()
    {
        ReleaseTexture();

        ShutdownBuffers();

        ReleaseModel();
    }

    void RasterTekModel::Render( ID3D11DeviceContext* _context )
    {
        RenderBuffers( _context );
    }

    int RasterTekModel::GetIndexCount() const
    {
        return m_index_count;
    }

    ID3D11ShaderResourceView* RasterTekModel::GetTexture()
    {
        return m_texture->GetTexture();
    }

    bool RasterTekModel::InitializeBuffers( ID3D11Device* _device )
    {
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

        for ( int i = 0; i < m_vertex_count; ++i )
        {
            vertices[ i ].pos = XMFLOAT3( m_vertex_data[ i ].x, m_vertex_data[ i ].y, m_vertex_data[ i ].z );
            vertices[ i ].tex = XMFLOAT2( m_vertex_data[ i ].u, m_vertex_data[ i ].v );
            vertices[ i ].normal = XMFLOAT3( m_vertex_data[ i ].nx, m_vertex_data[ i ].ny, m_vertex_data[ i ].nz );

            indices[ i ] = i;
        }

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

    void RasterTekModel::ShutdownBuffers()
    {
        SAFE_RELEASE( m_index_buffer );
        SAFE_RELEASE( m_vertex_buffer );
    }

    void RasterTekModel::RenderBuffers( ID3D11DeviceContext* _context )
    {
        UINT stride{ sizeof( VertexType ) };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &m_vertex_buffer, &stride, &offset );

        _context->IASetIndexBuffer( m_index_buffer, DXGI_FORMAT_R32_UINT, 0 );

        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    }

    bool RasterTekModel::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _texture_path )
    {
        m_texture = new Texture();
        if ( !m_texture )
        {
            return false;
        }

        if ( !m_texture->Initialize( _device, _context, _texture_path ) )
        {
            return false;
        }

        return true;
    }

    void RasterTekModel::ReleaseTexture()
    {
        SAFE_SHUTDOWN( m_texture );
    }

    bool RasterTekModel::LoadModel( const char* _model_path )
    {
        std::ifstream file( _model_path );
        if ( !file.is_open() )
        {
            return false;
        }

        char ch;
        file.get( ch );
        while ( ':' != ch )
        {
            file.get( ch );
        }

        file >> m_vertex_count;
        m_index_count = m_vertex_count;

        m_vertex_data = new VertexData[ m_vertex_count ];
        if ( !m_vertex_data )
        {
            file.close();
            return false;
        }

        file.get( ch );
        while ( ':' != ch )
        {
            file.get( ch );
        }

        file.get( ch );
        file.get( ch );

        for ( int i = 0; i < m_vertex_count; ++i )
        {
            file >> m_vertex_data[ i ].x >> m_vertex_data[ i ].y >> m_vertex_data[ i ].z;
            file >> m_vertex_data[ i ].u >> m_vertex_data[ i ].v;
            file >> m_vertex_data[ i ].nx >> m_vertex_data[ i ].ny >> m_vertex_data[ i ].nz;
        }

        file.close();

        return true;
    }

    void RasterTekModel::ReleaseModel()
    {
        if ( m_vertex_data )
        {
            delete[] m_vertex_data;
            m_vertex_data = nullptr;
        }
    }
#pragma endregion
}