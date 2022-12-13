#include "pch.h"
#include "ShaderBase.h"

namespace wf
{
	bool ShaderBase::Initialize( ID3D11Device* _device, HWND _hwnd )
	{
		if( !InitializeShader( _device, _hwnd ) )
		{
			return false;
		}

		return true;
	}

	void ShaderBase::Shutdown()
	{
		ShutdownShader();
	}

	bool ShaderBase::InitializeShader( ID3D11Device* _device, HWND _hwnd )
	{
		HRESULT hr{ S_OK };
		ID3D10Blob* error{ nullptr };

		ID3D10Blob* vs_buffer{ nullptr };
		hr = D3DCompileFromFile( m_vs.c_str(), nullptr, nullptr, m_vs_main.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vs_buffer, &error);
		if ( FAILED( hr ) )
		{
			if ( error )
			{
				OutputShaderErrorMessage( error, _hwnd, m_vs.c_str() );
			}
			else
			{
				MessageBox( _hwnd, m_vs.c_str(), L"Missing Shader File", MB_OK );
			}

			return false;
		}

		if ( !InitializeLayout( _device, vs_buffer ) )
		{
			SAFE_RELEASE( vs_buffer );
			return false;
		}

		hr = _device->CreateVertexShader( vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), nullptr, &m_vertex_shader );
		if ( FAILED( hr ) )
		{
			SAFE_RELEASE( vs_buffer );
			return false;
		}

		SAFE_RELEASE( vs_buffer );


		ID3D10Blob* ps_buffer{ nullptr };
		hr = D3DCompileFromFile( m_ps.c_str(), nullptr, nullptr, m_ps_main.c_str(), "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &ps_buffer, &error);
		if ( FAILED( hr ) )
		{
			if ( error )
			{
				OutputShaderErrorMessage( error, _hwnd, m_ps.c_str() );
			}
			else
			{
				MessageBox( _hwnd, m_ps.c_str(), L"Missing Shader File", MB_OK );
			}

			return false;
		}

		hr = _device->CreatePixelShader( ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), nullptr, &m_pixel_shader );
		if ( FAILED( hr ) )
		{
			SAFE_RELEASE( ps_buffer );
			return false;
		}

		SAFE_RELEASE( ps_buffer );

		if ( !InitializeBuffers( _device ) )
		{
			return false;
		}

		return true;
	}

	void ShaderBase::ShutdownShader()
	{
		ReleaseBuffers();

		SAFE_RELEASE( m_pixel_shader );
		SAFE_RELEASE( m_vertex_shader );
		SAFE_RELEASE( m_layout );
	}

	void ShaderBase::OutputShaderErrorMessage( ID3D10Blob* _blob, HWND _hwnd, const wchar_t* _filename )
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

}