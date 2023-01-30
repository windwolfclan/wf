#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>


// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

#define NAMESPACE_WF_S	namespace wf {
#define NAMESPACE_WF_E	}

#define SAFE_DELETE( p ) if( p ) { delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY( p ) if ( p ) { delete[] p; p = nullptr; }

#define SAFE_RELEASE( p ) if ( p ) { p->Release(); p = nullptr; }

#include "D3D12Context.h"
#include "D3DUtil.h"
#include "GameTimer.h"
#include "AppBase.h"