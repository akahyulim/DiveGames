#pragma once


#ifdef DIVE_WINDOW
	#ifdef DIVE_ENGINE
		#define DIVE_CLASS __declspec(dllexport)
	#else
		#define DIVE_CLASS __declspec(dllimport)
	#endif
#endif