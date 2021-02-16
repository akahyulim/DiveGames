#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <cstdint>
#include <iostream>
#include <wchar.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <typeinfo.h>
#include <functional>
#include <sstream>
#include <guiddef.h>
#include <iomanip>
#include <any>
#include <Windows.h>
#include <WinUser.h>
#include <winerror.h>
#include <variant>
#include <typeindex>
#include "objbase.h"

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dinput.h>

#ifdef DIVE_WINDOW
	#include <Windows.h>
#endif

