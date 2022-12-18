#pragma once

namespace wf
{
	struct PositionTextureVertex
	{
		XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
		XMFLOAT2 tex{ 0.0f, 0.0f };
	};

	struct PositionTextureNormalVertex
	{
		XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
		XMFLOAT2 tex{ 0.0f, 0.0f };
		XMFLOAT3 normal{ 0.0f, 0.0f, 0.0f };
	};

	using RastertekVertex = PositionTextureNormalVertex;

	struct PositionTextureNormalTangentBinormalVertex
	{
		XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
		XMFLOAT2 tex{ 0.0f, 0.0f };
		XMFLOAT3 normal{ 0.0f, 0.0f, 0.0f };
		XMFLOAT3 tangent{ 0.0f, 0.0f, 0.0f };
		XMFLOAT3 binormal{ 0.0f, 0.0f, 0.0f };
	};

	using TangentSpaceVertex = PositionTextureNormalTangentBinormalVertex;

	struct RastertekVertexData
	{
		float x{ 0.0f };
		float y{ 0.0f };
		float z{ 0.0f };
		float u{ 0.0f };
		float v{ 0.0f };
		float nx{ 0.0f };
		float ny{ 0.0f };
		float nz{ 0.0f };
	};


	struct ModelData
	{
		ID3D11Buffer* vertex_buffer{ nullptr };
		ID3D11Buffer* index_buffer{ nullptr };
		int vertex_count{ 0 };
		int index_count{ 0 };
		UINT stride{ 0 };
	};

	struct ModelParam
	{
		ID3D11Device* device{ nullptr };
		std::wstring path{};
		std::wstring key{};
	};

	class ModelLoader
	{
	public:
		ModelLoader();
		ModelLoader( const ModelLoader& );
		~ModelLoader();

		void Shutdown();

		bool LoadRastertekModel( const ModelParam& _param );
		bool LoadTangentSpaceModel( const ModelParam& _param );

		ModelData* GetModelData( std::wstring _key );

	private:
		bool LoadData( const ModelParam& _param, RastertekVertexData*& _data, ModelData*& _model_data );
		bool InitializeRastertekModelBuffers( const ModelParam& _param, const RastertekVertexData* _data, ModelData*& _model_data );
		bool InitializeTangentSpaceModelBuffers( const ModelParam& _param, TangentSpaceVertex*& _vertices, ModelData*& _model_data );

		void CalculateVectors( const RastertekVertexData* _data, TangentSpaceVertex*& _vertices, ModelData*& _model_data );
		void CalculateTangentBinormal( const RastertekVertexData& v1, const RastertekVertexData& v2, const RastertekVertexData& v3, XMFLOAT3& tangent, XMFLOAT3& binormal );
		void CalculateNormal( const XMFLOAT3& tangent, const XMFLOAT3& binormal, XMFLOAT3& normal );

	private:
		std::unordered_map< std::wstring, ModelData* > m_model_datas{};
	};

	void RenderModelData( ID3D11DeviceContext* _context, ModelData* _model_data );
}