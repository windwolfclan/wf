#pragma once

namespace wf
{
	class TranslateShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct TranslateBuffer
		{
			float translate_x{ 0.0f };
			float translate_y{ 0.0f };
			float padding1;
			float padding2;
		};

	public:
		TranslateShader();
		TranslateShader( const TranslateShader& );
		~TranslateShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			float _translate_x,
			float _translate_y
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			float _translate_x,
			float _translate_y
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_translate_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}