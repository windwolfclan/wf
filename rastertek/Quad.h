#pragma once

namespace wf
{
	class Quad
	{
	public:
		Quad();
		Quad( const Quad& );
		virtual ~Quad();

		bool Initialize( ID3D11Device* _device, int _quad_width, int _quad_height );
		void Shutdown();
		bool Render( ID3D11DeviceContext* _context, int _x, int _y );

		int GetIndexCount() const;

	protected:
		virtual bool InitializeBuffers( ID3D11Device* _device ) = 0;
		virtual void UpdatePosition( int _x, int _y ) = 0;
		virtual bool UpdateGpuData( ID3D11DeviceContext* _context ) = 0;

		void ShutdownBuffers();
		bool UpdateBuffers( ID3D11DeviceContext* _context, int _x, int _y );
		void RenderBuffers( ID3D11DeviceContext* _context );

	protected:
		ID3D11Buffer* m_vertex_buffer{ nullptr };
		ID3D11Buffer* m_index_buffer{ nullptr };
		int m_vertex_count{ 0 };
		int m_index_count{ 0 };

		int m_quad_width{ 0 };
		int m_quad_height{ 0 };
		int m_previous_x{ INT_MIN };
		int m_previous_y{ INT_MIN };

		UINT m_stride{ 0 };

	public:
		static void SetScreenSize( int _width, int _height );
		static Quad* CreateQuad( quad_type _quad_type );

	protected:
		static int s_screen_width;
		static int s_screen_height;
	};

	class TextureQuad
		: public Quad
	{
		using VertexType = PositionTextureVertex;
	public:
		TextureQuad() = default;
		TextureQuad( const TextureQuad& ) = default;
		virtual ~TextureQuad() = default;

	protected:
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void UpdatePosition( int _x, int _y ) override;
		virtual bool UpdateGpuData( ID3D11DeviceContext* _context ) override;

	protected:
		std::array<VertexType, 6> m_vertices{};
	};

	class TangentSpaceQuad
		: public Quad
	{
		using VertexType = PositionTextureNormalTangentBinormalVertex;
	public:
		TangentSpaceQuad() = default;
		TangentSpaceQuad( const TangentSpaceQuad& ) = default;
		virtual ~TangentSpaceQuad() = default;

	protected:
		virtual bool InitializeBuffers( ID3D11Device* _device ) override;
		virtual void UpdatePosition( int _x, int _y ) override;
		virtual bool UpdateGpuData( ID3D11DeviceContext* _context ) override;
		
		void CalculateTangentBinormal( const VertexType& v1, const VertexType& v2, const VertexType& v3, XMFLOAT3& tangent, XMFLOAT3& binormal );
		void CalculateNormal( const XMFLOAT3& tangent, const XMFLOAT3& binormal, XMFLOAT3& normal );

	protected:
		std::array<VertexType, 6> m_vertices{};
	};
}