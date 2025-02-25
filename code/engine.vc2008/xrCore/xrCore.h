#pragma once
#pragma warning (disable : 4530 )		// C++ vector(985)

#define ENGINE_VERSION "1.7f"

#ifndef DEBUG
#	define MASTER_GOLD
#endif // DEBUG

//#define BENCHMARK_BUILD

#ifdef	BENCHMARK_BUILD
#	define	BENCH_SEC_CALLCONV			__stdcall
#	define	BENCH_SEC_SCRAMBLEVTBL1		virtual int GetFlags()	{ return 1;}
#	define	BENCH_SEC_SCRAMBLEVTBL2		virtual void* GetData()	{ return 0;}
#	define	BENCH_SEC_SCRAMBLEVTBL3		virtual void* GetCache(){ return 0;}
#	define	BENCH_SEC_SIGN				, void *pBenchScrampleVoid = 0
#	define	BENCH_SEC_SCRAMBLEMEMBER1	float	m_fSrambleMember1;
#	define	BENCH_SEC_SCRAMBLEMEMBER2	float	m_fSrambleMember2;
#else	//	BENCHMARK_BUILD
#	define	BENCH_SEC_CALLCONV
#	define	BENCH_SEC_SCRAMBLEVTBL1
#	define	BENCH_SEC_SCRAMBLEVTBL2
#	define	BENCH_SEC_SCRAMBLEVTBL3
#	define	BENCH_SEC_SIGN
#	define	BENCH_SEC_SCRAMBLEMEMBER1
#	define	BENCH_SEC_SCRAMBLEMEMBER2
#endif	//	BENCHMARK_BUILD

#pragma warning(disable:4996)

#if (defined(_DEBUG) || defined(MIXED) || defined(DEBUG)) && !defined(FORCE_NO_EXCEPTIONS)
	// "debug" or "mixed"
#if !defined(_CPPUNWIND)
#error Please enable exceptions...
#endif
#define _HAS_EXCEPTIONS		1	// STL
#define XRAY_EXCEPTIONS		1	// XRAY

#define XR_NOEXCEPT noexcept
#define XR_NOEXCEPT_OP(x) noexcept(x)
#else
	// "release"
#if defined(_CPPUNWIND) && !defined __BORLANDC__ && !defined(_XRLAUNCHER) && !defined(__llvm__)
#error Please disable exceptions...
#endif
#define _HAS_EXCEPTIONS		1	// STL
#define XRAY_EXCEPTIONS		0	// XRAY
#pragma warning(disable:4530)

#define XR_NOEXCEPT throw()
#define XR_NOEXCEPT_OP(x)
#endif

#if !defined(_MT)
	// multithreading disabled
#error Please enable multi-threaded library...
#endif

#	include "xrCore_platform.h"

// *** try to minimize code bloat of STLport
#ifdef XRCORE_EXPORTS				// no exceptions, export allocator and common stuff
#define _STLP_DESIGNATED_DLL	1
#define _STLP_USE_DECLSPEC		1
#else
#define _STLP_USE_DECLSPEC		1	// no exceptions, import allocator and common stuff
#endif

// #include <exception>
// using std::exception;

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <typeinfo.h>
#include <cinttypes>
#include <chrono>

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif
#ifdef MIXED
#define DEBUG
#endif
#endif

#ifdef _EDITOR
#	define NO_FS_SCAN
#endif

// inline control - redefine to use compiler's heuristics ONLY
// it seems "IC" is misused in many places which cause code-bloat
// ...and VC7.1 really don't miss opportunities for inline :)
#define _inline			inline
#define __inline		inline
#define IC				inline
#define ICF				__forceinline			// !!! this should be used only in critical places found by PROFILER

#	define ICN			__declspec (noinline)

#ifndef DEBUG
#pragma inline_depth	( 254 )
#pragma inline_recursion( on )
#ifndef __BORLANDC__
#pragma intrinsic	(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcat)
#endif
#endif

#include <time.h>
// work-around dumb borland compiler
#define ALIGN(a)		__declspec(align(a))
#include <sys\utime.h>

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4345 )
#pragma warning (disable : 4714 )		// __forceinline not inlined
#ifndef DEBUG
#pragma warning (disable : 4189 )		//  local variable is initialized but not refenced
#endif									//	frequently in release code due to large amount of VERIFY

#ifdef _M_AMD64
#pragma warning (disable : 4512 )
#endif

// stl
#pragma warning (push)
#pragma warning (disable:4702)
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>

#include <unordered_map>
#include <unordered_set>

#include <string>
#pragma warning (pop)
#pragma warning (disable : 4100 )		// unreferenced formal parameter

// Our headers
#ifdef XRCORE_EXPORTS
#	define XRCORE_API __declspec(dllexport)
#else
#	define XRCORE_API __declspec(dllimport)
#endif

#include "_types.h"
#include "xrMemory.h"
#include "_stl_extensions.h"
#include "thread_utils.h"
#include "xrDebug.h"
#include "vector.h"
#include "clsid.h"
#include "xrDebug.h"

#include "xrsharedmem.h"
#include "xrstring.h"
#include "xr_resource.h"
#include "rt_compressor.h"
#include "xr_shared.h"
#include "string_concatenations.h"

// stl ext
struct XRCORE_API xr_rtoken {
	shared_str	name;
	int	   	id;
	xr_rtoken(const char* _nm, int _id) { name = _nm; id = _id; }
public:
	void	rename(const char* _nm) { name = _nm; }
	bool	equal(const char* _nm) { return (0 == xr_strcmp(*name, _nm)); }
};

#pragma pack (push,1)
struct XRCORE_API xr_shortcut {
	enum {
		flShift = 0x20,
		flCtrl = 0x40,
		flAlt = 0x80,
	};
	union {
		struct {
			u8	 	key;
			Flags8	ext;
		};
		u16		hotkey;
	};
	xr_shortcut(u8 k, BOOL a, BOOL c, BOOL s) :key(k) { ext.assign(u8((a ? flAlt : 0) | (c ? flCtrl : 0) | (s ? flShift : 0))); }
	xr_shortcut() { ext.zero(); key = 0; }
	bool		similar(const xr_shortcut& v)const { return (ext.flags == v.ext.flags) && (key == v.key); }
};
#pragma pack (pop)

using RStringVec = xr_vector<shared_str>;
using RStringSet = xr_set<shared_str>;
using RTokenVec = xr_vector<xr_rtoken>;

#define			xr_pure_interface	__interface

#include "FS.h"
#include "log.h"
#include "xr_trims.h"
#include "xr_ini.h"
#ifdef NO_FS_SCAN
#	include "ELocatorAPI.h"
#else
#	include "LocatorAPI.h"
#endif
#include "FileSystem.h"
#include "FTimer.h"
#include "fastdelegate.h"
#include "intrusive_ptr.h"
#ifndef XRCORE_STATIC
#include "net_utils.h"
#endif

// Check if user included some files, that a prohibited
#ifdef _MUTEX_
#error <mutex> file is prohibited, please use xrCriticalSection and xrCriticalSectionGuard instead
#endif
// Ban std::thread also
#ifdef _THREAD_
#error <thread> is prohibited, please use ttapi, or _beginthreadex
#endif

// destructor
template <class T>
class destructor
{
	T* ptr;
public:
	destructor(T* p) { ptr = p; }
	~destructor() { xr_delete(ptr); }
	IC T& operator() ()
	{
		return *ptr;
	}
};

// ********************************************** The Core definition
class XRCORE_API xrCore
{
public:
	string64	ApplicationName;
	string_path	ApplicationPath;
	string_path	WorkingPath;
	string64	UserName;
	string64	CompName;
	string64	UserDate;
	string64	UserTime;
	string1024	Params;
	DWORD		dwFrame;

public:
	void		_initialize(const char* ApplicationName, LogCallback cb = 0, BOOL init_fs = TRUE, const char* fs_fname = 0);
	void		_destroy();
	IC	void		SetPluginMode() { PluginMode = true; }

public:
	bool		PluginMode;
};

//Borland class dll interface
#define	_BCL			__stdcall

//Borland global function dll interface
#define	_BGCL			__stdcall

extern XRCORE_API xrCore Core;
extern XRCORE_API bool   gModulesLoaded;