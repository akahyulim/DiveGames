#pragma once

// pch
#include "stdafx.h"

// core
#include "core/InstanceID.h"
#include "core/Log.h"
#include "core/StringUtils.h"
#include "core/FileUtils.h"
#include "core/EventDispatcher.h"
#include "core/Window.h"
#include "core/Input.h"
#include "core/Engine.h"

// graphics
#include "graphics/Graphics.h"
#include "graphics/Texture.h"
#include "graphics/Texture2D.h"
#include "graphics/RenderTexture.h"
#include "graphics/Cubemap.h"
#include "graphics/Shader.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/ConstantBuffer.h"

// math
#include "math/Math.h"
#include "math/Frustum.h"
#include "math/BoundingBox.h"

// rendering
#include "rendering/Mesh.h"
#include "rendering/StaticMesh.h"
#include "rendering/SkinnedMesh.h"
#include "rendering/MeshFactory.h"
#include "rendering/Material.h"
#include "rendering/Renderer.h"

// resource
#include "resource/Resource.h"
#include "resource/ResourceManager.h"

// world
#include "world/Components/Component.h"
#include "world/Components/Transform.h"
#include "world/GameObject.h"
#include "world/World.h"
#include "world/WorldSerializer.h"