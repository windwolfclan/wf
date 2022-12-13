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

	bool LightShader::Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, XMFLOAT4 _ambient, XMFLOAT4 _diffuse, float _specular_power, XMFLOAT4 _specular, XMFLOAT3 _camera_pos, XMFLOAT3 _light_dir )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _ambient, _diffuse, _specular_power, _specular, _camera_pos, _light_dir ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool LightShader::SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, XMFLOAT4 _ambient, XMFLOAT4 _diffuse, float _specular_power, XMFLOAT4 _specular, XMFLOAT3 _camera_pos, XMFLOAT3 _light_dir )
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

		_context->DrawIndexed( _index_count, 0, 0 );
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
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( MatrixBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		HRESULT hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_matrix_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( CameraBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_camera_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( LightBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_light_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_SAMPLER_DESC sampler_desc{};
		ZeroMemory( &sampler_desc, sizeof( sampler_desc ) );
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[ 0 ] = 0.0f;
		sampler_desc.BorderColor[ 1 ] = 0.0f;
		sampler_desc.BorderColor[ 2 ] = 0.0f;
		sampler_desc.BorderColor[ 3 ] = 0.0f;
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = _device->CreateSamplerState( &sampler_desc, &m_sampler_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void LightShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_buffer );
		SAFE_RELEASE( m_camera_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

}