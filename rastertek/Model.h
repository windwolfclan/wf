#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

namespace wf
{
	class Model
	{
	private:
		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

	public:
		Model();
		Model( const Model& );
		~Model();

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
}