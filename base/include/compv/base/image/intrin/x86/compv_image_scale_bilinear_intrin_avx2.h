/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_IMAGE_SCALE_BILINEAR_INTRIN_AVX2_H_)
#define _COMPV_BASE_IMAGE_SCALE_BILINEAR_INTRIN_AVX2_H_

#include "compv/base/compv_config.h"
#if COMPV_ARCH_X86 && COMPV_INTRINSIC
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

void CompVImageScaleBilinear_Intrin_AVX2(
	const uint8_t* inPtr, compv_uscalar_t inStride,
	COMPV_ALIGNED(AVX) uint8_t* outPtr, compv_uscalar_t outWidth, compv_uscalar_t outYStart, compv_uscalar_t outYEnd, COMPV_ALIGNED(AVX) compv_uscalar_t outStride,
	compv_uscalar_t sf_x, compv_uscalar_t sf_y);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */

#endif /* _COMPV_BASE_IMAGE_SCALE_BILINEAR_INTRIN_AVX2_H_ */
