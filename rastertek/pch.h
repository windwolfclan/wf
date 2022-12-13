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

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dsound.h>

using namespace DirectX;

#include "ShaderBase.h"

#define SAFE_SHUTDOWN( x ) if( x ) { x->Shutdown(); delete x; x = nullptr; }
#define SAFE_RELEASE( x ) if( x ) { x->Release(); x = nullptr; }
#define SAFE_DELETE( x ) if( x ) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY( x ) if( x ) { delete[] x; x = nullptr; }