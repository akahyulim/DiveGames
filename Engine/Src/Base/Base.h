#pragma once

#ifndef DV_ASSERT
#include <assert.h>
#define DV_ASSERT(_EXPR) assert(_EXPR)
#endif

#ifndef DIVE_DELETE
#define DIVE_DELETE(p) { if(p) delete (p); (p) = nullptr; }
#endif

#ifndef DIVE_DELETE_ARRAY
#define DIVE_DELETE_ARRAY(p) { if(p) delete[](p); (p) = nullptr; }
#endif

#ifndef DIVE_RELEASE
#define DIVE_RELEASE(p) { if(p) (p)->Release(); (p) = nullptr; }
#endif

#include "Log.h"