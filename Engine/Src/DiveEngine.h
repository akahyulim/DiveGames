#pragma once

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 이름을 변경하고 싶다.

#include "divepch.h"

#include "Dive.h"

//= Base ==========================
#include "Base/Base.h"
#include "Base/Engine.h"
#include "Base/Time.h"
#include "Base/Log.h"

//= Events ========================
#include "Events/Event.h"
#include "Events/AppEvent.h"

//= Resource ======================

//= Renderer ======================
#include "Renderer/Graphics/GraphicsDevice.h"
#include "Renderer/Graphics/Texture.h"
#include "Renderer/Renderer.h"

//= Input =========================

//= Scene =========================
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Component.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Camera.h"

//= Math ==========================
