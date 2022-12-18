#include "pch.h"
#include "ModelLoader.h"

namespace wf
{
	ModelLoader::ModelLoader()
	{
	}

	ModelLoader::ModelLoader( const ModelLoader& )
	{
	}

	ModelLoader::~ModelLoader()
	{
	}

    void ModelLoader::Shutdown()
    {
        for ( auto& pair : m_model_datas )
        {
            SAFE_RELEASE( pair.second->index_buffer );
            SAFE_RELEASE( pair.second->vertex_buffer );

            SAFE_DELETE( pair.second );
        }

        m_model_datas.clear();
    }

	bool ModelLoader::LoadRastertekModel( const ModelParam& _param )
	{
        ModelData* model_data = new ModelData;
        if ( !model_data )
        {
            return false;
        }

        model_data->stride = sizeof( RastertekVertex );

        RastertekVertexData* data{ nullptr };
        if ( !LoadData( _param, data, model_data ) )
        {
            SAFE_DELETE_ARRAY( data );
            SAFE_DELETE( model_data );
            return false;
        }

        if ( !InitializeRastertekModelBuffers( _param, data, model_data ) )
        {
            SAFE_DELETE_ARRAY( data );
            SAFE_DELETE( model_data );
            return false;
        }

        m_model_datas.insert( std::make_pair( _param.key, model_data ) );

        SAFE_DELETE_ARRAY( data );

        return true;
	}

	bool ModelLoader::LoadTangentSpaceModel( const ModelParam& _param )
	{
        ModelData* model_data = new ModelData;
        if ( !model_data )
        {
            return false;
        }

        model_data->stride = sizeof( TangentSpaceVertex );
        
        RastertekVertexData* data{ nullptr };
        if ( !LoadData( _param, data, model_data ) )
        {
            SAFE_DELETE_ARRAY( data );
            SAFE_DELETE( model_data );
            return false;
        }

        TangentSpaceVertex* vertices = new TangentSpaceVertex[ model_data->vertex_count ];
        if ( !vertices )
        {
            SAFE_DELETE_ARRAY( data );
            SAFE_DELETE( model_data );
            return false;
        }

        CalculateVectors( data, vertices, model_data );

        if ( !InitializeTangentSpaceModelBuffers( _param, vertices, model_data ) )
        {
            SAFE_DELETE_ARRAY( vertices );
            SAFE_DELETE_ARRAY( data );
            SAFE_DELETE( model_data );
            return false;
        }

        m_model_datas.insert( std::make_pair( _param.key, model_data ) );

        SAFE_DELETE_ARRAY( vertices );
        SAFE_DELETE_ARRAY( data );

        return true;
	}

    ModelData* ModelLoader::GetModelData( std::wstring _key )
    {
        auto iter = m_model_datas.find( _key );
        return iter != m_model_datas.end() ? iter->second : nullptr;
    }

    bool ModelLoader::LoadData( const ModelParam& _param, RastertekVertexData*& _data, ModelData*& _model_data )
    {
        std::ifstream file( _param.path );
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

        file >> _model_data->vertex_count;
        _model_data->index_count = _model_data->vertex_count;

        _data = new RastertekVertexData[ _model_data->vertex_count ];
        if ( !_data )
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

        for ( int i = 0; i < _model_data->vertex_count; ++i )
        {
            file >> _data[ i ].x >> _data[ i ].y >> _data[ i ].z;
            file >> _data[ i ].u >> _data[ i ].v;
            file >> _data[ i ].nx >> _data[ i ].ny >> _data[ i ].nz;
        }

        file.close();

        return true;
    }

    bool ModelLoader::InitializeRastertekModelBuffers( const ModelParam& _param, const RastertekVertexData* _data, ModelData*& _model_data )
    {
        RastertekVertex* vertices = new RastertekVertex[ _model_data->vertex_count ];
        UINT* indices = new UINT[ _model_data->index_count ];
        if ( !vertices || !indices )
        {
            SAFE_DELETE_ARRAY( indices );
            SAFE_DELETE_ARRAY( vertices );
            return false;
        }

        for ( int i = 0; i < _model_data->vertex_count; ++i )
        {
            vertices[ i ].pos = XMFLOAT3( _data[ i ].x, _data[ i ].y, _data[ i ].z );
            vertices[ i ].tex = XMFLOAT2( _data[ i ].u, _data[ i ].v );
            vertices[ i ].normal = XMFLOAT3( _data[ i ].nx, _data[ i ].ny, _data[ i ].nz );

            indices[ i ] = i;
        }

        D3D11_BUFFER_DESC desc{};
        D3D11_SUBRESOURCE_DATA data{};

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( RastertekVertex ) * _model_data->vertex_count;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        ZeroMemory( &data, sizeof( data ) );
        data.pSysMem = vertices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        HRESULT hr = _param.device->CreateBuffer( &desc, &data, &_model_data->vertex_buffer );
        if ( FAILED( hr ) )
        {
            SAFE_DELETE_ARRAY( indices );
            SAFE_DELETE_ARRAY( vertices );
            return false;
        }

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( UINT ) * _model_data->index_count;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        ZeroMemory( &data, sizeof( data ) );
        data.pSysMem = indices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        hr = _param.device->CreateBuffer( &desc, &data, &_model_data->index_buffer );
        if ( FAILED( hr ) )
        {
            SAFE_DELETE_ARRAY( indices );
            SAFE_DELETE_ARRAY( vertices );
            return false;
        }

        SAFE_DELETE_ARRAY( indices );
        SAFE_DELETE_ARRAY( vertices );

        return true;;
    }

    bool ModelLoader::InitializeTangentSpaceModelBuffers( const ModelParam& _param, TangentSpaceVertex*& _vertices, ModelData*& _model_data )
    {
        UINT* indices = new UINT[ _model_data->index_count ];
        if ( !indices )
        {
            SAFE_DELETE_ARRAY( indices );
            return false;
        }

        for ( int i = 0; i < _model_data->index_count; ++i )
        {
            indices[ i ] = i;
        }

        D3D11_BUFFER_DESC desc{};
        D3D11_SUBRESOURCE_DATA data{};

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( TangentSpaceVertex ) * _model_data->vertex_count;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        ZeroMemory( &data, sizeof( data ) );
        data.pSysMem = _vertices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        HRESULT hr = _param.device->CreateBuffer( &desc, &data, &_model_data->vertex_buffer );
        if ( FAILED( hr ) )
        {
            SAFE_DELETE_ARRAY( indices );
            return false;
        }

        ZeroMemory( &desc, sizeof( desc ) );
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof( UINT ) * _model_data->index_count;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        ZeroMemory( &data, sizeof( data ) );
        data.pSysMem = indices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        hr = _param.device->CreateBuffer( &desc, &data, &_model_data->index_buffer );
        if ( FAILED( hr ) )
        {
            SAFE_DELETE_ARRAY( indices );
            return false;
        }

        SAFE_DELETE_ARRAY( indices );

        return true;
    }

    void ModelLoader::CalculateVectors( const RastertekVertexData* _data, TangentSpaceVertex*& _vertices, ModelData*& _model_data )
    {
        RastertekVertexData v1;
        RastertekVertexData v2;
        RastertekVertexData v3;

        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
        XMFLOAT3 normal;

        int face_count = _model_data->vertex_count / 3;
        int index = 0;

        auto set_data_by_index = [&]( RastertekVertexData& v, const RastertekVertexData* data, int index ) -> void
        {
            v.x = data[ index ].x;
            v.y = data[ index ].y;
            v.z = data[ index ].z;
            v.u = data[ index ].u;
            v.v = data[ index ].v;
            v.nx = data[ index ].nx;
            v.ny = data[ index ].ny;
            v.nz = data[ index ].nz;
        };

        auto set_vertex_data = [&]( int index ) -> void
        {
            _vertices[ index ].tangent = tangent;
            _vertices[ index ].binormal = binormal;
            _vertices[ index ].normal = normal;
        };

        for ( int i = 0; i < _model_data->vertex_count; ++i )
        {
            _vertices[ i ].pos = XMFLOAT3( _data[ i ].x, _data[ i ].y, _data[ i ].z );
            _vertices[ i ].tex = XMFLOAT2( _data[ i ].u, _data[ i ].v );
            _vertices[ i ].normal = XMFLOAT3( _data[ i ].nx, _data[ i ].ny, _data[ i ].nz );
        }


        for ( int i = 0; i < face_count; ++i )
        {
            set_data_by_index( v1, _data, index );
            ++index;
            set_data_by_index( v2, _data, index );
            ++index;
            set_data_by_index( v3, _data, index );
            ++index;

            CalculateTangentBinormal( v1, v2, v3, tangent, binormal );

            CalculateNormal( tangent, binormal, normal );

            set_vertex_data( index - 1 );
            set_vertex_data( index - 2 );
            set_vertex_data( index - 3 );
        }

    }

    void ModelLoader::CalculateTangentBinormal( const RastertekVertexData& v1, const RastertekVertexData& v2, const RastertekVertexData& v3, XMFLOAT3& tangent, XMFLOAT3& binormal )
    {
        XMFLOAT3 vector1{ v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
        XMFLOAT3 vector2{ v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };
        
        XMFLOAT2 tex1{ v2.u - v1.u, v2.v - v1.v };
        XMFLOAT2 tex2{ v3.u - v1.u, v3.v - v1.v };

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

    void ModelLoader::CalculateNormal( const XMFLOAT3& tangent, const XMFLOAT3& binormal, XMFLOAT3& normal )
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

    void RenderModelData( ID3D11DeviceContext* _context, ModelData* _model_data )
    {
        UINT stride{ _model_data->stride };
        UINT offset{ 0 };

        _context->IASetVertexBuffers( 0, 1, &_model_data->vertex_buffer, &stride, &offset );

        _context->IASetIndexBuffer( _model_data->index_buffer, DXGI_FORMAT_R32_UINT, 0 );

        _context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    }
}