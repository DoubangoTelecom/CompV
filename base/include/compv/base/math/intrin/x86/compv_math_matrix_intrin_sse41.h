/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_MATH_MATRIX_INTRIN_SSE41_H_)
#define _COMPV_BASE_MATH_MATRIX_INTRIN_SSE41_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

#if COMPV_ARCH_X86 && COMPV_INTRINSIC

COMPV_NAMESPACE_BEGIN()

void COMPV_DEPRECATED(CompVMathMatrixMulABt_64f_Intrin_SSE41)(const COMPV_ALIGNED(SSE) compv_float64_t* A, compv_uscalar_t aRows, COMPV_ALIGNED(SSE) compv_uscalar_t aStrideInBytes, const COMPV_ALIGNED(SSE) compv_float64_t* B, compv_uscalar_t bRows, compv_uscalar_t bCols, COMPV_ALIGNED(SSE) compv_uscalar_t bStrideInBytes, COMPV_ALIGNED(SSE) compv_float64_t* R, COMPV_ALIGNED(SSE) compv_uscalar_t rStrideInBytes);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */

#endif /* _COMPV_BASE_MATH_MATRIX_INTRIN_SSE41_H_ */
