#include "pch.h"
#include "Utility.h"


namespace wf 
{
	namespace utility
	{
		HRESULT CreateWrapSampler( ID3D11Device* _device, ID3D11SamplerState*& _sampler_state )
		{
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

			return _device->CreateSamplerState( &sampler_desc, &_sampler_state );
		}

		HRESULT CreateClampSampler( ID3D11Device* _device, ID3D11SamplerState*& _sampler_state )
		{
			D3D11_SAMPLER_DESC sampler_desc{};
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

			return _device->CreateSamplerState( &sampler_desc, &_sampler_state );
		}

		HRESULT CreateDynamicWriteBuffer( ID3D11Device* _device, ID3D11Buffer*& _buffer, UINT _ByteWidth )
		{
			D3D11_BUFFER_DESC buffer_desc;
			ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.ByteWidth = _ByteWidth;
			buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;

			return _device->CreateBuffer( &buffer_desc, nullptr, &_buffer );
		}

	}
}