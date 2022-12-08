#include "pch.h"
#include "TextureShader.h"

namespace wf
{
	TextureShader::TextureShader()
	{
	}

	TextureShader::TextureShader( const TextureShader& )
	{
	}

	TextureShader::~TextureShader()
	{
	}

	bool TextureShader::Initialize( ID3D11Device* _device, HWND _hwnd )
	{
		std::wstring vs = L"texture_vs.hlsl";
		std::wstring ps = L"texture_ps.hlsl";

		HRESULT hr = InitializeShader( _device, _hwnd, vs.c_str(), ps.c_str() );
		if ( FAILED( hr ) )
		{
			return false;
		}


		return true;
	}

	void TextureShader::Shutdown()
	{
		ShutdownShader();
	}

	bool TextureShader::Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv )
	{
		if ( !SetShaderParameters( _context, _w, _v, _p, _srv ) )
		{
			return false;
		}

		RenderShader( _context, _index_count );

		return true;
	}

	bool TextureShader::InitializeShader( ID3D11Device* _device, HWND _hwnd, const wchar_t* _vs, const wchar_t* _ps )
	{
		HRESULT hr{ S_OK };
		ID3D10Blob* error{ nullptr };


		ID3D10Blob* vs_buffer{ nullptr };
		hr = D3DCompileFromFile( _vs, nullptr, nullptr, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vs_buffer, &error );
		if ( FAILED( hr ) )
		{
			if ( error )
			{
				OutputShaderErrorMessage( error, _hwnd, _vs );
			}
			else
			{
				MessageBox( _hwnd, _vs, L"Missing Shader File", MB_OK );
			}

			return false;
		}

		ID3D10Blob* ps_buffer{ nullptr };
		hr = D3DCompileFromFile( _ps, nullptr, nullptr, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &ps_buffer, &error );
		if ( FAILED( hr ) )
		{
			if ( error )
			{
				OutputShaderErrorMessage( error, _hwnd, _ps );
			}
			else
			{
				MessageBox( _hwnd, _ps, L"Missing Shader File", MB_OK );
			}

			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout_desc[ 2 ] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT NumElements = sizeof( layout_desc ) / sizeof( layout_desc[ 0 ] );

		hr = _device->CreateInputLayout( layout_desc, NumElements, vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), &m_layout );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = _device->CreateVertexShader( vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), nullptr, &m_vertex_shader );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = _device->CreatePixelShader( ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), nullptr, &m_pixel_shader );
		if ( FAILED( hr ) )
		{
			return false;
		}

		ps_buffer->Release();
		ps_buffer = nullptr;

		vs_buffer->Release();
		vs_buffer = nullptr;

		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( MatrixBufferType );
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer( &buffer_desc, nullptr, &m_matrix_buffer );
		if ( FAILED( hr ) )
		{
			return false;
		}

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

		hr = _device->CreateSamplerState( &sampler_desc, &m_sampler_state );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void TextureShader::ShutdownShader()
	{
		if ( m_sampler_state )
		{
			m_sampler_state->Release();
			m_sampler_state = nullptr;
		}

		if ( m_matrix_buffer )
		{
			m_matrix_buffer->Release();
			m_matrix_buffer = nullptr;
		}

		if ( m_pixel_shader )
		{
			m_pixel_shader->Release();
			m_pixel_shader = nullptr;
		}

		if ( m_vertex_shader )
		{
			m_vertex_shader->Release();
			m_vertex_shader = nullptr;
		}

		if ( m_layout )
		{
			m_layout->Release();
			m_layout = nullptr;
		}
	}

	void TextureShader::OutputShaderErrorMessage( ID3D10Blob* _blob, HWND _hwnd, const wchar_t* _filename )
	{
		char* compile_error = (char*)_blob->GetBufferPointer();
		unsigned long long buffer_size = _blob->GetBufferSize();

		std::ofstream out( "shader-error.txt" );
		if ( !out.is_open() )
		{
			return;
		}

		for ( unsigned long long i = 0; i < buffer_size; ++i )
		{
			out << compile_error[ i ];
		}

		out.close();

		_blob->Release();
		_blob = nullptr;

		MessageBox( _hwnd, L"Error compiling shader\nCheck shader-error.txt", _filename, MB_OK );
	}

	bool TextureShader::SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv )
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
}