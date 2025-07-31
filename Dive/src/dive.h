#pragma once

// pch
#include "stdafx.h"

// core
#include "core/Object.h"
#include "core/Log.h"
#include "core/StringUtils.h"
#include "core/FileUtils.h"
#include "core/EventDispatcher.h"
#include "core/Window.h"
#include "core/Input.h"
#include "core/Engine.h"

// graphics
#include "graphics/Graphics.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/Texture2D.h"
#include "graphics/RenderTexture.h"
#include "graphics/Cubemap.h"
#include "graphics/InputLayoutKey.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Buffer.h"
#include "graphics/ConstantBuffer.h"

// math
#include "math/Math.h"
#include "math/Frustum.h"
#include "math/BoundingBox.h"

// rendering
#include "rendering/RenderDefs.h"
#include "rendering/StaticMesh.h"
//#include "rendering/SkinnedMesh.h"
#include "rendering/MeshFactory.h"
#include "rendering/Material.h"
#include "rendering/Renderer.h"
#include "rendering/RenderPass.h"

// resource
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "resource/YamlHelper.h"

// world
#include "world/Components/Component.h"
#include "world/Components/Transform.h"
#include "world/Components/Camera.h"
#include "world/Components/MeshRenderer.h"
#include "world/GameObject.h"
#include "world/World.h"
#include "world/WorldSerializer.h"
