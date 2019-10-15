/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_CORE_SIMD_GLOBALS_H_)
#define _COMPV_CORE_SIMD_GLOBALS_H_

#include "compv/core/compv_core_config.h"
#include "compv/core/compv_core_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

#if COMPV_ARCH_X86

COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_16s7_32s[];

COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL0_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL1_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL2_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL3_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL4_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL5_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL6_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL7_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL8_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL9_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL10_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL11_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL12_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL13_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL14_8u_32s[];
COMPV_EXTERNC COMPV_CORE_API COMPV_ALIGN_DEFAULT() int32_t kShuffleEpi8_DUP_SHL15_8u_32s[];

#endif /* COMPV_ARCH_X86 */

#endif /* _COMPV_CORE_SIMD_GLOBALS_H_ */
