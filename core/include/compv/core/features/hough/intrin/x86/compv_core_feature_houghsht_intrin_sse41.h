/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_CORE_FEATURE_HOUGHSHT_INTRIN_SSE41_H_)
#define _COMPV_CORE_FEATURE_HOUGHSHT_INTRIN_SSE41_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

#if COMPV_ARCH_X86 && COMPV_INTRINSIC

COMPV_NAMESPACE_BEGIN()

void CompVHoughShtAccGatherRow_4mpd_Intrin_SSE41(COMPV_ALIGNED(SSE) const int32_t* pCosRho, COMPV_ALIGNED(SSE) const int32_t* pRowTimesSinRho, compv_uscalar_t col, int32_t* pACC, compv_uscalar_t accStride, compv_uscalar_t maxTheta);

void CompVHoughShtRowTimesSinRho_Intrin_SSE41(COMPV_ALIGNED(SSE) const int32_t* pSinRho, COMPV_ALIGNED(SSE) compv_uscalar_t row, COMPV_ALIGNED(SSE) int32_t* rowTimesSinRhoPtr, compv_uscalar_t count);

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */

#endif /* _COMPV_CORE_FEATURE_HOUGHSHT_INTRIN_SSE41_H_ */
