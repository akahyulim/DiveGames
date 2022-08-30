#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.


#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <random>
#include <functional>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include <Windows.h>

#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Xinput.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")


using namespace std::string_literals;