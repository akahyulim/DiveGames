#pragma once

#include "DivePch.h"

//= Core ==================================================
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Stopwatch.h"
#include "Core/Object.h"
#include "Core/Context.h"
#include "Core/Time.h"
#include "Core/StringHelper.h"
#include "Core/FileSystemHelper.h"
#include "Core/IniHelper.h"
#include "Core/Main.h"
#include "Core/EventSystem.h"
#include "Core/CoreEvents.h"


//= Engine ================================================
#include "Engine/Engine.h"
#include "Engine/Application.h"


//= Geometry ==============================================
//#include "Geometry/Math.h"
#include "Geometry/Mesh.h"


//= Input =================================================
#include "Input/Input.h"


//= Graphics ==============================================
#include "Graphics//D3D11/Shader.h"
#include "Graphics/D3D11/IndexBuffer.h"
#include "Graphics/D3D11/ConstantBuffer.h"
#include "Graphics/D3D11/RenderDeviceEnums.h"
#include "Graphics/D3D11/RenderDeviceTypes.h"
#include "Graphics/D3D11/DepthStencilState.h"
#include "Graphics/D3D11/RasterizerState.h"
#include "Graphics/D3D11/BlendState.h"
#include "Graphics/D3D11/Sampler.h"
#include "Graphics/D3D11/Texture.h"
#include "Graphics/D3D11/Texture2D.h"
#include "Graphics/GraphicsEnums.h"
#include "Graphics/DeviceAndSwapChain.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Material.h"
#include "Graphics/Legacy.h"
#include "Graphics/Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Window.h"


//= Scene =================================================
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Component.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/RenderableMesh.h"
#include "Scene/Components/SkinnedRenderableMesh.h"
