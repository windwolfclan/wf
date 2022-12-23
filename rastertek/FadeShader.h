#pragma once

namespace wf
{
	class FadeShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct FadeBuffer
		{
			float fade_amount{ 0.0f };
			float padding1;
			float padding2;
			float padding3;
		};

	public:
		FadeShader();
		FadeShader( const FadeShader& );
		~FadeShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			float _fade_amount
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			float _fade_amount
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_fade_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}