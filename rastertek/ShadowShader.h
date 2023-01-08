#pragma once

namespace wf
{
	class ShadowShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
			XMMATRIX lv;
			XMMATRIX lp;
		};

		struct VSLightBufferType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			float padding{ 0.0f };
		};

		struct PSLightBufferType
		{
			XMFLOAT4 ambient{ 1.0f, 1.0f, 1.0f,1.0f };
			XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

	public:
		ShadowShader();
		ShadowShader( const ShadowShader& );
		~ShadowShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			XMMATRIX _lv,
			XMMATRIX _lp,
			ID3D11ShaderResourceView* _src,
			ID3D11ShaderResourceView* _depth,
			XMFLOAT3 _light_pos,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			XMMATRIX _lv,
			XMMATRIX _lp,
			ID3D11ShaderResourceView* _src,
			ID3D11ShaderResourceView* _depth,
			XMFLOAT3 _light_pos,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse
		);
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_ps_light_buffer{ nullptr };
		ID3D11Buffer* m_vs_light_buffer{ nullptr };
		ID3D11SamplerState* m_clamp_state{ nullptr };
		ID3D11SamplerState* m_wrap_state{ nullptr };
	};
}