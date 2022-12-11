#pragma once

namespace wf
{
	class FontShader
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct PixelBufferType
		{
			XMFLOAT4 pixel_color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

	public:
		FontShader();
		FontShader( const FontShader& );
		~FontShader();

		bool Initialize( ID3D11Device* _device, HWND _hwnd );
		void Shutdown();
		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _pixel_color
		);

	private:
		bool InitializeShader( ID3D11Device* _device, HWND _hwnd, const wchar_t* _vs, const wchar_t* _ps );
		void ShutdownShader();
		void OutputShaderErrorMessage( ID3D10Blob* _blob, HWND _hwnd, const wchar_t* _filename );

		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _pixel_color
		);
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

	private:
		ID3D11InputLayout* m_layout{ nullptr };
		ID3D11VertexShader* m_vertex_shader{ nullptr };
		ID3D11PixelShader* m_pixel_shader{ nullptr };
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_pixel_buffer{ nullptr };

		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}