#include "pch.h"
#include "BumpShader.h"

namespace wf
{
	BumpShader::BumpShader()
	{
		m_vs = L"bump_vs.hlsl";
		m_ps = L"bump_ps.hlsl";
		m_vs_main = "bump_vs";
		m_ps_main = "bump_ps";
	}

	BumpShader::BumpShader( const BumpShader& )
	{
	}

	BumpShader::~BumpShader()
	{
	}

	bool BumpShader::Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView** _srv, XMFLOAT4 _diffuse, XMFLOAT3 _light_dir )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv, _diffuse, _light_dir ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool BumpShader::SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView** _srv, XMFLOAT4 _diffuse, XMFLOAT3 _light_dir )
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
				data->diffuse = _diffuse;
				data->light_dir = _light_dir;
				data->padding = 0.0f;

				_context->Unmap( m_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_light_buffer );
		}

		_context->PSSetShaderResources( 0, 2, _srv );

		return true;
	}

	void BumpShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool BumpShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[ 5 ] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		HRESULT hr = _device->CreateInputLayout( layout_desc, NumElements, _blob->GetBufferPointer(), _blob->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	bool BumpShader::InitializeBuffers( ID3D11Device* _device )
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

	void BumpShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_light_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}


}