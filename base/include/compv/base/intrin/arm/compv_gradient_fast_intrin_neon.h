/* Copyright (C) 2016-2018 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_GRADIENT_FAST_INTRIN_NEON_H_)
#define _COMPV_BASE_GRADIENT_FAST_INTRIN_NEON_H_

#include "compv/base/compv_config.h"
#if COMPV_ARCH_ARM && COMPV_INTRINSIC
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

void CompVGradientFastGradX_8u16s_Intrin_NEON(COMPV_ALIGNED(NEON) const uint8_t* input, COMPV_ALIGNED(NEON) int16_t* dx, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);
void CompVGradientFastGradX_8u32f_Intrin_NEON(COMPV_ALIGNED(NEON) const uint8_t* input, COMPV_ALIGNED(NEON) compv_float32_t* dx, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);
void CompVGradientFastGradX_32f32f_Intrin_NEON(COMPV_ALIGNED(NEON) const compv_float32_t* input, COMPV_ALIGNED(NEON) compv_float32_t* dx, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);
void CompVGradientFastGradY_8u16s_Intrin_NEON(COMPV_ALIGNED(NEON) const uint8_t* input, COMPV_ALIGNED(NEON) int16_t* dy, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);
void CompVGradientFastGradY_8u32f_Intrin_NEON(COMPV_ALIGNED(NEON) const uint8_t* input, COMPV_ALIGNED(NEON) compv_float32_t* dy, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);
void CompVGradientFastGradY_32f32f_Intrin_NEON(COMPV_ALIGNED(NEON) const compv_float32_t* input, COMPV_ALIGNED(NEON) compv_float32_t* dy, compv_uscalar_t width, compv_uscalar_t height, COMPV_ALIGNED(NEON) compv_uscalar_t stride);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_ARM && COMPV_INTRINSIC */

#endif /* _COMPV_BASE_GRADIENT_FAST_INTRIN_NEON_H_ */
