#pragma once
#include "Log.h"

#define DV_DELETE(p) { if(p) delete (p); (p) = nullptr; }
#define DV_DELETE_ARRAY(p) { if(p) delete[](p); (p) = nullptr; }
#define DV_RELEASE(p) { if(p) (p)->Release(); (p) = nullptr; }
