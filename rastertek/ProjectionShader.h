#pragma once

namespace wf
{
	class ProjectionShader
		: public ShaderBase
	{
	public:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
			XMMATRIX v2;
			XMMATRIX p2;
		};

		struct LightBufferType
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT3 direction;
			float padding;
		};

	public:
		ProjectionShader();
		ProjectionShader( const ProjectionShader& );
		~ProjectionShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT3 _direction,
			XMMATRIX _v2,
			XMMATRIX _p2,
			ID3D11ShaderResourceView* _projection
		);

	protected:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT3 _direction,
			XMMATRIX _v2,
			XMMATRIX _p2,
			ID3D11ShaderResourceView* _projection
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	protected:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_light_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};

	class LightProjectionShader
		: public ProjectionShader
	{
		struct LightPositionBuffer
		{
			XMFLOAT3 position{};
			float padding;
		};

	public:
		LightProjectionShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT3 _light_pos,
			XMMATRIX _v2,
			XMMATRIX _p2,
			ID3D11ShaderResourceView* _projection
		);

	protected:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT3 _light_pos,
			XMMATRIX _v2,
			XMMATRIX _p2,
			ID3D11ShaderResourceView* _projection
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	protected:
		ID3D11Buffer* m_light_position_buffer{ nullptr };
	};
}