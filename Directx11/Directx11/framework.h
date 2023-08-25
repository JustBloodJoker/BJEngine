#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>


#define NOMINMAX
#define _XM_NO_INTRINSICS_
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <dxsdk-d3dx/D3DX11.h>
#include <d3d11.h>



#include <DirectXMath.h>
namespace dx = DirectX;

#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <sstream>
#include <map>

#include <d3dcompiler.h>
#include <dwrite.h>
#include <d2d1.h>
#include <xaudio2.h>
#include <x3daudio.h>
#include <dinput.h>

#include <locale>
#include <codecvt>