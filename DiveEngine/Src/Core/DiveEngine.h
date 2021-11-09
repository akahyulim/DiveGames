#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 결국 이 곳에 std lib와 runtime base lib를 include해야 한다.
// 그리고 이는 오직 runtime 내부에서만 include해야 한다.
// app에서는 개별 header를 include한다.

//= Core ==========================
#include "DiveCore.h"
#include "Object.h"
#include "Event.h"
#include "ThreadPool.h"
// 아래 세 개의 항목은 따로 분리하는 게 나을 것 같다.
#include "Runtime.h"
#include "RenderPath.h"
#include "RenderPath3D_Legacy.h"

//= Resource ======================
#include "../Resource/Resource.h"
#include "../Resource/ResourceManager.h"

//= Renderer ======================
#include "../Renderer/Graphics/GraphicsInclude.h"
#include "../Renderer/Graphics/GraphicsDevice.h"
#include "../Renderer/Graphics/dvTexture.h"
#include "../Renderer/Graphics/dvTexture2D.h"
#include "../Renderer/Graphics/dvRenderTexture.h"
#include "../Renderer/Graphics/Texture.h"			// 삭제 대상
#include "../Renderer/Renderer.h"
#include "../Renderer/GraphicsEnums.h"
#include "../Renderer/VertexTypes.h"
#include "../Renderer/Mesh.h"

//= Input =========================
#include "../Input/Input.h"

//= Scene =========================
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Scene/GameObject.h"
#include "../Scene/Component/Component.h"
#include "../Scene/Component/Transform.h"
#include "../Scene/Component/MeshRenderer.h"
#include "../Scene/Component/Camera.h"
#include "../Scene/Component/Light.h"

//= Helper ========================
// 나중에 더 나눌 예정이다.
#include "../Helper/dvMath.h"
#include "../Helper/Geometry.h"
#include "../Helper/Timer.h"
#include "../Helper/Log.h"
#include "../Helper/FontImporter.h"
#include "../Helper/IniHelper.h"
#include "../Helper/FileStream.h"
#include "../Helper/StringHelper.h"
#include "../Helper/FileSystemHelper.h"
#include "../Helper/FormatHelper.h"


#pragma comment(lib, "DiveEngine.lib")