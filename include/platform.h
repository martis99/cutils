#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define P_WIN
	#if defined(_WIN64)
		#define P_WIN64
	#else
		#define P_WIN32
	#endif
#elif __linux__
	#define P_LINUX
#else
	#error "Platform not supported"
#endif

#if defined(P_WIN)
	#include <Windows.h>
	#define P_MAX_PATH MAX_PATH
#else
	#define P_MAX_PATH 256
#endif

#endif
