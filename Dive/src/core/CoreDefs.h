#pragma once
#include <Windows.h>
#include <filesystem>
#include "Log.h"

#define DV_DELETE(p) { if(p) delete (p); (p) = nullptr; }
#define DV_DELETE_ARRAY(p) { if(p) delete[](p); (p) = nullptr; }
#define DV_RELEASE(p) { if(p) (p)->Release(); (p) = nullptr; }

#define DV_EXPAND_MACRO(x) x
#define DV_STRINGIFY_MACRO(x) #x

#ifdef _DEBUG
	#define DV_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { DV_LOG(type, critical, msg, __VA_ARGS__); __debugbreak(); } }
	#define DV_INTERNAL_ASSERT_WITH_MSG(type, check, ...) DV_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}, file {1}, line {2}", __VA_ARGS__, std::filesystem::path(__FILE__).filename().string(), __LINE__)
	#define DV_INTERNAL_ASSERT_NO_MSG(type, check) DV_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}, file {1}, line {2}", DV_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define DV_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define DV_INTERNAL_ASSERT_GET_MACRO(...) DV_EXPAND_MACRO( DV_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, DV_INTERNAL_ASSERT_WITH_MSG, DV_INTERNAL_ASSERT_NO_MSG) )

	#define DV_ASSERT(type, ...) DV_EXPAND_MACRO( DV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(type, __VA_ARGS__) )

#else
	#define DV_ASSERT(...)
#endif
