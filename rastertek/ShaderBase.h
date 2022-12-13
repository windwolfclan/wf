#pragma once

namespace wf
{
	struct ShaderBase
	{
		ShaderBase() = default;
		virtual ~ShaderBase() = default;

		bool Initialize( ID3D11Device* _device, HWND _hwnd );
		void Shutdown();

	protected:
		bool InitializeShader( ID3D11Device* _device, HWND _hwnd );
		void ShutdownShader();

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) = 0;
		virtual bool InitializeBuffers( ID3D11Device* _device ) = 0;
		virtual void ReleaseBuffers() = 0;
		
		void OutputShaderErrorMessage( ID3D10Blob* _blob, HWND _hwnd, const wchar_t* _filename );

		std::wstring m_vs{};
		std::wstring m_ps{};
		std::string m_vs_main{};
		std::string m_ps_main{};

		ID3D11InputLayout* m_layout{ nullptr };
		ID3D11VertexShader* m_vertex_shader{ nullptr };
		ID3D11PixelShader* m_pixel_shader{ nullptr };
	};
}