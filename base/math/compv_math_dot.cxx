/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/compv_math_dot.h"
#include "compv/base/compv_generic_invoke.h"
#include "compv/base/parallel/compv_parallel.h"
#include "compv/base/compv_cpu.h"

#include "compv/base/math/intrin/x86/compv_math_dot_intrin_sse2.h"
#include "compv/base/math/intrin/x86/compv_math_dot_intrin_avx.h"
#include "compv/base/math/intrin/arm/compv_math_dot_intrin_neon.h"

COMPV_NAMESPACE_BEGIN()

#if COMPV_ASM && COMPV_ARCH_X64
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_X64_SSE2(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_X64_SSE2(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_X64_AVX(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_X64_AVX(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_X64_FMA3_AVX(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_X64_FMA3_AVX(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
#endif /* COMPV_ASM && COMPV_ARCH_X64 */

#if COMPV_ASM && COMPV_ARCH_ARM32
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_NEON32(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_NEON32(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_FMA_NEON32(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_FMA_NEON32(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
#endif /* COMPV_ASM && COMPV_ARCH_ARM32 */

#if COMPV_ASM && COMPV_ARCH_ARM64
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_NEON64(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_NEON64(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDotSub_64f64f_Asm_FMA_NEON64(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
COMPV_EXTERNC void CompVMathDotDot_64f64f_Asm_FMA_NEON64(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret);
#endif /* COMPV_ASM && COMPV_ARCH_ARM64 */

template<typename T>
static void CompVMathDotDot_C(const T* ptrA, const T* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
{
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPGPU implementation could be found");
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width2 = width & -2;
	compv_float64_t vecSum[4] = { 0, 0, 0, 0 }; // AVX-like vector
	for (compv_uscalar_t j = 0; j < height; ++j) {
		// SIMD-way to have same MD5 as SSE, AVX and NEON code
		compv_uscalar_t i;
		for (i = 0; i < width16; i += 16) {
			// TODO(dmi): SIMD -> no need for "float64_t" to store "vecMul" and "vecAdd"
			// E.g. for "int16_t" we can use "int32_t" as temp storage then, convert to "float64_t" at the end
			// For NEON use "long muliply" and "long add" to fuse conversion and math op
			const compv_float64_t vecMul[16] = {
				(ptrA[i + 0] * ptrB[i + 0]), (ptrA[i + 1] * ptrB[i + 1]), (ptrA[i + 2] * ptrB[i + 2]), (ptrA[i + 3] * ptrB[i + 3]),
				(ptrA[i + 4] * ptrB[i + 4]), (ptrA[i + 5] * ptrB[i + 5]), (ptrA[i + 6] * ptrB[i + 6]), (ptrA[i + 7] * ptrB[i + 7]),
				(ptrA[i + 8] * ptrB[i + 8]), (ptrA[i + 9] * ptrB[i + 9]), (ptrA[i + 10] * ptrB[i + 10]), (ptrA[i + 11] * ptrB[i + 11]),
				(ptrA[i + 12] * ptrB[i + 12]), (ptrA[i + 13] * ptrB[i + 13]), (ptrA[i + 14] * ptrB[i + 14]), (ptrA[i + 15] * ptrB[i + 15]),
			};
			const compv_float64_t vecAdd[8] = {
				(vecMul[0] + vecMul[4]), (vecMul[1] + vecMul[5]), (vecMul[2] + vecMul[6]), (vecMul[3] + vecMul[7]),
				(vecMul[8] + vecMul[12]), (vecMul[9] + vecMul[13]), (vecMul[10] + vecMul[14]), (vecMul[11] + vecMul[15]),
			};
			vecSum[0] += (vecAdd[0] + vecAdd[4]);
			vecSum[1] += (vecAdd[1] + vecAdd[5]);
			vecSum[2] += (vecAdd[2] + vecAdd[6]);
			vecSum[3] += (vecAdd[3] + vecAdd[7]);
		}
		for (; i < width2; i += 2) {
			vecSum[0] += ptrA[i] * ptrB[i];
			vecSum[1] += ptrA[i + 1] * ptrB[i + 1];
		}
		for (; i < width; i += 1) {
			vecSum[0] += ptrA[i] * ptrB[i];
		}

		ptrA += strideA;
		ptrB += strideB;
	}

	vecSum[0] += vecSum[2];
	vecSum[1] += vecSum[3];
	vecSum[0] += vecSum[1];

	*ret = vecSum[0];
}

template<typename T>
static void CompVMathDotDotSub_C(const T* ptrA, const T* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
{
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPGPU implementation could be found");
	const compv_uscalar_t width16 = width & -16;
	const compv_uscalar_t width2 = width & -2;
	compv_float64_t vecSum[4] = { 0, 0, 0, 0 }; // AVX-like vector
	for (compv_uscalar_t j = 0; j < height; ++j) {
		// SIMD-way to have same MD5 as SSE, AVX and NEON code
		compv_uscalar_t i;
		for (i = 0; i < width16; i += 16) {
			// TODO(dmi): SIMD -> no need for "float64_t" to store "vecMul" and "vecAdd"
			// E.g. for "int16_t" we can use "int32_t" as temp storage then, convert to "float64_t" at the end
			// For NEON use "long muliply" and "long add" to fuse conversion and math op
			const compv_float64_t vecDiff[16] = {
				(ptrA[i + 0] - ptrB[i + 0]), (ptrA[i + 1] - ptrB[i + 1]), (ptrA[i + 2] - ptrB[i + 2]), (ptrA[i + 3] - ptrB[i + 3]),
				(ptrA[i + 4] - ptrB[i + 4]), (ptrA[i + 5] - ptrB[i + 5]), (ptrA[i + 6] - ptrB[i + 6]), (ptrA[i + 7] - ptrB[i + 7]),
				(ptrA[i + 8] - ptrB[i + 8]), (ptrA[i + 9] - ptrB[i + 9]), (ptrA[i + 10] - ptrB[i + 10]), (ptrA[i + 11] - ptrB[i + 11]),
				(ptrA[i + 12] - ptrB[i + 12]), (ptrA[i + 13] - ptrB[i + 13]), (ptrA[i + 14] - ptrB[i + 14]), (ptrA[i + 15] - ptrB[i + 15])
			};
			const compv_float64_t vecMul[16] = {
				(vecDiff[0] * vecDiff[0]), (vecDiff[1] * vecDiff[1]), (vecDiff[2] * vecDiff[2]), (vecDiff[3] * vecDiff[3]),
				(vecDiff[4] * vecDiff[4]), (vecDiff[5] * vecDiff[5]), (vecDiff[6] * vecDiff[6]), (vecDiff[7] * vecDiff[7]),
				(vecDiff[8] * vecDiff[8]), (vecDiff[9] * vecDiff[9]), (vecDiff[10] * vecDiff[10]), (vecDiff[11] * vecDiff[11]),
				(vecDiff[12] * vecDiff[12]), (vecDiff[13] * vecDiff[13]), (vecDiff[14] * vecDiff[14]), (vecDiff[15] * vecDiff[15]),
			};
			const compv_float64_t vecAdd[8] = {
				(vecMul[0] + vecMul[4]), (vecMul[1] + vecMul[5]), (vecMul[2] + vecMul[6]), (vecMul[3] + vecMul[7]),
				(vecMul[8] + vecMul[12]), (vecMul[9] + vecMul[13]), (vecMul[10] + vecMul[14]), (vecMul[11] + vecMul[15]),
			};
			vecSum[0] += (vecAdd[0] + vecAdd[4]);
			vecSum[1] += (vecAdd[1] + vecAdd[5]);
			vecSum[2] += (vecAdd[2] + vecAdd[6]);
			vecSum[3] += (vecAdd[3] + vecAdd[7]);
		}
		for (; i < width2; i += 2) {
			const compv_float64_t diff0 = (ptrA[i] - ptrB[i]);
			const compv_float64_t diff1 = (ptrA[i + 1] - ptrB[i + 1]);
			vecSum[0] += diff0 * diff0;
			vecSum[1] += diff1 * diff1;
		}
		for (; i < width; i += 1) {
			const compv_float64_t diff0 = (ptrA[i] - ptrB[i]);
			vecSum[0] += diff0 * diff0;
		}
		ptrA += strideA;
		ptrB += strideB;
	}

	vecSum[0] += vecSum[2];
	vecSum[1] += vecSum[3];
	vecSum[0] += vecSum[1];

	*ret = vecSum[0];
}

template<typename T>
static COMPV_ERROR_CODE CompVMathDotDot(const CompVMatPtr &A, const CompVMatPtr &B, double* ret)
{
	const size_t rows = A->rows();
	const size_t cols = A->cols();
	const size_t strideA = A->stride();
	const size_t strideB = B->stride();
	compv_float64_t sum = 0;

	const size_t threadsCount = CompVThreadDispatcher::guessNumThreadsDividingAcrossY(1, rows, 1);
	std::vector<compv_float64_t > mt_sums(threadsCount - 1);

	auto funcPtr = [&](const size_t start, const size_t end, const size_t threadIdx) -> COMPV_ERROR_CODE {
		COMPV_ASSERT(threadIdx < threadsCount);
		const T* ptrA = A->ptr<const T>(start);
		const T* ptrB = B->ptr<const T>(start);
		compv_float64_t* mt_sum = threadIdx ? &mt_sums[threadIdx - 1] : &sum;
		if (std::is_same<T, compv_float64_t>::value) {
			void(*CompVMathDot_64f64f)(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
				= nullptr;
			COMPV_CHECK_CODE_RETURN(CompVMathDot::hookDot_64f(&CompVMathDot_64f64f));
			CompVMathDot_64f64f(
				reinterpret_cast<const compv_float64_t*>(ptrA), reinterpret_cast<const compv_float64_t*>(ptrB),
				cols, (end - start), strideA, strideB,
				mt_sum
			);
		}
		else {
			CompVMathDotDot_C(
				ptrA, ptrB,
				cols, (end - start), strideA, strideB,
				mt_sum
			);
		}
		return COMPV_ERROR_CODE_S_OK;
	};
	COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
		funcPtr,
		rows,
		threadsCount
	));
	for (std::vector<compv_float64_t >::const_iterator it = mt_sums.begin(); it < mt_sums.end(); ++it) {
		sum += *it;
	}

	*ret = sum;
	return COMPV_ERROR_CODE_S_OK;
}

template<typename T>
static COMPV_ERROR_CODE CompVMathDotDotSub(const CompVMatPtr &A, const CompVMatPtr &B, double* ret)
{
	const size_t rows = A->rows();
	const size_t cols = A->cols();
	const size_t strideA = A->stride();
	const size_t strideB = B->stride();
	compv_float64_t sum;

	const size_t threadsCount = CompVThreadDispatcher::guessNumThreadsDividingAcrossY(1, rows, 1);
	std::vector<compv_float64_t > mt_sums(threadsCount - 1);

	auto funcPtr = [&](const size_t start, const size_t end, const size_t threadIdx) -> COMPV_ERROR_CODE {
		COMPV_ASSERT(threadIdx < threadsCount);
		const T* ptrA = A->ptr<const T>(start);
		const T* ptrB = B->ptr<const T>(start);
		compv_float64_t* mt_sum = threadIdx ? &mt_sums[threadIdx - 1] : &sum;
		if (std::is_same<T, compv_float64_t>::value) {
			void(*CompVMathDotDotSub_64f64f)(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
				= nullptr;
			COMPV_CHECK_CODE_RETURN(CompVMathDot::hookDotSub_64f(&CompVMathDotDotSub_64f64f));
			CompVMathDotDotSub_64f64f(
				reinterpret_cast<const compv_float64_t*>(ptrA), reinterpret_cast<const compv_float64_t*>(ptrB),
				cols, (end - start), strideA, strideB,
				mt_sum
			);
		}
		else {
			CompVMathDotDotSub_C(
				ptrA, ptrB,
				cols, (end - start), strideA, strideB,
				mt_sum
			);
		}
		return COMPV_ERROR_CODE_S_OK;
	};
	COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
		funcPtr,
		rows,
		threadsCount
	));
	for (std::vector<compv_float64_t >::const_iterator it = mt_sums.begin(); it < mt_sums.end(); ++it) {
		sum += *it;
	}

	*ret = sum;
	return COMPV_ERROR_CODE_S_OK;
}

// ret = Dot_product(A, B)
// https://en.wikipedia.org/wiki/Dot_product
COMPV_ERROR_CODE CompVMathDot::dot(const CompVMatPtr &A, const CompVMatPtr &B, double* ret)
{
	COMPV_CHECK_EXP_RETURN(!A || !B || !ret || A->cols() != B->cols() || A->rows() != B->rows() || A->subType() != B->subType() || A->planeCount() != B->planeCount()
		, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	CompVGenericFloatInvokeCodeRawType(A->subType(), CompVMathDotDot, A, B, ret);
	return COMPV_ERROR_CODE_S_OK;
}

// ret = Dot_product((A-B), (A-B))
// https://en.wikipedia.org/wiki/Dot_product
COMPV_ERROR_CODE CompVMathDot::dotSub(const CompVMatPtr &A, const CompVMatPtr &B, double* ret)
{
	COMPV_CHECK_EXP_RETURN(!A || !B || !ret || A->cols() != B->cols() || A->rows() != B->rows() || A->subType() != B->subType() || A->planeCount() != B->planeCount()
		, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	CompVGenericFloatInvokeCodeRawType(A->subType(), CompVMathDotDotSub, A, B, ret);
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVMathDot::hookDotSub_64f(
	void(**CompVMathDotDotSub_64f64f)(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
)
{
	COMPV_CHECK_EXP_RETURN(!CompVMathDotDotSub_64f64f, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_C;
#if COMPV_ARCH_X86
	if (CompVCpu::isEnabled(kCpuFlagSSE2)) {
		COMPV_EXEC_IFDEF_INTRIN_X86(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_X64_SSE2);
	}
	if (CompVCpu::isEnabled(kCpuFlagAVX)) {
		COMPV_EXEC_IFDEF_INTRIN_X86(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Intrin_AVX);
		COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_X64_AVX);
		if (CompVCpu::isEnabled(kCpuFlagFMA3)) {
			COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_X64_FMA3_AVX); //!\\ Not faster but more accurate
		}
	}
#elif COMPV_ARCH_ARM
	if (CompVCpu::isEnabled(kCpuFlagARM_NEON)) {
		COMPV_EXEC_IFDEF_INTRIN_ARM64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Intrin_NEON64);
		COMPV_EXEC_IFDEF_ASM_ARM32(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_NEON32);
		COMPV_EXEC_IFDEF_ASM_ARM64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_NEON64);
		if (CompVCpu::isEnabled(kCpuFlagARM_NEON_FMA)) {
			COMPV_EXEC_IFDEF_ASM_ARM32(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_FMA_NEON32);
			COMPV_EXEC_IFDEF_ASM_ARM64(*CompVMathDotDotSub_64f64f = CompVMathDotDotSub_64f64f_Asm_FMA_NEON64);
		}
	}
#endif
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVMathDot::hookDot_64f(
	void(**CompVMathDotDot_64f64f)(const compv_float64_t* ptrA, const compv_float64_t* ptrB, const compv_uscalar_t width, const compv_uscalar_t height, const compv_uscalar_t strideA, const compv_uscalar_t strideB, compv_float64_t* ret)
)
{
	COMPV_CHECK_EXP_RETURN(!CompVMathDotDot_64f64f, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	*CompVMathDotDot_64f64f = CompVMathDotDot_C;
#if COMPV_ARCH_X86
	if (CompVCpu::isEnabled(kCpuFlagSSE2)) {
		COMPV_EXEC_IFDEF_INTRIN_X86(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_X64_SSE2);
	}
	if (CompVCpu::isEnabled(kCpuFlagAVX)) {
		COMPV_EXEC_IFDEF_INTRIN_X86(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Intrin_AVX);
		COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_X64_AVX);
		if (CompVCpu::isEnabled(kCpuFlagFMA3)) {
			COMPV_EXEC_IFDEF_ASM_X64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_X64_FMA3_AVX); //!\\ Not faster but more accurate
		}
	}
#elif COMPV_ARCH_ARM
	if (CompVCpu::isEnabled(kCpuFlagARM_NEON)) {
		COMPV_EXEC_IFDEF_INTRIN_ARM64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Intrin_NEON64);
		COMPV_EXEC_IFDEF_ASM_ARM32(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_NEON32);
		COMPV_EXEC_IFDEF_ASM_ARM64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_NEON64);
		if (CompVCpu::isEnabled(kCpuFlagARM_NEON_FMA)) {
			COMPV_EXEC_IFDEF_ASM_ARM32(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_FMA_NEON32);
			COMPV_EXEC_IFDEF_ASM_ARM64(*CompVMathDotDot_64f64f = CompVMathDotDot_64f64f_Asm_FMA_NEON64);
		}
	}
#endif

	return COMPV_ERROR_CODE_S_OK;
}

COMPV_NAMESPACE_END()
