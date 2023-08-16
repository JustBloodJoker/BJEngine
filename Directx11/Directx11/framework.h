#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


#define _XM_NO_INTRINSICS_
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <dxsdk-d3dx/D3DX11.h>
#include <d3d11.h>

#include <vector>

#include <string>


#include <DirectXMath.h>
namespace dx = DirectX;

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fstream>

#include <map>

#include <d3dcompiler.h>


#include <dwrite.h>
#include <d2d1.h>



#include <xaudio2.h>
#include <x3daudio.h>

#include <dinput.h>