#include "pch.h"
#include "GlowmapShader.h"

namespace wf
{
	GlowmapShader::GlowmapShader()
	{
		m_vs = L"glowmap_vs.hlsl";
		m_ps = L"glowmap_ps.hlsl";
		m_vs_main = "glowmap_vs";
		m_ps_main = "glowmap_ps";
	}

	GlowmapShader::GlowmapShader( const GlowmapShader& )
	{
	}

	GlowmapShader::~GlowmapShader()
	{
	}

	bool GlowmapShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _src,
		ID3D11ShaderResourceView* _glowmap
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _src, _glowmap ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool GlowmapShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _src, 
		ID3D11ShaderResourceView* _glowmap
	)
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
		
		_context->PSSetShaderResources( 0, 1, &_src );
		_context->PSSetShaderResources( 1, 1, &_glowmap );

		return true;
	}

	void GlowmapShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool GlowmapShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool GlowmapShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateClampSampler( _device, m_sampler_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void GlowmapShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_matrix_buffer );
	}

}