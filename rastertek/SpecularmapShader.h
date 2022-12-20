#pragma once

namespace wf
{
	class SpecularShader
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
			XMFLOAT4 diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
			XMFLOAT4 specular{ 0.0f, 0.0f, 0.0f, 0.0f };
			float specular_power{ 1.0f };
			XMFLOAT3 light_dir{ 0.0f, 0.0f, 0.0f };
			
		};

		struct CameraBufferType
		{
			XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
			float padding{ 0.0f };
		};

	public:
		SpecularShader();
		SpecularShader( const SpecularShader& );
		~SpecularShader();

		bool Render(
			ID3D11DeviceContext* _context,
			int _index_count,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView** _srv,
			XMFLOAT3 _camera_position,
			XMFLOAT4 _diffuse,
			XMFLOAT4 _specular,
			float _specular_power,
			XMFLOAT3 _light_dir
		);

	private:
		bool SetShaderParameters(
			ID3D11DeviceContext* _context,
			XMMATRIX _w,
			XMMATRIX _v,
			XMMATRIX _p,
			ID3D11ShaderResourceView** _srv,
			XMFLOAT3 _camera_position,
			XMFLOAT4 _diffuse,
			XMFLOAT4 _specular,
			float _specular_power,
			XMFLOAT3 _light_dir
		);

		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	private:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11Buffer* m_light_buffer{ nullptr };
		ID3D11Buffer* m_camera_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};
}