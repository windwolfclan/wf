#include "pch.h"
#include "WaterShader.h"

namespace wf
{
	WaterShader::WaterShader()
	{
		m_vs = L"water_vs.hlsl";
		m_ps = L"water_ps.hlsl";
		m_vs_main = "water_vs";
		m_ps_main = "water_ps";
	}

	WaterShader::WaterShader( const WaterShader& )
	{
	}

	WaterShader::~WaterShader()
	{
	}

	bool WaterShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		XMMATRIX _r,
		ID3D11ShaderResourceView* _reflect,
		ID3D11ShaderResourceView* _refract,
		ID3D11ShaderResourceView* _normal,
		float _water_translation,
		float _reflect_refract_scale
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _r, _reflect, _refract, _normal, _water_translation, _reflect_refract_scale ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool WaterShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		XMMATRIX _r,
		ID3D11ShaderResourceView* _reflect,
		ID3D11ShaderResourceView* _refract,
		ID3D11ShaderResourceView* _normal,
		float _water_translation,
		float _reflect_refract_scale
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
			_r = XMMatrixTranspose( _r );

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_reflect_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				ReflectBufferType* data = (ReflectBufferType*)mappedResource.pData;
				data->r = _r;

				_context->Unmap( m_reflect_buffer, 0 );
			}

			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_reflect_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_water_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				WaterBufferType* data = (WaterBufferType*)mappedResource.pData;
				data->water_translation = _water_translation;
				data->reflect_refract_scale = _reflect_refract_scale;

				_context->Unmap( m_water_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_water_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_reflect );
		_context->PSSetShaderResources( 1, 1, &_refract );
		_context->PSSetShaderResources( 2, 1, &_normal );

		return true;
	}

	void WaterShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );


		ID3D11ShaderResourceView* null_view{ nullptr };
		_context->PSSetShaderResources( 0, 1, &null_view );
		_context->PSSetShaderResources( 1, 1, &null_view );
		_context->PSSetShaderResources( 2, 1, &null_view );
	}

	bool WaterShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool WaterShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_reflect_buffer, sizeof( ReflectBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_water_buffer, sizeof( WaterBufferType ) );
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

	void WaterShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_water_buffer );
		SAFE_RELEASE( m_reflect_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}
}