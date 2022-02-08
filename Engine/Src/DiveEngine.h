#pragma once

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// 결국 이 곳에 std lib와 runtime base lib를 include해야 한다.
// 그리고 이는 오직 runtime 내부에서만 include해야 한다.
// app에서는 개별 header를 include한다.

#include "divepch.h"

//= Base ==========================
#include "Base/Base.h"
#include "Base/Object.h"
#include "Base/Engine.h"
#include "Base/Time.h"
#include "Base/Log.h"

//= Events ========================
#include "Events/Event.h"
#include "Events/AppEvent.h"

//= Resource ======================

//= Renderer ======================

//= Input =========================

//= Scene =========================

//= Math ==========================
