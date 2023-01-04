#include "pch.h"
#include "GlassShader.h"

namespace wf
{
	GlassShader::GlassShader()
	{
		m_vs = L"glass_vs.hlsl";
		m_ps = L"glass_ps.hlsl";
		m_vs_main = "glass_vs";
		m_ps_main = "glass_ps";
	}

	GlassShader::GlassShader( const GlassShader& )
	{
	}

	GlassShader::~GlassShader()
	{
	}

	bool GlassShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _color,
		ID3D11ShaderResourceView* _normal,
		ID3D11ShaderResourceView* _refract,
		float _refract_scale
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _color, _normal, _refract, _refract_scale ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool GlassShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _color,
		ID3D11ShaderResourceView* _normal,
		ID3D11ShaderResourceView* _refract,
		float _refract_scale
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
			if ( SUCCEEDED( _context->Map( m_glass_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				GlassBufferType* data = (GlassBufferType*)mappedResource.pData;
				data->refract_scale = _refract_scale;

				_context->Unmap( m_glass_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_glass_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_color );
		_context->PSSetShaderResources( 1, 1, &_normal );
		_context->PSSetShaderResources( 2, 1, &_refract );

		return true;
	}

	void GlassShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
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

	bool GlassShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool GlassShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_glass_buffer, sizeof( GlassBufferType ) );
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

	void GlassShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_glass_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}
}