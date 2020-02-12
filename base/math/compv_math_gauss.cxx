/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/compv_math_gauss.h"

COMPV_NAMESPACE_BEGIN()

COMPV_ERROR_CODE CompVMathGauss::kernelDim1FixedPoint(CompVMatPtrPtr fixedPointKernel, size_t size, float sigma)
{
	CompVMatPtr normalizedKernel;
	COMPV_CHECK_CODE_RETURN(CompVMathGauss::kernelDim1<compv_float32_t>(&normalizedKernel, size, sigma));
	COMPV_CHECK_CODE_RETURN(CompVMathConvlt::fixedPointKernel(normalizedKernel, fixedPointKernel));
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_NAMESPACE_END()
