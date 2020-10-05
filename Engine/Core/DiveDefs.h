#pragma once
//#include <string>
//#include <stdint.h>

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable: 4251)

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = nullptr; } }
#endif