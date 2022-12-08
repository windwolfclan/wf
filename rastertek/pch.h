#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <fstream>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

#define SAFE_SHUTDOWN( x ) if( x ) { x->Shutdown(); x->Release(); x = nullptr; }
#define SAFE_RELEASE( x ) if( x ) { x->Release(); x = nullptr; }