#pragma once

namespace wf
{
	class TextureShader
		: public ShaderBase
	{
	protected:
		struct MatrixBufferType
		{
			XMMATRIX w;
			XMMATRIX v;
			XMMATRIX p;
		};

	public:
		TextureShader();
		TextureShader( const TextureShader& );
		~TextureShader();

		bool Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv );

	protected:
		bool SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv );
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

	protected:
		ID3D11Buffer* m_matrix_buffer{ nullptr };
		ID3D11SamplerState* m_sampler_state{ nullptr };
	};

	struct SliceBuffer
	{
		XMFLOAT4 resolution;
		XMFLOAT4 border;
	};

	class SliceShader
		: public TextureShader
	{
	public:
		SliceShader();

		bool Render( ID3D11DeviceContext* _context, int _index_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, SliceBuffer& _slice_buffer );

	protected:
		bool SetShaderParameters( ID3D11DeviceContext* _context, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv, SliceBuffer& _slice_buffer );
		void RenderShader( ID3D11DeviceContext* _context, int _index_count );
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void ReleaseBuffers() override;

		ID3D11Buffer* m_slice_buffer{ nullptr };

	};

	class InstanceTextureShader
		: public TextureShader
	{
	public:
		InstanceTextureShader();

		bool Render( ID3D11DeviceContext* _context, int _vertex_count, int _instance_count, XMMATRIX _w, XMMATRIX _v, XMMATRIX _p, ID3D11ShaderResourceView* _srv );

	private:
		void RenderShader( ID3D11DeviceContext* _context, int _vertex_count, int _instance_count );

		virtual bool InitializeLayout( ID3D11Device* _device, ID3D10Blob*& _blob ) override;
	};


}