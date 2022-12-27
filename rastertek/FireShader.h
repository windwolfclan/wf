#pragma once

namespace wf
{
	class FireShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct NoiseBufferType
		{
			float frame_time;
			XMFLOAT3 scroll_speed{ 0.0f, 0.0f, 0.0f };
			XMFLOAT3 scales{ 1.0f, 1.0f, 1.0f };
			float padding;
		};

		struct DistortionBufferType
		{
			XMFLOAT2 distortion1;
			XMFLOAT2 distortion2;
			XMFLOAT2 distortion3;
			float distortion_scale;
			float distortion_bias;
		};

	public:
		FireShader();
		FireShader( const FireShader& );
		~FireShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _fire,
			ID3D11ShaderResourceView* _noise,
			ID3D11ShaderResourceView* _alpha,
			float _frame_time,
			XMFLOAT3 _scroll_speed,
			XMFLOAT3 _scales,
			XMFLOAT2 _distortion1,
			XMFLOAT2 _distortion2,
			XMFLOAT2 _distortion3,
			float _distortion_scale,
			float _distortion_bias
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _fire,
			ID3D11ShaderResourceView* _noise,
			ID3D11ShaderResourceView* _alpha,
			float _frame_time,
			XMFLOAT3 _scroll_speed,
			XMFLOAT3 _scales,
			XMFLOAT2 _distortion1,
			XMFLOAT2 _distortion2,
			XMFLOAT2 _distortion3,
			float _distortion_scale,
			float _distortion_bias
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_noise_buffer{ nullptr };
		ID3D11Buffer* m_distortion_buffer{ nullptr };
		
		ID3D11SamplerState* m_sampler_state1{ nullptr };
		ID3D11SamplerState* m_sampler_state2{ nullptr };
	};
}