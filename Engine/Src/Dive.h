#pragma once

#include "DivePch.h"

//= Core ==================================================
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Core/Stopwatch.h"
#include "Core/System.h"
#include "Core/SystemManager.h"
#include "Core/Timer.h"
#include "Core/Engine.h"
#include "Core/Settings.h"
#include "Core/StringHelper.h"
#include "Core/FileSystemHelper.h"
#include "Core/IniHelper.h"
#include "Core/Settings.h"
#include "Core/Application.h"
#include "Core/Main.h"

//= Event =================================================
#include "Events/EventSystem.h"
#include "Events/WindowEvent.h"

//= Rendering =============================================
#include "Rendering/D3D11/VertexTypes.h"
#include "Rendering/D3D11/VertexTypes.h"
#include "Rendering/D3D11/IndexBuffer.h"
#include "Rendering/D3D11/ConstantBuffer.h"
#include "Rendering/D3D11/RenderDeviceEnums.h"
#include "Rendering/D3D11/RenderDeviceTypes.h"
#include "Rendering/D3D11/RenderDevice.h"
#include "Rendering/D3D11/DepthStencilState.h"
#include "Rendering/D3D11/RasterizerState.h"
#include "Rendering/D3D11/BlendState.h"
#include "Rendering/Renderer.h"