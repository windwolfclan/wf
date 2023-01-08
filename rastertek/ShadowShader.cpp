#include "pch.h"
#include "ShadowShader.h"

namespace wf
{
	ShadowShader::ShadowShader()
	{
		m_vs = L"shadow_vs.hlsl";
		m_ps = L"shadow_ps.hlsl";
		m_vs_main = "shadow_vs";
		m_ps_main = "shadow_ps";
	}

	ShadowShader::ShadowShader( const ShadowShader& )
	{
	}

	ShadowShader::~ShadowShader()
	{
	}

	bool ShadowShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		XMMATRIX _lv,
		XMMATRIX _lp,
		ID3D11ShaderResourceView* _src,
		ID3D11ShaderResourceView* _depth,
		XMFLOAT3 _light_pos,
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _lv, _lp, _src, _depth, _light_pos, _ambient, _diffuse ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool ShadowShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		XMMATRIX _lv,
		XMMATRIX _lp,
		ID3D11ShaderResourceView* _src,
		ID3D11ShaderResourceView* _depth,
		XMFLOAT3 _light_pos,
		XMFLOAT4 _ambient,
		XMFLOAT4 _diffuse
	)
	{
		{
			_w = XMMatrixTranspose( _w );
			_v = XMMatrixTranspose( _v );
			_p = XMMatrixTranspose( _p );
			_lv = XMMatrixTranspose( _lv );
			_lp = XMMatrixTranspose( _lp );

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				MatrixBufferType* data = (MatrixBufferType*)mappedResource.pData;
				data->w = _w;
				data->v = _v;
				data->p = _p;
				data->lv = _lv;
				data->lp = _lp;

				_context->Unmap( m_matrix_buffer, 0 );
			}

			UINT slot = 0;
			_context->VSSetConstantBuffers( slot, 1, &m_matrix_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_vs_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				VSLightBufferType* data = (VSLightBufferType*)mappedResource.pData;
				data->pos = _light_pos;
				data->padding = 0.0f;

				_context->Unmap( m_vs_light_buffer, 0 );
			}
			UINT slot = 1;
			_context->VSSetConstantBuffers( slot, 1, &m_vs_light_buffer );
		}

		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if ( SUCCEEDED( _context->Map( m_ps_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				PSLightBufferType* data = (PSLightBufferType*)mappedResource.pData;
				data->ambient = _ambient;
				data->diffuse = _diffuse;

				_context->Unmap( m_ps_light_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_ps_light_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_src );
		_context->PSSetShaderResources( 1, 1, &_depth );

		return true;
	}

	void ShadowShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_clamp_state );

		_context->PSSetSamplers( 1, 1, &m_wrap_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool ShadowShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool ShadowShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_vs_light_buffer, sizeof( VSLightBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_ps_light_buffer, sizeof( PSLightBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateClampSampler( _device, m_clamp_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateWrapSampler( _device, m_wrap_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void ShadowShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_wrap_state );
		SAFE_RELEASE( m_clamp_state );
		SAFE_RELEASE( m_ps_light_buffer );
		SAFE_RELEASE( m_vs_light_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

}