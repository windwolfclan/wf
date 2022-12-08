#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Texture.h"

namespace wf
{
	class ColorModel
	{
	private:
		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

	public:
		ColorModel();
		ColorModel( const ColorModel& );
		~ColorModel();

		bool Initialize( ID3D11Device* _device );
		void Shutdown();
		void Render( ID3D11DeviceContext* _context );

		int GetIndexCount() const;

	private:
		bool InitializeBuffers( ID3D11Device* _device );
		void ShutdownBuffers();
		void RenderBuffers( ID3D11DeviceContext* _context );

	private:
		ID3D11Buffer* m_vertex_buffer{ nullptr };
		ID3D11Buffer* m_index_buffer{ nullptr };
		int m_vertex_count{ 0 };
		int m_index_count{ 0 };
	};

	class TextureModel
	{
	private:
		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT2 tex{ 0.0f, 0.0f };
		};

	public:
		TextureModel();
		TextureModel( const TextureModel& );
		~TextureModel();

		bool Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path );
		void Shutdown();
		void Render( ID3D11DeviceContext* _context );

		int GetIndexCount() const;
		ID3D11ShaderResourceView* GetTexture();

	private:
		bool InitializeBuffers( ID3D11Device* _device );
		void ShutdownBuffers();
		void RenderBuffers( ID3D11DeviceContext* _context );

		bool LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path );
		void ReleaseTexture();

	private:
		ID3D11Buffer* m_vertex_buffer{ nullptr };
		ID3D11Buffer* m_index_buffer{ nullptr };
		int m_vertex_count{ 0 };
		int m_index_count{ 0 };

		Texture* m_texture{ nullptr };
	};
}