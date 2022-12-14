#include "pch.h"
#include "TextureShader.h"

namespace wf
{
	TextureShader::TextureShader()
	{
		m_vs = L"texture_vs.hlsl";
		m_ps = L"texture_ps.hlsl";
		m_vs_main = "TextureVertexShader";
		m_ps_main = "TexturePixelShader";
	}

	TextureShader::TextureShader( const TextureShader& )
	{
	}

	TextureShader::~TextureShader()
	{
	}

	bool TextureShader::Render( 
		ID3D11DeviceContext* _context, 
		int _index_count, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _srv 
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool TextureShader::SetShaderParameters(
		ID3D11DeviceContext* _context, 
		XMMATRIX _w, 
		XMMATRIX _v, 
		XMMATRIX _p, 
		ID3D11ShaderResourceView* _srv 
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
		_context->PSSetShaderResources( 0, 1, &_srv );

		return true;
	}

	void TextureShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool TextureShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool TextureShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
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

	void TextureShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_matrix_buffer );
	}

	InstanceTextureShader::InstanceTextureShader()
	{
		m_vs = L"texture_vs.hlsl";
		m_ps = L"texture_ps.hlsl";
		m_vs_main = "instance_texture_vs";
		m_ps_main = "TexturePixelShader";
	}

	bool InstanceTextureShader::Render( ID3D11DeviceContext* _context, int _vertex_count, int _instance_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv ) )
		{
			return false;
		}

		RenderShader( _context, _vertex_count, _instance_count );

		return true;
	}

	void InstanceTextureShader::RenderShader( ID3D11DeviceContext* _context, int _vertex_count, int _instance_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawInstanced( _vertex_count, _instance_count, 0, 0 );
	}

	bool InstanceTextureShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
	{
		D3D11_INPUT_ELEMENT_DESC layout_desc[ 3 ] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		HRESULT hr = _device->CreateInputLayout( layout_desc, NumElements, _blob->GetBufferPointer(), _blob->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

}