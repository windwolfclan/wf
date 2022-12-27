#include "pch.h"
#include "MultiLightShader.h"

namespace wf
{
	MultiLightShader::MultiLightShader()
	{
		m_vs = L"multi_light_vs.hlsl";
		m_ps = L"multi_light_ps.hlsl";
		m_vs_main = "multi_light_vs";
		m_ps_main = "multi_light_ps";
	}

	MultiLightShader::MultiLightShader( const MultiLightShader& )
	{
	}

	MultiLightShader::~MultiLightShader()
	{
	}

	bool MultiLightShader::Render( 
		ID3D11DeviceContext* _context, 
		int _index_count, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _srv, 
		LightPositionBuffer& _position, 
		LightColorBuffer& _diffuse 
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _position, _diffuse ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool MultiLightShader::SetShaderParameters( 
		ID3D11DeviceContext* _context, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _srv, 
		LightPositionBuffer& _position, 
		LightColorBuffer& _diffuse 
	)
	{
		{
			_w = XMMatrixTranspose( _w );
			_v = XMMatrixTranspose( _v );
			_p = XMMatrixTranspose( _p );

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				MatrixBufferType* data = (MatrixBufferType*)mappedResource.pData;
				data->w = _w;
				data->v = _v;
				data->p = _p;

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
				for ( int i = 0; i < LIGHT_COUNT; ++i )
				{
					data->position[ i ] = _position.position[ i ];
				}

				_context->Unmap( m_light_position_buffer, 0 );
			}

			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_light_position_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_light_color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightColorBuffer* data = (LightColorBuffer*)mappedResource.pData;
				for ( int i = 0; i < LIGHT_COUNT; ++i )
				{
					data->diffuse[ i ] = _diffuse.diffuse[ i ];
				}

				_context->Unmap( m_light_color_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_color_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_srv );

		return true;
	}

	void MultiLightShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool MultiLightShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[ 3 ] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		HRESULT hr = _device->CreateInputLayout( layout_desc, NumElements, _blob->GetBufferPointer(), _blob->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	bool MultiLightShader::InitializeBuffers( ID3D11Device* _device )
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

		hr = utility::CreateDynamicWriteBuffer( _device, m_light_color_buffer, sizeof( LightColorBuffer ) );
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

	void MultiLightShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_color_buffer );
		SAFE_RELEASE( m_light_position_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

}