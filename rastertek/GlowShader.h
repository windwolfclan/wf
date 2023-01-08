#pragma once

namespace wf
{
	class GlowShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct GlowBufferType
		{
			float glow_strength;
			XMFLOAT3 padding;
		};

	public:
		GlowShader();
		GlowShader( const GlowShader& );
		~GlowShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _src,
			ID3D11ShaderResourceView* _glow,
			float _glow_strength
		);

	protected:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _src,
			ID3D11ShaderResourceView* _glow,
			float _glow_strength
		);
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	protected:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_glow_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};


}