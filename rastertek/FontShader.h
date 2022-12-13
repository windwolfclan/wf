#pragma once

namespace wf
{
	class FontShader
		: public ShaderBase
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
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _pixel_color
		);
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_pixel_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}