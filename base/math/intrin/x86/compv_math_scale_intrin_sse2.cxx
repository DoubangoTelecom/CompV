/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/intrin/x86/compv_math_scale_intrin_sse2.h"

#if COMPV_ARCH_X86 && COMPV_INTRINSIC
#include "compv/base/compv_debug.h"

COMPV_NAMESPACE_BEGIN()

// Must not require memory alignment (random access from SVM)
void CompVMathScaleScale_64f64f_Intrin_SSE2(const compv_float64_t* ptrIn, compv_float64_t* ptrOut, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t stride, const compv_float64_t* s1)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("See ASM code with support for AVX");

	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width2 = width & -2;
	const __m128d vecScale = _mm_set1_pd(*s1); // _mm_shuffle_pd(_mm_load_sd(s1), 0x00)
	compv_uscalar_t i;

	for (compv_uscalar_t j = 0; j < height; ++j) {
		for (i = 0; i < width16; i += 16) { // test "width16, width16"
			_mm_storeu_pd(&ptrOut[i], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 2], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 2]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 4], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 4]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 6], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 6]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 8], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 8]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 10], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 10]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 12], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 12]), vecScale));
			_mm_storeu_pd(&ptrOut[i + 14], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i + 14]), vecScale));
		}
		for (; i < width2; i += 2) { // not "test width2, width2" but "cmp i, width2"
			_mm_storeu_pd(&ptrOut[i], _mm_mul_pd(_mm_loadu_pd(&ptrIn[i]), vecScale));
		}
		for (; i < width; i += 1) {
			_mm_store_sd(&ptrOut[i], _mm_mul_sd(_mm_load_sd(&ptrIn[i]), vecScale));
		}
		ptrIn += stride;
		ptrOut += stride;
	}
}

void CompVMathScaleScale_32f32f_Intrin_SSE2(COMPV_ALIGNED(SSE) const compv_float32_t* ptrIn, COMPV_ALIGNED(SSE) compv_float32_t* ptrOut, const compv_uscalar_t width, const compv_uscalar_t height, COMPV_ALIGNED(SSE) const compv_uscalar_t stride, const compv_float32_t* s1)
{
	COMPV_DEBUG_INFO_CHECK_SSE2();
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("See ASM code with support for AVX");

	const compv_uscalar_t width32 = width & -32;
	const compv_uscalar_t width4 = width & -4;
	const __m128 vecScale = _mm_set1_ps(*s1);
	compv_uscalar_t i;

	for (compv_uscalar_t j = 0; j < height; ++j) {
		for (i = 0; i < width32; i += 32) { // test "width16, width16"
			_mm_store_ps(&ptrOut[i], _mm_mul_ps(_mm_load_ps(&ptrIn[i]), vecScale));
			_mm_store_ps(&ptrOut[i + 4], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 4]), vecScale));
			_mm_store_ps(&ptrOut[i + 8], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 8]), vecScale));
			_mm_store_ps(&ptrOut[i + 12], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 12]), vecScale));
			_mm_store_ps(&ptrOut[i + 16], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 16]), vecScale));
			_mm_store_ps(&ptrOut[i + 20], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 20]), vecScale));
			_mm_store_ps(&ptrOut[i + 24], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 24]), vecScale));
			_mm_store_ps(&ptrOut[i + 28], _mm_mul_ps(_mm_load_ps(&ptrIn[i + 28]), vecScale));
		}
		for (; i < width4; i += 4) { // not "test width4, width4" but "cmp i, width4"
			_mm_store_ps(&ptrOut[i], _mm_mul_ps(_mm_load_ps(&ptrIn[i]), vecScale));
		}
		for (; i < width; i += 1) {
			_mm_store_ss(&ptrOut[i], _mm_mul_ss(_mm_load_ss(&ptrIn[i]), vecScale));
		}
		ptrIn += stride;
		ptrOut += stride;
	}
}

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_X86 && COMPV_INTRINSIC */
