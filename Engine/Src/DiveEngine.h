#pragma once

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 이름을 변경하고 싶다.

#include "divepch.h"

//= Core ==========================
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "Core/Object.h"
#include "Core/EventSystem.h"
#include "Core/Timer.h"
#include "Core/Variant.h"

//= Engine ========================
#include "Engine/EngineDef.h"
#include "Engine/Application.h"
#include "Engine/Engine.h"

//= IO ============================
#include "IO/Log.h"
#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "IO/IOEvents.h"

//= Resource ======================
#include "Resource/Resource.h"
#include "Resource/ResourceCache.h"

//= Graphics ======================
#include "Graphics/GraphicsDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsEvents.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/InputLayout.h"

//= Renderer ======================
#include "Renderer/Renderer.h"
#include "Renderer/Model.h"
#include "Renderer/Mesh.h"
#include "Renderer/View.h"

//= Input =========================

//= Scene =========================
#include "Scene/Scene.h" 
#include "Scene/GameObject.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/StaticModel.h"

//= Math ==========================
#include "Math/MathDefs.h"
#include "Math/StringHash.h"
#include "Math/Math.h"

//= Helper ========================
