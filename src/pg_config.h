#ifndef PG_CONFIG
#define PG_CONFIG

/*************************
* Platform declaration   *
*************************/
#if defined (WIN64)  // Windows 64-bit
	#define PG_PLATFORM_WIN32
	#define PG_PLATFORM_WIN_64BIT
#elif defined (WIN32) // Windows 32-bit
	#define PG_PLATFORM_WIN32
	#define PG_PLATFORM_WIN_32BIT
#elif defined (_WIN32_WCE) // Windows Mobile
	#define PG_PLATFORM_WIN32
	#define PG_PLATFORM_WINCE
#endif

#if defined (__APPLE__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE // iOS 
		#define PG_PLATFORM_IOS
	#elif TARGET_OS_MAC // MacOS Desktop
		#define PG_PLATFORM_MACOS
		#define PG_PLATFORM_COCOA
	#endif

	#if defined (__LP64__)
		#define PG_PLATFORM_MAC_64BIT
	#else
		#define PG_PLATFORM_MAC_32BIT
	#endif
#endif

#if defined (PG_PLATFORM_WIN_64BIT) || defined (PG_PLATFORM_MAC_64BIT)
	#define PG_PLATFORM_64BIT
#elif defined (PG_PLATFORM_WIN_32BIT) || defined (PG_PLATFORM_MAC_32BIT)
	#define PG_PLATFORM_32BIT
#endif

/******************************
* Platform-neutral headers    *
******************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#if defined(PG_PLATFORM_WIN32) && defined(_DEBUG) // Enable Windows CRT Memory Leak detection in Debug builds
//	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
//	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif

/******************************
* Platform-specific headers   *
******************************/
#if defined(PG_PLATFORM_WIN32)
/*	#define WIN32_EXTRA_LEAN
	#define _WINSOCKAPI_ // Don't include winsock.h, use winsock2.h
	#include <windows.h>
	#include <tchar.h>
	#ifdef PG_PLATFORM_WINCE
		#include <gx.h>
		#include <aygshell.h>
	#endif*/
#elif defined(PG_PLATFORM_MACOS)
/*	#include <Carbon/Carbon.h>*/
#elif defined(PG_PLATFORM_LINUX)
/*	#include <gtk/gtk.h>*/
#else
	#define PG_PLATFORM_GENERIC
#endif

/***************************
* Basic Types              *
* Modify these if their    *
* sizes aren't correct.    *
***************************/

/* integer types of specific sizes: */
typedef char			int8;		/* 8 bits  */
typedef short			int16;		/* 16 bits */
typedef int				int32;		/* 32 bits */
typedef long long		int64;		/* 64 bits */

/* unsigned integer types of specific sizes: */
typedef unsigned char	uint8;		/* 8 bits  */
typedef unsigned short	uint16;		/* 16 bits */
typedef unsigned int	uint32;		/* 32 bits */
typedef unsigned long long uint64;	/* 64 bits */

/* floating-point value: */
typedef float			real;	/* most efficient floating-point type (may depend on platform and compiler) */

/* assertion */
#ifdef PG_ENABLE_ASSERT
	#define PG_ASSERT(x)		(assert(x))
#else
	#define PG_ASSERT(x)		((void)0)
#endif

/**************
* Constants   *
**************/
#define PG_TINY		1e-6f
#define PG_HUGE		1e6f
#define PG_PI		3.14159265359f
#define PG_2PI		6.28318530718f
#define PG_PI_OVER2	1.5707963268f
#ifdef PG_PLATFORM_MACOS
#define PG_RAND_MAX	RAND_MAX
#else
#define PG_RAND_MAX	RAND_MAX
#endif

/***********************
* One Argument Macros  *
***********************/
#define PG_UNUSED(a)		(void)((a))
#define PG_ABS(a)			(((a) < 0) ? -(a) : (a))
#define PG_FABS(a)			(((a) < 0.0f) ? -(a) : (a))
#define PG_ROUND(a)			(((a) > 0) ? (int32)((a) + 0.5f) : -(int32)(0.5f - (a)))
/* take sign of a, either -1, 0, or 1 */
#define PG_ZSGN(a)			(((a) < 0) ? -1 : ((a) > 0) ? 1 : 0)
/* take binary sign of a, either -1, or 1 if >= 0 */
#define PG_SGN(a)			(((a) < 0) ? -1 : 1)
#define PG_SQR(a)			((a) * (a))
/* bitwise NOT */
#define PG_NOT(a)			(0xFFFFFFFF ^ (a))
/* detect NAN and INF numbers &*/
#ifdef PG_PLATFORM_WIN32
#include <float.h>
#define PG_ISNAN(x)	_isnan(x)
#define PG_ISINF(x)	(!_finite(x))
#else
#define PG_ISNAN(x)	isnan(x)
#define PG_ISINF(x)	isinf(x)
#endif
/* set a random seed */
#ifdef PG_PLATFORM_MACOS
#define PG_SRANDOM(s)		srandom((s))
#else
#define PG_SRANDOM(s)		srand((s))
#endif
/* random non-negative integer */
#ifdef PG_PLATFORM_MACOS
#define PG_RANDOM()			random()
#else
#define PG_RANDOM()			rand()
#endif
/* random number between 0 and 1 */
#define PG_FRANDOM()		((real)PG_RANDOM() / (real)PG_RAND_MAX)
/* random number between -1 and 1 */
#define PG_FRANDOMSGN()		(PG_FRANDOM() * 2.0f - 1.0f)
/* floor and ceil macros (much faster than the math.h functions) */
#define PG_FLOOR(x)			(((x) < 0.0f) ? ((int32)(x) - 1) : ((int32)(x)))
#define PG_CEIL(x)			(((x) < 0.0f) ? ((int32)(x)) : ((int32)(x) + 1))
/* degree <--> radian conversions */
#define PG_RAD(x)			((x) * PG_PI / 180.0f)
#define PG_DEG(x)			((x) * 180.0f / PG_PI)

/************************
* Multi-Argument Macros *
************************/
#define PG_MIN(a,b)				(((a) < (b)) ? (a) : (b))
#define PG_MAX(a,b)				(((a) > (b)) ? (a) : (b))
#define PG_SWAP(a,b)			{ a ^= b; b ^= a; a ^= b; }
/* linear interpolation from l (when a=0) to h (when a=1) */
/* (equal to (a*h)+((1-a)*l) */
#define PG_LERP(a,l,h)			((l) + (((h) - (l)) * (a)))
/* clamp the input to the specified range */
#define PG_CLAMP(v,l,h)			((v) < (l) ? (l) : (v) > (h) ? (h) : v)

#endif
