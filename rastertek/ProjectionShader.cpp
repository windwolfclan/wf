#include "pch.h"
#include "ProjectionShader.h"

namespace wf
{
	ProjectionShader::ProjectionShader()
	{
		m_vs = L"projection_vs.hlsl";
		m_ps = L"projection_ps.hlsl";
		m_vs_main = "projection_vs";
		m_ps_main = "projection_ps";
	}

	ProjectionShader::ProjectionShader( const ProjectionShader& )
	{
	}

	ProjectionShader::~ProjectionShader()
	{
	}

	bool ProjectionShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _srv,
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse,
		XMFLOAT3 _direction,
		XMMATRIX _v2,
		XMMATRIX _p2,
		ID3D11ShaderResourceView* _projection
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _ambient, _diffuse, _direction, _v2, _p2, _projection ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool ProjectionShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _srv,
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse,
		XMFLOAT3 _direction,
		XMMATRIX _v2,
		XMMATRIX _p2,
		ID3D11ShaderResourceView* _projection
	)
	{
		{
			_w = XMMatrixTranspose( _w );
			_v = XMMatrixTranspose( _v );
			_p = XMMatrixTranspose( _p );
			_v2 = XMMatrixTranspose( _v2 );
			_p2 = XMMatrixTranspose( _p2 );

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				MatrixBufferType* data = (MatrixBufferType*)mappedResource.pData;
				data->w = _w;
				data->v = _v;
				data->p = _p;
				data->v2 = _v2;
				data->p2 = _p2;

				_context->Unmap( m_matrix_buffer, 0 );
			}

			UINT slot = 0;
			_context->VSSetConstantBuffers( slot, 1, &m_matrix_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightBufferType* data = (LightBufferType*)mappedResource.pData;
				data->ambient = _ambient;
				data->diffuse = _diffuse;
				data->direction = _direction;
				data->padding = 0.0f;

				_context->Unmap( m_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_srv );
		_context->PSSetShaderResources( 1, 1, &_projection );

		return true;
	}

	void ProjectionShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );


		ID3D11ShaderResourceView* null_view{ nullptr };
		_context->PSSetShaderResources( 0, 1, &null_view );
		_context->PSSetShaderResources( 1, 1, &null_view );
	}

	bool ProjectionShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[ 3 ] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		HRESULT hr = _device->CreateInputLayout( layout_desc, NumElements, _blob->GetBufferPointer(), _blob->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	bool ProjectionShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_light_buffer, sizeof( LightBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateWrapSampler( _device, m_sampler_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void ProjectionShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

	LightProjectionShader::LightProjectionShader()
	{
		m_vs = L"projection_vs.hlsl";
		m_ps = L"projection_ps.hlsl";
		m_vs_main = "projection_vs2";
		m_ps_main = "projection_ps2";
	}

	bool LightProjectionShader::Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, XMFLOAT4 _ambient, XMFLOAT4 _diffuse, XMFLOAT3 _light_pos, XMMATRIX _v2, XMMATRIX _p2, ID3D11ShaderResourceView* _projection )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _ambient, _diffuse, _light_pos, _v2, _p2, _projection ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool LightProjectionShader::SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, XMFLOAT4 _ambient, XMFLOAT4 _diffuse, XMFLOAT3 _light_pos, XMMATRIX _v2, XMMATRIX _p2, ID3D11ShaderResourceView* _projection )
	{
		{
			_w = XMMatrixTranspose( _w );
			_v = XMMatrixTranspose( _v );
			_p = XMMatrixTranspose( _p );
			_v2 = XMMatrixTranspose( _v2 );
			_p2 = XMMatrixTranspose( _p2 );

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				MatrixBufferType* data = (MatrixBufferType*)mappedResource.pData;
				data->w = _w;
				data->v = _v;
				data->p = _p;
				data->v2 = _v2;
				data->p2 = _p2;

				_context->Unmap( m_matrix_buffer, 0 );
			}

			UINT slot = 0;
			_context->VSSetConstantBuffers( slot, 1, &m_matrix_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_light_position_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightPositionBuffer* data = (LightPositionBuffer*)mappedResource.pData;
				data->position = _light_pos;

				_context->Unmap( m_light_position_buffer, 0 );
			}

			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_light_position_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightBufferType* data = (LightBufferType*)mappedResource.pData;
				data->ambient = _ambient;
				data->diffuse = _diffuse;
				data->direction = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
				data->padding = 0.0f;

				_context->Unmap( m_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_srv );
		_context->PSSetShaderResources( 1, 1, &_projection );

		return true;
	}

	void LightProjectionShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );

		ID3D11ShaderResourceView* null_view{ nullptr };
		_context->PSSetShaderResources( 0, 1, &null_view );
		_context->PSSetShaderResources( 1, 1, &null_view );
	}

	bool LightProjectionShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_light_position_buffer, sizeof( LightPositionBuffer ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_light_buffer, sizeof( LightBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateWrapSampler( _device, m_sampler_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void LightProjectionShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_buffer );
		SAFE_RELEASE( m_light_position_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

}