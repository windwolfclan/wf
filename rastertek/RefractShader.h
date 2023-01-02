#pragma once

namespace wf
{
	class RefractShader
		: public ShaderBase
	{
	private:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

		struct LightBufferType
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT3 light_direction;
			float padding;
		};

		struct ClipPlaneBufferType
		{
			XMFLOAT4 clip_plane;
		};

	public:
		RefractShader();
		RefractShader( const RefractShader& );
		~RefractShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT3 _light_direction,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT4 _clip_plane
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView* _srv,
			XMFLOAT3 _light_direction,
			XMFLOAT4 _ambient,
			XMFLOAT4 _diffuse,
			XMFLOAT4 _clip_plane
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_light_buffer{ nullptr };
		ID3D11Buffer* m_clip_plane_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}