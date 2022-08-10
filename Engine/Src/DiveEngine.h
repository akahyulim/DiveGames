#pragma once

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 이름을 변경하고 싶다.

#include "divepch.h"

#include "Dive.h"

//= Base ==========================
#include "Base/Base.h"
#include "Base/Engine.h"
#include "Base/Object.h"
#include "Base/Time.h"
#include "Base/Log.h"

//= Core ==========================
#include "Core/StringHash.h"
#include "Core/DvContext.h"
#include "Core/DvObject.h"
#include "Core/DvEventSystem.h"
#include "Core/DvTimer.h"

//= Engine ========================
#include "Engine/DvEngineDef.h"
#include "Engine/Application.h"
#include "Engine/DvEngine.h"

//= IO ============================
#include "IO/DvLog.h"
#include "IO/IOEvents.h"

//= Events ========================
#include "Events/EventSystem.h"
#include "Events/EngineEvents.h"

//= Resource ======================
#include "Resource/Importer/ModelImporter.h"
#include "Resource/FileStream.h"
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"

//= Graphics ======================
#include "Graphics/DvGraphics.h"
#include "Graphics/GraphicsEvents.h"

//= Renderer ======================
#include "Renderer/Graphics/GraphicsDef.h"
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Graphics/Texture.h"
#include "Renderer/Graphics/Texture2D.h"
#include "Renderer/Graphics/TextureCube.h"
#include "Renderer/Graphics/VertexBuffer.h"
#include "Renderer/Graphics/DvVertexBuffer.h"
#include "Renderer/Graphics/IndexBuffer.h"
#include "Renderer/Graphics/DvIndexBuffer.h"
#include "Renderer/Graphics/ConstantBuffer.h"
#include "Renderer/ConstantBuffers.h"
#include "Renderer/Renderer.h"
#include "Renderer/DvRenderer.h"
#include "Renderer/PipelineState.h"
#include "Renderer/CommandList.h"
#include "Renderer/RenderPath.h"
#include "Renderer/Mesh.h"
#include "Renderer/DvMesh.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "Renderer/DvModel.h"
#include "Renderer/DvView.h"

//= Input =========================
#include "Input/input.h"

//= Scene =========================
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/SpriteRenderable.h"
#include "Scene/Component/MeshRenderable.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Light.h"

//= Math ==========================
#include "Math/Math.h"

//= Helper ========================
#include "Helper/FileSystem.h"
