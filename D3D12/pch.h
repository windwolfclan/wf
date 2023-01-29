#pragma once

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

using namespace DirectX;

#define NAMESPACE_WF_S	namespace wf {
#define NAMESPACE_WF_E	}

#define SAFE_DELETE( p ) if( p ) { delete p; p = nullptr }
#define SAFE_DELETE_ARRAY( p ) if ( p ) { delete[] p; p = nullptr }

#define SAFE_RELEASE( p ) if ( p ) { p->Release(); p = nullptr }

#include "GameTimer.h"
#include "AppBase.h"