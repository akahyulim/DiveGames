#pragma once

// 프로젝트 전역 설정
#define DIRECTINPUT_VERSION 0x0800

// 표준 라이브러리
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <filesystem>

#include <memory>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <chrono>
#include <random>
#include <variant>
#include <thread>
#include <mutex>

#include <array>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>

// Windows 시스템 헤더
#include <Windows.h>
#include <atlconv.h>  // CString 변환 관련

// DirectX 관련 헤더
#include <wrl/client.h>
#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

// 외부 라이브러리
#include <DirectXTex/DirectXTex.h>
#include <yaml-cpp/yaml.h>

// 링커 라이브러리 설정
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

// 프로젝트 공용 헤더
#include "Common.h"
#include "core/Log.h"