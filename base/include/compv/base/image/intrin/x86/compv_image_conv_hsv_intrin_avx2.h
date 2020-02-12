/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_IMAGE_CONV_HSV_INTRIN_AVX2_H_)
#define _COMPV_BASE_IMAGE_CONV_HSV_INTRIN_AVX2_H_

#include "compv/base/compv_config.h"
#if COMPV_ARCH_X86 && COMPV_INTRINSIC
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

void CompVImageConvRgb24ToHsv_Intrin_AVX2(COMPV_ALIGNED(AVX) const uint8_t* rgb24Ptr, COMPV_ALIGNED(AVX) uint8_t* hsvPtr, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(AVX) compv_uscalar_t stride);

void CompVImageConvRgba32ToHsv_Intrin_AVX2(COMPV_ALIGNED(AVX) const uint8_t* rgba32Ptr, COMPV_ALIGNED(AVX) uint8_t* hsvPtr, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(AVX) compv_uscalar_t stride);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */

#endif /* _COMPV_BASE_IMAGE_CONV_HSV_INTRIN_AVX2_H_ */
