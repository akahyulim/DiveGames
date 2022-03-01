#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <random>
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <filesystem>

#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex/DirectXTex.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>