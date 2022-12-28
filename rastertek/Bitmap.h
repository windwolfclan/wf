#pragma once

#include "Texture.h"

namespace wf
{
	class Bitmap
	{
		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT2 tex{ 0.0f, 0.0f };
		};

	public:
		Bitmap();
		Bitmap( const Bitmap& );
		~Bitmap();

		bool Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, int _screen_width, int _screen_height, const char* _path, int _bitmap_width, int _bitmap_height );
		void Shutdown();
		bool Render( ID3D11DeviceContext* _context, int _x, int _y );

		int GetIndexCount();
		ID3D11ShaderResourceView* GetTexture() const;

	private:
		bool InitializeBuffers( ID3D11Device* _device );
		void ShutdownBuffers();

		bool UpdateBuffers( ID3D11DeviceContext* _context, int _x, int _y );
		void RenderBuffers( ID3D11DeviceContext* _context );

		bool LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path );
		void ReleaseTexture();

	private:
		ID3D11Buffer* m_vertex_buffer{ nullptr };
		ID3D11Buffer* m_index_buffer{ nullptr };
		int m_vertex_count{ 0 };
		int m_index_count{ 0 };

		Texture* m_texture{ nullptr };

		int m_screen_width{ 0 };
		int m_screen_height{ 0 };
		int m_bitmap_width{ 0 };
		int m_bitmap_height{ 0 };
		int m_previous_x{ INT_MIN };
		int m_previous_y{ INT_MIN };

		std::array<VertexType, 6> m_vertices{};
	};

	class RenderTargetBitmap
	{
		using VertexType = PositionTextureVertex;
	public:
		RenderTargetBitmap();
		RenderTargetBitmap( const RenderTargetBitmap& );
		~RenderTargetBitmap();

		bool Initialize( ID3D11Device* _device, int _width, int _height );
		void Shutdown();
		void Render( ID3D11DeviceContext* _context );

		int GetIndexCount();

	private:
		bool InitializeBuffers( ID3D11Device* _device , int _width, int _height );
		void ShutdownBuffers();
		void RenderBuffers( ID3D11DeviceContext* _context );

	private:
		ID3D11Buffer* m_vertex_buffer{ nullptr };
		ID3D11Buffer* m_index_buffer{ nullptr };
		int m_vertex_count{ 0 };
		int m_index_count{ 0 };
	};
}