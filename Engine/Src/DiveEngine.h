#pragma once

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 결국 이 곳에 std lib와 runtime base lib를 include해야 한다.
// 그리고 이는 오직 runtime 내부에서만 include해야 한다.
// app에서는 개별 header를 include한다.

#include "divepch.h"

//= Base ==========================
#include "Base/DiveCore.h"
#include "Base/Object.h"
#include "Base/Event.h"
#include "Base/ThreadPool.h"
#include "Base/Runtime.h"
#include "Base/RenderPath.h"
#include "Base/RenderPath3D_Legacy.h"
#include "Base/Timer.h"
#include "Base/Log.h"
#include "Base/IniHelper.h"
#include "Base/FileStream.h"
#include "Base/FormatHelper.h"

//= Resource ======================
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"

//= Renderer ======================
#include "Renderer/Graphics/GraphicsInclude.h"
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Graphics/dvTexture.h"
#include "Renderer/Graphics/dvTexture2D.h"
#include "Renderer/Graphics/dvRenderTexture.h"
#include "Renderer/Graphics/Texture.h"			// 삭제 대상
#include "Renderer/Renderer.h"
#include "Renderer/GraphicsEnums.h"
#include "Renderer/VertexTypes.h"
#include "Renderer/Mesh.h"
#include "Renderer/dvMesh.h"
#include "Renderer/StaticMesh.h"

//= Input =========================
#include "Input/Input.h"

//= Scene =========================
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/MeshRenderer.h"
#include "Scene/Component/dvMeshRenderer.h"
#include "Scene/Component/StaticMeshRenderer.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Light.h"

//= Utils ========================
#include "Utils/Math.h"
#include "Utils/Geometry.h"
#include "Utils/String.h"
#include "Utils/FileSystem.h"