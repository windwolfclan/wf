#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;

namespace wf
{
	class TextureShader
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

	public:
		TextureShader();
		TextureShader( const TextureShader& );
		~TextureShader();

		bool Initialize( ID3D11Device* _device, HWND _hwnd );
		void Shutdown();
		bool Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv );

	private:
		bool InitializeShader( ID3D11Device* _device, HWND _hwnd, const wchar_t* _vs, const wchar_t* _ps );
		void ShutdownShader();
		void OutputShaderErrorMessage( ID3D10Blob* _blob, HWND _hwnd, const wchar_t* _filename );

		bool SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv );
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

	private:
		ID3D11InputLayout* m_layout{ nullptr };
		ID3D11VertexShader* m_vertex_shader{ nullptr };
		ID3D11PixelShader* m_pixel_shader{ nullptr };
		ID3D11Buffer* m_matrix_buffer{ nullptr };

		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}