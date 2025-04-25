#pragma once

// pch
#include "stdafx.h"

// core
#include "core/UUID.h"
#include "core/Log.h"
#include "core/StringUtils.h"
#include "core/Object.h"
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

// transforms
#include "transforms/transforms.h"

// rendering
#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/ModelFactory.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"

// resource
#include "resource/Resource.h"
#include "resource/ResourceManager.h"

// world
#include "world/World.h"
#include "world/GameObject.h"
#include "world/Components.h"
#include "world/WorldSerializer.h"