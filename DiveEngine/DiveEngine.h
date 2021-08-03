#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

//= Core ==========================
#include "DiveCore.h"
#include "Object.h"
#include "Runtime.h"
#include "Event.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "RenderPath3D.h"

//= Graphics ======================
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include "Renderer.h"
#include "GBuffer.h"
#include "VertexTypes.h"
#include "Mesh.h"
#include "Texture.h"
#include "dvTexture.h"
#include "dvTexture2D.h"
#include "dvRenderTexture.h"

//= Input =========================
#include "Input.h"

//= Scene =========================
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"

//= Helper ========================
#include "dvMath.h"
#include "Geometry.h"
#include "Timer.h"
#include "Log.h"

//= Tools =========================
#include "FontImporter.h"
#include "IniHelper.h"
#include "FileStream.h"
#include "StringHelper.h"
#include "FileSystemHelper.h"
#include "FormatHelper.h"


#pragma comment(lib, "DiveEngine.lib")