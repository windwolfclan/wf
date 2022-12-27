#pragma once

namespace wf
{
	namespace utility
	{
		HRESULT CreateDynamicWriteBuffer( ID3D11Device* _device, ID3D11Buffer*& _buffer, UINT _ByteWidth );
		HRESULT CreateWrapSampler( ID3D11Device* _device, ID3D11SamplerState*& _sampler_state );
		HRESULT CreateClampSampler( ID3D11Device* _device, ID3D11SamplerState*& _sampler_state );
	}
}