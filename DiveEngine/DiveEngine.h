#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include "CommonInclude.h"

#include "Log.h"
#include "Object.h"
#include "Runtime.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "RenderPath3D.h"

//= Graphics ======================
#include "GraphicsDevice.h"
#include "Renderer.h"
#include "VertexTypes.h"
#include "Mesh.h"
#include "Texture.h"

//= Input =========================
#include "Input.h"

//= Scene =========================
#include "Scene.h"
//#include "GameObject.h"
//#include "Component.h"

//= Tool ==========================
#include "FontImporter.h"
#include "IniHelper.h"
#include "FileStream.h"
#include "StringHelper.h"
#include "FileSystemHelper.h"
#include "FormatHelper.h"


#pragma comment(lib, "DiveEngine.lib")