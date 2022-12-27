#include "pch.h"
#include "FireShader.h"

namespace wf
{
	FireShader::FireShader()
	{
		m_vs = L"fire_vs.hlsl";
		m_ps = L"fire_ps.hlsl";
		m_vs_main = "fire_vs";
		m_ps_main = "fire_ps";
	}

	FireShader::FireShader( const FireShader& )
	{
	}

	FireShader::~FireShader()
	{
	}

	bool FireShader::Render( 
		ID3D11DeviceContext* _context, 
		int _index_count, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _fire,
		ID3D11ShaderResourceView* _noise,
		ID3D11ShaderResourceView* _alpha,
		float _frame_time,
		XMFLOAT3 _scroll_speed,
		XMFLOAT3 _scales,
		XMFLOAT2 _distortion1,
		XMFLOAT2 _distortion2,
		XMFLOAT2 _distortion3,
		float _distortion_scale,
		float _distortion_bias 
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _fire, _noise, _alpha, _frame_time, _scroll_speed, _scales, _distortion1, _distortion2, _distortion3, _distortion_scale, _distortion_bias ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool FireShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _fire,
		ID3D11ShaderResourceView* _noise,
		ID3D11ShaderResourceView* _alpha,
		float _frame_time,
		XMFLOAT3 _scroll_speed,
		XMFLOAT3 _scales,
		XMFLOAT2 _distortion1,
		XMFLOAT2 _distortion2,
		XMFLOAT2 _distortion3,
		float _distortion_scale,
		float _distortion_bias
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
			if ( SUCCEEDED( _context->Map( m_noise_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				NoiseBufferType* data = (NoiseBufferType*)mappedResource.pData;
				data->frame_time = _frame_time;
				data->scroll_speed = _scroll_speed;
				data->scales = _scales;
				data->padding = 0.0f;

				_context->Unmap( m_noise_buffer, 0 );
			}

			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_noise_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_distortion_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				DistortionBufferType* data = (DistortionBufferType*)mappedResource.pData;
				data->distortion1 = _distortion1;
				data->distortion2 = _distortion2;
				data->distortion3 = _distortion3;
				data->distortion_bias = _distortion_bias;
				data->distortion_scale = _distortion_scale;

				_context->Unmap( m_distortion_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_distortion_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_fire );
		_context->PSSetShaderResources( 1, 1, &_noise );
		_context->PSSetShaderResources( 2, 1, &_alpha);

		return true;
	}

	void FireShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state1 );
		_context->PSSetSamplers( 1, 1, &m_sampler_state2 );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool FireShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[ 2 ] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		HRESULT hr = _device->CreateInputLayout( layout_desc, NumElements, _blob->GetBufferPointer(), _blob->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	bool FireShader::InitializeBuffers( ID3D11Device* _device )
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
		buffer_desc.ByteWidth = sizeof( NoiseBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_noise_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( DistortionBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_distortion_buffer );
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

		hr = _device->CreateSamplerState( &sampler_desc, &m_sampler_state1 );
		if ( FAILED( hr ) )
		{
			return false;
		}

		ZeroMemory( &sampler_desc, sizeof( sampler_desc ) );
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[ 0 ] = 0.0f;
		sampler_desc.BorderColor[ 1 ] = 0.0f;
		sampler_desc.BorderColor[ 2 ] = 0.0f;
		sampler_desc.BorderColor[ 3 ] = 0.0f;
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = _device->CreateSamplerState( &sampler_desc, &m_sampler_state2 );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void FireShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state2 );
		SAFE_RELEASE( m_sampler_state1 );
		SAFE_RELEASE( m_distortion_buffer );
		SAFE_RELEASE( m_noise_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}
}