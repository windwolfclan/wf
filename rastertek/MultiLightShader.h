#pragma once

namespace wf
{
	constexpr int LIGHT_COUNT = 4;

	class MultiLightShader
		: public ShaderBase
	{
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

	public:
		struct LightPositionBuffer
		{
			XMFLOAT4 position[ LIGHT_COUNT ];
		};

		struct LightColorBuffer
		{
			XMFLOAT4 diffuse[ LIGHT_COUNT ];
		};

	public:
		MultiLightShader();
		MultiLightShader( const MultiLightShader& );
		~MultiLightShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			LightPositionBuffer& _position,
			LightColorBuffer& _diffuse
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			LightPositionBuffer& _position,
			LightColorBuffer& _diffuse
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_light_position_buffer{ nullptr };
		ID3D11Buffer* m_light_color_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}