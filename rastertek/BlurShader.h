#pragma once

namespace wf
{
	class BlurShader
		: public ShaderBase
	{
	protected:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct BlurBufferType
		{
			XMFLOAT2 resolution;
			float weight_count;
			float padding1;
			XMFLOAT4 weights1;
			XMFLOAT4 weights2;
			XMFLOAT4 weights3;
			XMFLOAT4 weights4;
		};

	public:
		BlurShader();
		BlurShader( const BlurShader& );
		virtual ~BlurShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _texture,
			XMFLOAT2 _resolution,
			float _weight_count,
			std::vector<float>& _weights
		);

	protected:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _texture,
			XMFLOAT2 _resolution,
			float _weight_count,
			std::vector<float>& _weights
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	protected:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_blur_buffer{ nullptr };

		ID3D11SamplerState* m_sampler_state{ nullptr };
	};

	class HorizontalBlurShader
		: public BlurShader
	{
	public:
		HorizontalBlurShader();
		HorizontalBlurShader( const HorizontalBlurShader& );
		virtual ~HorizontalBlurShader();
	};

	class VerticalBlurShader
		: public BlurShader
	{
	public:
		VerticalBlurShader();
		VerticalBlurShader( const VerticalBlurShader& );
		virtual ~VerticalBlurShader();
	};
}