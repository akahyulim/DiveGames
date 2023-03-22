#pragma once

#ifndef DV_ASSERT
#include <assert.h>
#define DV_ASSERT(_EXPR) assert(_EXPR)
#endif

#ifndef DV_DELETE
#define DV_DELETE(p) { if(p) delete (p); (p) = nullptr; }
#endif

#ifndef DV_DELETE_ARRAY
#define DV_DELETE_ARRAY(p) { if(p) delete[](p); (p) = nullptr; }
#endif

#ifndef DV_RELEASE
#define DV_RELEASE(p) { if(p) (p)->Release(); (p) = nullptr; }
#endif