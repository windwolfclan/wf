#include "pch.h"
#include "Utility.h"


namespace wf 
{
	namespace utility
	{
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

		std::vector<float> CreateGaussianKernel( const size_t length )
		{
			assert( length % 2 == 1 );

			std::vector<float> kernel;
			kernel.resize( length, 0.0 );

			const double sigma{ 1.5 };
			const double s{ 2.0 * sigma * sigma };
			double sum{ 0.0 };

			int half = static_cast<int>( length ) / 2;
			for ( int i = -half; i <= half; ++i )
			{
				double r = sqrt( i * i );

				kernel[ i + half ] = static_cast<float>( ( exp( -( r * r ) / s ) ) / ( XM_PI * s ) );
				sum += kernel[ i + half ];
			}

			for ( size_t i = 0; i < length; ++i )
			{
				kernel[ i ] /= static_cast<float>( sum );
			}

			return kernel;
		}

	}
}