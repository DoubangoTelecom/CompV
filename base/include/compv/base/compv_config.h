/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#ifndef _COMPV_BASE_CONFIG_H_
#define _COMPV_BASE_CONFIG_H_

// Platforms info http://sourceforge.net/p/predef/wiki/Home/

// Windows (XP/Vista/7/CE and Windows Mobile) macro definition.
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN16) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#	define COMPV_OS_WINDOWS	1
#	if defined(_WIN32_WCE) || defined(UNDER_CE)
#		define COMPV_OS_WINDOWS_CE		1
#		define COMPV_STDCALL			__cdecl
#	else
#		define COMPV_STDCALL			__stdcall
#	endif
#	if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || WINAPI_FAMILY == WINAPI_FAMILY_APP)
#		define COMPV_OS_WINDOWS_RT		1
#	endif
#else
#	define COMPV_STDCALL
#endif
// OS: OSX or iOS
#if defined(__APPLE__)
#   include <TargetConditionals.h>
#   include <Availability.h>
#	define COMPV_OS_APPLE				1
#endif
#if TARGET_OS_MAC || (defined (macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__)))
#	define COMPV_OS_OSX					1
#endif
#if TARGET_OS_IPHONE
#	define COMPV_OS_IPHONE				1
#endif
#if TARGET_IPHONE_SIMULATOR
#	define COMPV_OS_IPHONE_SIMULATOR	1
#endif
// OS: Android
#if defined(__ANDROID__) || defined(ANDROID)
#	define COMPV_OS_ANDROID				1
#	include <android/api-level.h>
#endif
// OS: BSD
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#	define COMPV_OS_BSD					1
#endif
// OS: Cygwin
#if defined(__CYGWIN__)
#	define COMPV_OS_CYGWIN				1
#endif
// OS: GNU Linix
#if defined(__gnu_linux__)
#	define COMPV_OS_GNU_LINUX			1
#endif
// OS: Linux
#if defined(__linux__)
#	define COMPV_OS_LINUX				1
#endif
// OS: MS DOS
#if defined(MSDOS) || defined(__MSDOS__) || defined(_MSDOS) || defined(__DOS__)
#	define COMPV_OS_MSDOS				1
#endif
// OS: NetBSD
#if defined(__NetBSD__)
#	define COMPV_OS_NETBSD				1
#endif
// OS: OpenBSD
#if defined(__OpenBSD__)
#	define COMPV_OS_OPENBSD				1
#endif
// OS: OS/2
#if defined(OS2) || defined(_OS2) || defined(__OS2__) || defined(__TOS_OS2__)
#	define COMPV_OS_OS2					1
#endif
// OS: Solaris
#if defined(sun) || defined(__sun)
#	define COMPV_OS_SOLARIS				1
#endif
// OS: Symbian32
#if defined(__SYMBIAN32__)
#	define COMPV_OS_SYMBIAN32			1
#endif
// OS: Unix
#if defined(__unix__) || defined(__unix)
#	define COMPV_OS_UNIX				1
#endif
// OS: VxWorks
#if defined(__VXWORKS__) || defined(__vxworks)
#	define COMPV_OS_VXWORKS				1
#endif

// ARCH: Alpha
#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
#	define COMPV_ARCH_ALPHA				1
#endif
// ARCH: AMD64
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
#	define COMPV_ARCH_X64_AMD64			1
#endif
// ARCH:ARM
#if defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm) || defined(__aarch64__)
#	define COMPV_ARCH_ARM				1
#endif
// ARCH: ARM64
#if defined(__aarch64__)
#	define COMPV_ARCH_ARM64				1
#endif
// ARCH: ARM32
#define COMPV_ARCH_ARM32 (COMPV_ARCH_ARM && !COMPV_ARCH_ARM64)
// ARCH: Blackfin
#if defined(__bfin) || defined(__BFIN__)
#	define COMPV_ARCH_BLACKFIN			1
#endif
// ARCH: Convex
#if defined(__convex__)
#	define COMPV_ARCH_CONVEX			1
#endif
// ARCH: Epiphany
#if defined(__epiphany__)
#	define COMPV_ARCH_EPIPHANY			1
#endif
// ARCH: HP/PA RISC
#if defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
#	define COMPV_ARCH_HPPA				1
#endif
// ARCH: Intel x86
#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__i386) || defined(__i386) || defined(__IA32__) || defined(_M_I86) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386)
#	define COMPV_ARCH_X86_INTEL			1
#endif
// ARCH: Intel Itanium (IA-64)
#if defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) || defined(_M_IA64) || defined(__itanium__)
#	define COMPV_ARCH_X64_INTEL_IA64	1
#endif
// ARCH: Motorola 68k
#if defined(__m68k__) || defined(M68000) || defined(__MC68K__)
#	define COMPV_ARCH_MOTO68K			1
#endif
// ARCH: MIPS
#if defined(__mips__) || defined(mips) || defined(__mips) || defined(__MIPS__)
#	define COMPV_ARCH_MIPS				1
#endif
// ARCH: PowerPC
#if defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(__ppc)
#	define	COMPV_ARCH_POWERPC			1
#endif
// ARCH: Pyramid 9810
#if defined(pyr)
#	define COMPV_ARCH_PYRAMID9810		1
#endif
// ARCH: RS/6000
#if defined(__THW_RS6000) || defined(_IBMR2) || defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || defined(_ARCH_PWR4)
#	define COMPV_ARCH_RS6000			1
#endif
// ARCH: SPARC
#if defined(__sparc__) || defined(__sparc)
#	define COMPV_ARCH_SPARC				1
#endif
// ARCH: SuperH
#if defined(__sh__)
#	define COMPV_ARCH_SUPERH			1
#endif
// ARCH: SystemZ
#if defined(__370__) || defined(__THW_370__) || defined(__s390__) || defined(__s390x__) || defined(__zarch__) || defined(__SYSC_ZARCH__)
#	define	COMPV_ARCH_SYSTEMZ			1
#endif
// ARCH: TMS320
#if defined(_TMS320C2XX) || defined(__TMS320C2000__) || defined(_TMS320C5X) || defined(__TMS320C55X__) || defined(_TMS320C6X) || defined(__TMS320C6X__)
#	define	COMPV_ARCH_TMS320			1
#endif
// ARCH: TMS470
#if defined(__TMS470__)
#	define COMPV_ARCH_TMS470			1
#endif


// ARCH: All X86
#if COMPV_ARCH_X64_AMD64 || COMPV_ARCH_X64_INTEL_IA64 || COMPV_ARCH_X86_INTEL || defined(__i386__)
#	define COMPV_ARCH_X86				1
#endif
// ARCH: All X64
#if COMPV_ARCH_X64_AMD64 || COMPV_ARCH_X64_INTEL_IA64 || defined (__X86_64__)
#	define COMPV_ARCH_X64				1
#endif

// ASM / INTRINSIC enable/disable
#if !defined(COMPV_ASM)
#	define COMPV_ASM                       1
#endif /* COMPV_ASM */
#if !defined(COMPV_INTRINSIC)
#	define COMPV_INTRINSIC                 1
#endif /* COMPV_INTRINSIC */

// Intel TBBMalloc
#define  COMPV_TBBMALLOC				1

// Disable some well-known warnings
#if defined(_MSC_VER)
#	define COMPV_VS_DISABLE_WARNINGS_BEGIN(nn) \
		__pragma(warning( push )) \
		__pragma(warning( disable : ##nn ))
#	define COMPV_VS_DISABLE_WARNINGS_END() \
		__pragma(warning( pop ))
#else
#	define COMPV_VS_DISABLE_WARNINGS_BEGIN(nn)
#	define COMPV_VS_DISABLE_WARNINGS_END()
#endif /* _MSC_VER */
#if defined(__GNUC__)
#	define COMPV_GCC_DO_PRAGMA(x) _Pragma (#x)
#	define COMPV_GCC_DISABLE_WARNINGS_BEGIN(w) \
		COMPV_GCC_DO_PRAGMA(GCC diagnostic push) \
		COMPV_GCC_DO_PRAGMA(GCC diagnostic ignored w)
#	define COMPV_GCC_DISABLE_WARNINGS_END() \
		COMPV_GCC_DO_PRAGMA(GCC diagnostic pop)
#else
#	define COMPV_GCC_DISABLE_WARNINGS_BEGIN(tt)
#	define COMPV_GCC_DISABLE_WARNINGS_END()
#endif /* __GNUC__ */

// Hidden symbols (used on ASM code to avoid export outside the shared lib)
#if defined(__GNUC__)
#	define COMPV_GCC_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))
#else
#	define COMPV_GCC_VISIBILITY_HIDDEN
#endif

// Guards against C++ name mangling
#if defined(__cplusplus)
#	define COMPV_EXTERNC_BEGIN() extern "C" {
#	define COMPV_EXTERNC_END()	}
#else
#	define COMPV_EXTERNC_BEGIN()
#	define COMPV_EXTERNC_END()
#endif

#if defined(__cplusplus)
#   define COMPV_EXTERNC extern "C"
#else
#   define COMPV_EXTERNC
#endif

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) || (__cplusplus > 199711L)
#	define COMPV_CPP11              1
#endif


// Windows's symbols export
#if COMPV_OS_WINDOWS && !defined(COMPV_STATIC)
#	if defined(COMPV_BASE_EXPORTS)
# 		define COMPV_BASE_API		__declspec(dllexport)
#	else
# 		define COMPV_BASE_API		__declspec(dllimport)
#	endif
#else
# 	define COMPV_BASE_API
#endif

/* define "COMPV_DEPRECATED(func)" macro */
#if defined(__GNUC__)
#	define COMPV_DEPRECATED(func) __attribute__ ((deprecated)) func
#elif defined(_MSC_VER)
#	define COMPV_DEPRECATED(func) __declspec(deprecated) func
#else
#	pragma message("WARNING: Deprecated not supported for this compiler")
#	define COMPV_DEPRECATED(func) func
#endif

// namespace (you can update the namespace using CFLAGS+=-DCOMPV_NAMESPACE=YourNameSpace)
#if !defined COMPV_NAMESPACE
#	define COMPV_NAMESPACE compv
#endif
#if !defined(COMPV_NAMESPACE_BEGIN)
#	define COMPV_NAMESPACE_BEGIN() namespace COMPV_NAMESPACE {
#endif
#if !defined(COMPV_NAMESPACE_END)
#	define COMPV_NAMESPACE_END() }
#endif

#define COMPV_DEFAULT(v)
#define COMPV_CONDITION(c)

#define COMPV_OVERRIDE_DECL(base_class) override
#define COMPV_OVERRIDE_IMPL(base_class)
#define COMPV_OVERRIDE_IMPL0(base_class, method) COMPV_ERROR_CODE method
#define COMPV_OVERRIDE_IMPL1(base_class, ret_type, method) ret_type method
#define COMPV_OVERRIDE_DECL0(base_class, method) virtual COMPV_ERROR_CODE method
#define COMPV_OVERRIDE_DECL1(base_class, ret_type, method) virtual ret_type method

#if defined(_MSC_VER)
#	if !defined(_CRT_SECURE_NO_WARNINGS)
#	define _CRT_SECURE_NO_WARNINGS
#	endif
#	define COMPV_SHOULD_INLINE		__inline
#	define COMPV_ALWAYS_INLINE		__forceinline
#	define COMPV_ALIGN(x)			__declspec(align(x))
#	define COMPV_NAKED				__declspec(naked)
#	define COMPV_INLINE				_inline
#	define COMPV_TEMPLATE_EXTERN	extern template
#	pragma warning( disable : 4996 )
#	if _MSC_VER >= 1700
// Warning	3	warning C4752: found Intel(R) Advanced Vector Extensions; consider using /arch:AVX
#		pragma warning(disable: 4752)
#	endif
#elif defined(__GNUC__)
#	define COMPV_ALWAYS_INLINE		__inline __attribute__ ((__always_inline__))
#	define COMPV_SHOULD_INLINE		inline
#	define COMPV_ALIGN(x)			__attribute__((aligned(x)))
#	define COMPV_NAKED				__attribute__((naked))
#	define COMPV_INLINE				inline
#	define COMPV_TEMPLATE_EXTERN	template< >
#else
#	define COMPV_ALWAYS_INLINE		inline
#	define COMPV_SHOULD_INLINE		inline
#	define COMPV_INLINE				inline
#	define COMPV_ALIGN(x)			__attribute__((aligned(x)))
#	define COMPV_TEMPLATE_EXTERN	template< >
#endif

// SIMD Alignment
#define COMPV_ALIGNV_SIMD_MMX		8
#define COMPV_ALIGNV_SIMD_SSE		16
#define COMPV_ALIGNV_SIMD_AVX		32
#define COMPV_ALIGNV_SIMD_AVX2		COMPV_ALIGNV_SIMD_AVX
#define COMPV_ALIGNV_SIMD_AVX512	64
#define COMPV_ALIGNV_SIMD_NEON		16 // ARM NEON
#define COMPV_ALIGNV_SIMD_DEFAULT	64 // This is max to make sure all requirements will work (also equal to expected cache line)

// GPU Alignment
#define COMPV_ALIGNV_GPU_PAGE		4096 // In Bytes
#define COMPV_ALIGNV_GPU_LINE		64 // In Bytes

// Default cache line
#if !defined(COMPV_CACHE1_LINE_SIZE)
#	define COMPV_CACHE1_LINE_SIZE	64 // In Bytes
#endif
#if !defined(COMPV_PREFETCH_DISTANCE)
#	if COMPV_ARCH_ARM
#		define COMPV_PREFETCH_DISTANCE (COMPV_CACHE1_LINE_SIZE * 3) // In Bytes
#	else
#		define COMPV_PREFETCH_DISTANCE (COMPV_CACHE1_LINE_SIZE << 2) // In Bytes
#	endif
#endif

// Default Phys mem size (RAM)
#if !defined(COMPV_PHYS_MEM_SIZE)
#	define COMPV_PHYS_MEM_SIZE	(1024 << 20) // In Bytes -> 1GB
#endif

// Default heap limit (used ny tbbMalloc)
#if !defined(COMPV_HEAP_LIMIT)
#	define COMPV_HEAP_LIMIT	(256 << 20) // In Bytes -> 256MB
#endif

#if !defined (HAVE_GETTIMEOFDAY)
#if COMPV_OS_WINDOWS
#	define HAVE_GETTIMEOFDAY				0
#else
#	define HAVE_GETTIMEOFDAY				1
#endif
#endif /* HAVE_GETTIMEOFDAY */

#if defined(COMPV_OS_ANDROID)
#	define HAVE_CLOCK_GETTIME				1
#endif

// LightGBM
#if !defined(USE_SOCKET) && !defined(USE_MPI)
#	define USE_SOCKET
#endif

// Semaphore type: CPP11 or CPPstd
#if !defined(COMPV_PARALLEL_SEMA11)
#	define COMPV_PARALLEL_SEMA11 0 // use C++11 Semophores ?
#endif

// Thread affinity setting
#if !defined (COMPV_PARALLEL_THREAD_SET_AFFINITY)
#	define COMPV_PARALLEL_THREAD_SET_AFFINITY 0 // set thread affinity after starting?
#endif

// Intel libraries
#if defined(__INTEL_COMPILER) || defined(HAVE_IPP) // TODO(dmi): not correct, having Intel compiler doesn't means we have IPP
#	define COMPV_HAVE_INTEL_IPP		1
#endif
#if defined(__INTEL_COMPILER) || defined(HAVE_TBB) // TODO(dmi): not correct, having Intel compiler doesn't means we have TBB
#	define COMPV_HAVE_INTEL_TBB		1
#endif

#if COMPV_OS_WINDOWS
#	if !defined(_WINSOCKAPI_)
#	define _WINSOCKAPI_
#	endif
#	if !defined(NOMINMAX)
#	define NOMINMAX
#	endif
#	include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <string>
#include <cstdlib>

// Must be at the bottom to make sure we can redefine all macros
#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

//!\\ DO NOT ADD ANY def after this line (after "include<config.h>")


#if COMPV_INTRINSIC
#	if COMPV_ARCH_X86
#		if defined(_MSC_VER)
#			include <intrin.h>
#			pragma intrinsic(_BitScanForward, _bittestandreset, _InterlockedIncrement, _InterlockedDecrement)
#			if COMPV_ARCH_X64
#			pragma intrinsic(_bittestandreset64, _BitScanForward64)
#			endif
#		elif defined(__GNUC__)
#			include <x86intrin.h>
#		endif
#	elif COMPV_ARCH_ARM
#		include <arm_neon.h>
#	endif
#endif

#if COMPV_TBBMALLOC
#	define __TBB_SOURCE_DIRECTLY_INCLUDED	1
#	define __TBBMALLOC_BUILD				1
#endif

#endif /* _COMPV_BASE_CONFIG_H_ */
