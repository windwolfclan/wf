#pragma once

namespace wf
{
	class GlassShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct GlassBufferType
		{
			float refract_scale;
			XMFLOAT3 padding;
		};


	public:
		GlassShader();
		GlassShader( const GlassShader& );
		~GlassShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _color,
			ID3D11ShaderResourceView* _normal,
			ID3D11ShaderResourceView* _refract,
			float _refract_scale
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _color,
			ID3D11ShaderResourceView* _normal,
			ID3D11ShaderResourceView* _refract,
			float _refract_scale
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_glass_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}