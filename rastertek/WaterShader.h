#pragma once

namespace wf
{
	class WaterShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct ReflectBufferType
		{
			XMMATRIX r;
		};

		struct WaterBufferType
		{
			float water_translation;
			float reflect_refract_scale;
			XMFLOAT2 padding;
		};

	public:
		WaterShader();
		WaterShader( const WaterShader& );
		~WaterShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			XMMATRIX _r,
			ID3D11ShaderResourceView* _reflect,
			ID3D11ShaderResourceView* _refract,
			ID3D11ShaderResourceView* _normal,
			float _water_translation,
			float _reflect_refract_scale
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			XMMATRIX _r,
			ID3D11ShaderResourceView* _reflect,
			ID3D11ShaderResourceView* _refract,
			ID3D11ShaderResourceView* _normal,
			float _water_translation,
			float _reflect_refract_scale
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_reflect_buffer{ nullptr };
		ID3D11Buffer* m_water_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}