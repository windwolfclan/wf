#include "pch.h"
#include "LightShader.h"

namespace wf
{
	LightShader::LightShader()
	{
		m_vs = L"light_vs.hlsl";
		m_ps = L"light_ps.hlsl";
		m_vs_main = "LightVertexShader";
		m_ps_main = "LightPixelShader";
	}

	LightShader::LightShader( const LightShader& )
	{
	}

	LightShader::~LightShader()
	{
	}

	bool LightShader::Render( 
		ID3D11DeviceContext* _context,
		int _index_count, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p,
		ID3D11ShaderResourceView* _srv, 
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse, 
		float _specular_power, 
		XMFLOAT4 _specular, 
		XMFLOAT3 _camera_pos, 
		XMFLOAT3 _light_dir 
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _ambient, _diffuse, _specular_power, _specular, _camera_pos, _light_dir ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool LightShader::SetShaderParameters( 
		ID3D11DeviceContext* _context,
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _srv,
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse,
		float _specular_power,
		XMFLOAT4 _specular,
		XMFLOAT3 _camera_pos, 
		XMFLOAT3 _light_dir
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
			if ( SUCCEEDED( _context->Map( m_camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				CameraBufferType* data = (CameraBufferType*)mappedResource.pData;
				data->pos = _camera_pos;
				data->padding = 0.0f;

				_context->Unmap( m_camera_buffer, 0 );
			}
			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_camera_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightBufferType* data = (LightBufferType*)mappedResource.pData;
				data->ambient = _ambient;
				data->diffuse = _diffuse;
				data->light_dir = _light_dir;
				data->specular_power = _specular_power;
				data->specular = _specular;

				_context->Unmap( m_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_srv );

		return true;
	}

	void LightShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		if ( m_point_state )
		{
			_context->PSSetSamplers( 1, 1, &m_point_state );
		}

		_context->DrawIndexed( _index_count, 0, 0 );

		ID3D11SamplerState* null_state{ nullptr };
		_context->PSSetSamplers( 1, 1, &null_state );
	}

	bool LightShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool LightShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_camera_buffer, sizeof( CameraBufferType ) );
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

	void LightShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_point_state );
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_buffer );
		SAFE_RELEASE( m_camera_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

	DefferedLightShader::DefferedLightShader()
	{
		m_vs = L"light_vs.hlsl";
		m_ps = L"light_ps.hlsl";
		m_vs_main = "deffered_light_vs";
		m_ps_main = "deffered_light_ps";
	}

	bool DefferedLightShader::Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, ID3D11ShaderResourceView* _normal, XMFLOAT3 _light_dir )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _normal, _light_dir ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool DefferedLightShader::SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, ID3D11ShaderResourceView* _normal, XMFLOAT3 _light_dir )
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
			if ( SUCCEEDED( _context->Map( m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				LightBufferType* data = (LightBufferType*)mappedResource.pData;
				data->light_dir = _light_dir;

				_context->Unmap( m_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_srv );
		_context->PSSetShaderResources( 1, 1, &_normal );

		return true;
	}

	bool DefferedLightShader::InitializeBuffers( ID3D11Device* _device )
	{
		if ( !__super::InitializeBuffers( _device ) )
		{
			return false;
		}

		HRESULT hr = utility::CreateClampSampler( _device, m_point_state );
		if ( FAILED( hr ) )
		{
			return false;
		}


		return true;
	}

}