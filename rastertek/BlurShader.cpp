#include "pch.h"
#include "BlurShader.h"

namespace wf
{
	BlurShader::BlurShader()
	{
	}

	BlurShader::BlurShader( const BlurShader& )
	{
	}

	BlurShader::~BlurShader()
	{
	}

	bool BlurShader::Render(
		ID3D11DeviceContext* _context,
		int _index_count,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _texture,
		XMFLOAT2 _resolution,
		float _weight_count,
		std::vector<float>& _weights
	)
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _texture, _resolution, _weight_count, _weights) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool BlurShader::SetShaderParameters(
		ID3D11DeviceContext* _context,
		XMMATRIX _w,
		XMMATRIX _v,
		XMMATRIX _p,
		ID3D11ShaderResourceView* _texture,
		XMFLOAT2 _resolution,
		float _weight_count,
		std::vector<float>& _weights
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
			if ( SUCCEEDED( _context->Map( m_blur_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
			{
				BlurBufferType* data = (BlurBufferType*)mappedResource.pData;
				data->resolution = _resolution;
				data->weight_count = _weight_count;

				data->weights1 = XMFLOAT4( _weights[ 0 ], _weights[ 1 ], _weights[ 2 ], _weights[ 3 ] );
				data->weights2 = XMFLOAT4( _weights[ 4 ], _weights[ 5 ], _weights[ 6 ], _weights[ 7 ] );
				data->weights3 = XMFLOAT4( _weights[ 8 ], _weights[ 9 ], _weights[ 10 ], _weights[ 11 ] );
				data->weights4 = XMFLOAT4( _weights[ 12 ], _weights[ 13 ], _weights[ 14 ], 0.0f );

				_context->Unmap( m_blur_buffer, 0 );
			}

			UINT slot = 0;
			_context->PSSetConstantBuffers( slot, 1, &m_blur_buffer );
		}

		_context->PSSetShaderResources( 0, 1, &_texture );

		return true;
	}

	void BlurShader::RenderShader( ID3D11DeviceContext* _context, int _index_count )
	{
		_context->IASetInputLayout( m_layout );

		_context->VSSetShader( m_vertex_shader, nullptr, 0 );

		_context->PSSetShader( m_pixel_shader, nullptr, 0 );

		_context->PSSetSamplers( 0, 1, &m_sampler_state );

		_context->DrawIndexed( _index_count, 0, 0 );
	}

	bool BlurShader::InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob )
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

	bool BlurShader::InitializeBuffers( ID3D11Device* _device )
	{
		HRESULT hr{ S_OK };

		hr = utility::CreateDynamicWriteBuffer( _device, m_matrix_buffer, sizeof( MatrixBufferType ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = utility::CreateDynamicWriteBuffer( _device, m_blur_buffer, sizeof( BlurBufferType ) );
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

	void BlurShader::ReleaseBuffers()
	{
		SAFE_RELEASE( m_sampler_state );
		SAFE_RELEASE( m_blur_buffer );
		SAFE_RELEASE( m_matrix_buffer );
	}

	HorizontalBlurShader::HorizontalBlurShader()
	{
		m_vs = L"blur_vs.hlsl";
		m_ps = L"blur_ps.hlsl";
		m_vs_main = "blur_vs";
		m_ps_main = "horizontal_blur";
	}

	HorizontalBlurShader::HorizontalBlurShader( const HorizontalBlurShader& )
	{
	}

	HorizontalBlurShader::~HorizontalBlurShader()
	{
	}

	VerticalBlurShader::VerticalBlurShader()
	{
		m_vs = L"blur_vs.hlsl";
		m_ps = L"blur_ps.hlsl";
		m_vs_main = "blur_vs";
		m_ps_main = "vertical_blur";
	}

	VerticalBlurShader::VerticalBlurShader( const VerticalBlurShader& )
	{
	}

	VerticalBlurShader::~VerticalBlurShader()
	{
	}

}