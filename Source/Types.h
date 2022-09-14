// ========================================
// !!! MUST BE INCLUDED FIRST EVERWHERE !!!
// ========================================
// 
// Defines :
//   base types (s8,s16,s32,s64,u8,u16,u32,u64),
//   macros to make functions compatible between VC++/GCC, 
//   macros WIN32 (for any version of Windows), UNIX (for Linux), DEBUG, PLATFORM_NB_BITS (32 or 64),
//   macros MY_ASSERT(x), sizeofarray(array), sizeofmember(type, member),
//   macros M_PI, M_PI_2, M_2_PI, ... M_LOG_2,
//   ConsoleWriteXXX(...) : global logging functions
//
// Ensures:
//   Compiler is C++ 11 compliant,
//   Some types sizes,
//   
// Does :
//   All the needed C/C++/system #includes,
//   Includes libraries (VC++ only).

#pragma once

/****************************************************************************\
*                                                                            *
*  Per system #define (BEFORE INCLUDES)                                      *
*                                                                            *
\****************************************************************************/

//#define INCLUDE_BOOST

#if defined(__unix__) || defined(linux) || defined(__linux) || defined(__linux__)

//	#warning "Unix/Linux environment"
	#if !defined(UNIX)
		#define UNIX
	#endif
	#define __OS__ "Linux"
	#define _FILE_OFFSET_BITS 64		// fseek64/ftell64

	#define __dup		dup
	#define __dup2		dup2
	#define __close		close
	#define __fdopen	fdopen
	#define __fileno	fileno
	#define __chdir		chdir
	#define __mkdir(p)	mkdir(p,S_IRWXU|S_IRWXG|S_IROTH)
	#define __rmdir		rmdir
	#define __getcwd	getcwd
	#define __access	access
	#define __unlink	unlink
	#define __fseek64	fseeko
	#define __ftell64	ftello
	#define __sstat64	stat64
	#define __stat64	stat64
	#define __spawnl	spawnl
	#define __getch		getch					// defined in linux_compat.h
	#define __kbhit		kbhit					// defined in linux_compat.h
	#define __strtoll	strtoll					// 64 bits signed atoi
	#define __strtoul	strtoul					// 64 bits unsigned atoi
	#define __vswprintf	vswprintf
	#define __write		write
	#define __isatty	isatty
	#define __fileno	fileno

#elif defined(_WIN32) || defined(WIN32) || defined(__WIN32__)

//	#pragma message("Windows environment")
	#if !defined(WIN32)
		#define WIN32
	#endif
	#define __OS__ "Windows"
	#define WIN32_LEAN_AND_MEAN					// no MFC overhead, limit Win32 pollution
	#define HANDLE_PRAGMA_PACK_PUSH_POP		1	// To use "#pragma pack" on WIN32 like GCC

	#define __dup		_dup					// To avoid a stupid warning on VC++
	#define __dup2		_dup2
	#define __close		_close
	#define __fdopen	_fdopen
	#define __fileno	_fileno
	#define __chdir		_chdir
	#define __mkdir		_mkdir
	#define __rmdir		_rmdir
	#define __getcwd	_getcwd
	#define __access	_access
	#define __unlink	_unlink
	#define __fseek64	_fseeki64
	#define __ftell64	_ftelli64
	#define __sstat64	__stat64				// structure, not a function.
	#define __stat64	_stat64					// Attention  l'ordre de ces 2 lignes !
	#define __spawnl	_spawnl
	#define __getch		_getch
	#define __kbhit		_kbhit
	#define __strtoll	_strtoi64				// 64 bits signed atoi
	#define __strtoul	_strtoui64				// 64 bits unsigned atoi
	#define __vswprintf	_vsnwprintf
	#define __write		_write
	#define __isatty	_isatty
	#define __fileno	_fileno
#endif

#if defined(_DEBUG)
	#define DEBUG
#endif

#define ConsoleWrite(...)		(printf(__VA_ARGS__),printf("\n"))
#define ConsoleWriteOk(...)		(printf(__VA_ARGS__),printf("\n"))
#define ConsoleWriteWarn(...)	(printf(__VA_ARGS__),printf("\n"))
#define ConsoleWriteErr(...)	(printf(__VA_ARGS__),printf("\n"))

/****************************************************************************\
*                                                                            *
*    Les #include                                                            *
*                                                                            *
\****************************************************************************/
#if defined(WIN32)
////////////////////////////////////////////////////////////////////////////////////
	#include <windows.h>				// Windows management
	#include <psapi.h>					// for GetProcessMemoryInfo()
//	#include <mmsystem.h>				// Windows sound system
//	#include <vfw.h>					// Video For Windows
	#include <io.h>						// _open_osfhandle
	#pragma warning (push)
	#pragma warning (disable: 4985)
		#include <intrin.h>				// __cpuid, __rdtsc()
	#pragma warning (pop)
	#include <direct.h>					// directories functions
//	#define _WIN32_WINNT 0x0501
//	#include <commdlg.h>				// Windows common dialog
//	#include <commctrl.h>				// Windows common dialog
	#include <process.h>
	#include <conio.h>					// Just for _getch()
//	#include <winsock2.h>				// Conditional include to avoid a redefinition warning in some projects
	#define socklen_t int
////////////////////////////////////////////////////////////////////////////////////
#elif defined(UNIX)
////////////////////////////////////////////////////////////////////////////////////
	#include <unistd.h>					// getcwd, fork
	#include <termios.h>				// to control info on the console (read keyboard...)
	#include <stdint.h>					// defines [u]intXX_t types to define types uXX/sXX
	#include <dirent.h>					// basic i/o
	#include <execinfo.h>				// To dump call stack.
	#include <netinet/in.h>				// sockaddr
	#include <netdb.h>					//
	#include <arpa/inet.h>				//
	#include <sys/time.h>				// timeval
	#include <sys/wait.h>				// waitpid
	#include <sys/Types.h>				// types for socket()
	#include <sys/socket.h>				// socket()
	#include <sys/ioctl.h>				// 
//	#include <X11/keysym.h>				// keys symbols
//	#include <X11/Xlib.h>				// for keys, ...
//	#include <X11/Xutil.h>				// No X11 for the Gravity board and RPi, we access directly the frame buffer
//	#include <X11/Xatom.h>				//
	#define SOCKET_ERROR -1
////////////////////////////////////////////////////////////////////////////////////
#endif
#define GLEW_STATIC
#include <GL/glew.h>			// GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform
#include <GLFW/glfw3.h>			// Librairie facilitant l'initialisation de fenêtres et contextes OpenGL
#include <glm/glm.hpp>			// Librairie gérant les types vecteur/matrice pour OpenGL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	// Permet l'utilisation de glm::value_ptr()


////////////////////////////////////////////////////////////////////////////////////

// C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <climits>
#include <cfloat>
#include <csignal>
#include <ctime>
#include <clocale>
#if defined(WIN32)
	#pragma warning (push)
	#pragma warning (disable: 4985)	// to remove an annoying warning in 64 bits
#endif
 #include <cmath>
#if defined(WIN32)
	#pragma warning (pop)
#endif
#include <cerrno>
#include <cwchar>
#include <sys/stat.h>
#include <sys/Types.h>
#include <fcntl.h>


// C++ STL headers
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <deque>
#include <set>
#include <string>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <numeric>
#include <typeinfo>
#include <bitset>
#include <thread>
#include <mutex>
#include <chrono>
#if defined(UNIX)
//	#include <parallel/algorithm>	// __gnu_parallel::sort() + compiler : -fopenmp
#endif


// EXTERNAL LIBRARIES HEADERS
//#include <omp.h>
// #include "win32_compat.h"			// provides Windows with missing ISO C standard functions (for portability)
// #include "linux_compat.h"			// provides Linux with Win32 C functions (for portability)


/****************************************************************************\
*                                                                            *
*  typedef and compiler check                                                *
*                                                                            *
\****************************************************************************/

///////////////////////
// 32-64 bits checks //
///////////////////////
#if defined(__x86_64__) || defined(_M_X64)  || defined(__LP64__) || defined(__POWERPC64__) || defined(_M_IA64)
	#define PLATFORM_NB_BITS 64
#elif defined(__i386__) || defined(_M_IX86) || defined(_M_PPC)   || defined(__POWERPC__)   || defined(__LP32__) || defined(ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__ARM_ARCH_6__)
	#define PLATFORM_NB_BITS 32
#else
	#error Define your platform ! (see Types.h)
#endif


#if defined(__GNUC__)

	/////////////////////////
	// COMPILER IS GCC/G++ //
	/////////////////////////

	#if ((__GNUC__ >= 3) && (__GNUC_MINOR__ >= 4)) || (__GNUC__ >= 4)
		// almost C99 compiler, it should be ok.
		#define __FUNCTION_NAME__	__FUNCTION__
	#else
		#error Compiler not C99 compliant. g++ 3.4 or better required
	#endif
	
#elif defined(_MSC_VER)

	//////////////////////
	// COMPILER IS VC++ //
	//////////////////////

	#if (_MSC_VER >= 1300)
		// almost C99 compiler, it should be ok.
		#define __FUNCTION_NAME__	__FUNCTION__ 
		//__FUNCSIG__
	#else
		#error Compiler not C99 compliant. Visual C++ 2003 (7.1) or better required !
	#endif

#else

	//////////////////////
	// UNKNOWN COMPILER //
	//////////////////////
	
	#if defined(__STDC_VERSION__)
		#if (__STDC_VERSION__ >= 199901L)
			// unknown C99 compiler, it should be ok.
			#define __FUNCTION_NAME__	__FUNCTION__
		#else
			#error Compiler not C99 compliant !
		#endif
	#else
		// we can't check, just pray
		#define __FUNCTION_NAME__	__FUNCTION__
	#endif

#endif



// Définition de MY_ASSERT(x)
#if defined(NDEBUG)
	#define MY_ASSERT(x) (void(0))
#else
	#define VALUE2STRING(x) #x						// Do not use.
	#define VALUE_TO_STRING(x) VALUE2STRING(x)		// Use this one.
	#if defined(__GNUC__)
		#define MY_ASSERT(x) if (!(x)) { printf("\nAssertion failed: (%s), function %s, file %s, line %d.\n", VALUE_TO_STRING(x), __PRETTY_FUNCTION__, __FILE__, __LINE__); abort(); }
	#elif defined(_MSC_VER)
		#define MY_ASSERT(x) if (!(x)) { printf("\nAssertion failed: (%s), function %s, file %s, line %d.\n", VALUE_TO_STRING(x), __FUNCSIG__,         __FILE__, __LINE__); abort(); }
	#else
		#error
	#endif
#endif

/*
// Some test definition here
#define DEFINED_BUT_NO_VALUE
#define DEFINED_INT 3
#define DEFINED_STR "ABC"

// definition to expand macro then apply to pragma message
#define VAR_NAME_VALUE(var) #var "="  VALUE_TO_STRING(var)

// Some example here
#pragma message VAR_NAME_VALUE(NOT_DEFINED)
#pragma message VAR_NAME_VALUE(DEFINED_BUT_NO_VALUE)
#pragma message VAR_NAME_VALUE(DEFINED_INT)
#pragma message VAR_NAME_VALUE(DEFINED_STR)

#pragma message(VAR_NAME_VALUE(WCHAR_MAX))

gcc -dM -E - < /dev/null
*/

// ensure types have the correct size : (must include <limits.h> & <wchar.h>)
#if UCHAR_MAX != 255
	#error [Types.h] char type : UCHAR_MAX must be 255
#elif USHRT_MAX != 65535
	#error [Types.h] short type : USHRT_MAX must be 65535
#elif UINT_MAX != 4294967295U
	#error [Types.h] int type : UINT_MAX must be 4294967295U
#elif ULLONG_MAX != 18446744073709551615ULL
	#error [Types.h] long long type : ULLONG_MAX must be 18446744073709551615ULL
#endif


// REQUIRE C++11 compliant compiler
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t 	s64;


#define sizeofarray(array) (sizeof(array) / sizeof(array[0]))
#define sizeofmember(type, member) sizeof(((type *)0)->member)

//#if !defined(UNICODE)
//	#error You must define globally UNICODE macro ! (see Types.h) 
//	// In Visual Studio, set "Use Unicode Character Set" in "Character set" in 
//	// the menu "Configuration properties", it will define UNICODE automatically.
//	// On Linux, define the macro in a Makefile.
//#endif

// Are you serious brudah??
using namespace std;


/****************************************************************************\
*                                                                            *
*    Les #pragma                                                             *
*                                                                            *
\****************************************************************************/

#if defined(WIN32)

//	#pragma warning(disable : 4786)	// disable warnings about long names (stl)
//	#pragma warning(disable : 4996)	// disable warning "Calling potentially unsafe method" (macro _SCL_SECURE_NO_WARNINGS)
	#pragma warning(disable : 4099)	// disable "no .pdb warning"
	#pragma warning(disable : 4049)	// locally defined symbol Func() imported
//	#pragma warning(disable : 4221)	// no public symbol found
/*
	// LINK WITH THESE LIBRARIES (use a Makefile for Linux) :
	#if defined(DEBUG)
		#pragma comment(lib, "libxml2-win64-vc141-shared_D.lib")		// libxml2  library
		#pragma comment(lib, "zlib-win64-vc141-static_D.lib")			// zLib (static version)
		#pragma comment(lib, "shapelib-win64-vc141-static_D.lib")		// shapelib
	#else
		#pragma comment(lib, "libxml2-win64-vc141-shared.lib")			// libxml2  library
		#pragma comment(lib, "zlib-win64-vc141-static.lib")				// zLib (static version)
		#pragma comment(lib, "shapelib-win64-vc141-static.lib")			// shapelib
	#endif
*/
#elif defined(UNIX)

	// nothing special

#endif



/****************************************************************************\
*                                                                            *
*  Per system #define (AFTER INCLUDES)                                       *
*                                                                            *
\****************************************************************************/

#if defined(UNIX)
#elif defined(WIN32)
#endif


////////////////
// Math macro //
////////////////

#ifndef M_PI			// PI
	#define M_PI		3.1415926535897932384626433832795
	#define M_PIf		3.1415926535897932384626433832795f
#endif
#ifndef M_PI_2			// PI/2
	#define M_PI_2		1.5707963267948966192313216916398
	#define M_PI_2f		1.5707963267948966192313216916398f
#endif
#ifndef M_2_PI			// 2.PI
	#define M_2_PI		6.283185307179586476925286766559
	#define M_2_PIf		6.283185307179586476925286766559f
#endif
#ifndef M_PI_180		// PI/180
	#define M_PI_180	0.01745329251994329576923690768489		// degree * M_PI_180 -> radian
	#define M_PI_180f	0.01745329251994329576923690768489f		// degree * M_PI_180 -> radian
#endif
#ifndef M_180_PI		// 180/PI
	#define M_180_PI	57.295779513082320876798154814105		// radian * M_180_PI -> degree
	#define M_180_PIf	57.295779513082320876798154814105f		// radian * M_180_PI -> degree
#endif
#ifndef M_PI_360		// PI/360
	#define M_PI_360	0.00872664625997164788461845384244
	#define M_PI_360f	0.00872664625997164788461845384244f
#endif
#ifndef M_EXP_1			// e^1 (<=> exp(1.0))
	#define M_EXP_1		2.7182818284590452353602874713527
	#define M_EXP_1f	2.7182818284590452353602874713527f
#endif
#ifndef M_LOG_2			// ln(2) (<=> log(2.0))
	#define M_LOG_2		0.69314718055994530941723212145818
	#define M_LOG_2f	0.69314718055994530941723212145818f
#endif