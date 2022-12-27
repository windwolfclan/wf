#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "winmm.lib" )

#include <Windows.h>
#include <mmsystem.h>
#include <fstream>
#include <array>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dsound.h>

using namespace DirectX;

#include "ShaderBase.h"
#include "Utility.h"
#include "DXTK/DDSTextureLoader.h"

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

	struct TargaHeader
	{
		unsigned char data1[ 12 ]{ 0, };
		unsigned short width{ 0 };
		unsigned short height{ 0 };
		unsigned char bpp{ 0 };
		unsigned char data2{ 0 };
	};

	bool LoadTarga( const wchar_t* _path, int& _width, int& _height, unsigned char*& _buffer );

	enum class quad_type
	{
		none = 0,
		texture,
		tangent_space,
	};
}

#define SAFE_SHUTDOWN( x ) if( x ) { x->Shutdown(); delete x; x = nullptr; }
#define SAFE_RELEASE( x ) if( x ) { x->Release(); x = nullptr; }
#define SAFE_DELETE( x ) if( x ) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY( x ) if( x ) { delete[] x; x = nullptr; }