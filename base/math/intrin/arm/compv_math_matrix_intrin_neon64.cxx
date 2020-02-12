/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/math/intrin/arm/compv_math_matrix_intrin_neon64.h"

#if COMPV_ARCH_ARM64 && COMPV_INTRINSIC
#include "compv/base/intrin/arm/compv_intrin_neon.h"
#include "compv/base/compv_debug.h"

COMPV_NAMESPACE_BEGIN()

void CompVMathMatrixMulABt_64f_Intrin_NEON64(const COMPV_ALIGNED(NEON) compv_float64_t* A, compv_uscalar_t aRows, COMPV_ALIGNED(NEON) compv_uscalar_t aStrideInBytes, const COMPV_ALIGNED(NEON) compv_float64_t* B, compv_uscalar_t bRows, compv_uscalar_t bCols, COMPV_ALIGNED(NEON) compv_uscalar_t bStrideInBytes, COMPV_ALIGNED(NEON) compv_float64_t* R, COMPV_ALIGNED(NEON) compv_uscalar_t rStrideInBytes)
{
	COMPV_DEBUG_INFO_CHECK_NEON();

	const compv_float64_t* B0;
	compv_scalar_t k, bColsSigned = static_cast<compv_scalar_t>(bCols);

	compv_uscalar_t i, j;
	float64x2_t vec0, vec1, vec2, vec3, vecSum;
	
	for (i = 0; i < aRows; ++i) {
		B0 = B;
		for (j = 0; j < bRows; ++j) {
			vecSum = vdupq_n_f64(0);
			for (k = 0; k < bColsSigned - 15; k += 16) {
				vec0 = vmulq_f64(vld1q_f64(&A[k + 0]), vld1q_f64(&B0[k + 0]));
				vec1 = vmulq_f64(vld1q_f64(&A[k + 2]), vld1q_f64(&B0[k + 2]));
				vec2 = vmulq_f64(vld1q_f64(&A[k + 4]), vld1q_f64(&B0[k + 4]));
				vec3 = vmulq_f64(vld1q_f64(&A[k + 6]), vld1q_f64(&B0[k + 6]));
				vec0 = vmlaq_f64(vec0, vld1q_f64(&A[k + 8]), vld1q_f64(&B0[k + 8]));
				vec1 = vmlaq_f64(vec1, vld1q_f64(&A[k + 10]), vld1q_f64(&B0[k + 10]));
				vec2 = vmlaq_f64(vec2, vld1q_f64(&A[k + 12]), vld1q_f64(&B0[k + 12]));
				vec3 = vmlaq_f64(vec3, vld1q_f64(&A[k + 14]), vld1q_f64(&B0[k + 14]));
				vec0 = vaddq_f64(vec0, vec1);
				vec2 = vaddq_f64(vec2, vec3);
				vecSum = vaddq_f64(vecSum, vec0);
				vecSum = vaddq_f64(vecSum, vec2);
			}
			if (k < bColsSigned - 7) {
				vec0 = vmulq_f64(vld1q_f64(&A[k + 0]), vld1q_f64(&B0[k + 0]));
				vec1 = vmulq_f64(vld1q_f64(&A[k + 2]), vld1q_f64(&B0[k + 2]));
				vec0 = vmlaq_f64(vec0, vld1q_f64(&A[k + 4]), vld1q_f64(&B0[k + 4]));
				vec1 = vmlaq_f64(vec1, vld1q_f64(&A[k + 6]), vld1q_f64(&B0[k + 6]));
				vec0 = vaddq_f64(vec0, vec1);
				vecSum = vaddq_f64(vecSum, vec0);
				k += 8;
			}
			if (k < bColsSigned - 3) {
				vec0 = vmulq_f64(vld1q_f64(&A[k + 0]), vld1q_f64(&B0[k + 0]));
				vec0 = vmlaq_f64(vec0, vld1q_f64(&A[k + 2]), vld1q_f64(&B0[k + 2]));
				vecSum = vaddq_f64(vecSum, vec0);
				k += 4;
			}
			for (; k < bColsSigned; ++k) {
				vecSum = vcombine_f64(
					vmla_f64(vget_low_f64(vecSum), vld1_f64(&A[k + 0]), vld1_f64(&B0[k + 0])),
					vget_high_f64(vecSum));
			}
			
			vst1_f64(&R[j], (float64x1_t) { vpaddd_f64(vecSum) });

			B0 = reinterpret_cast<const compv_float64_t*>(reinterpret_cast<const uint8_t*>(B0) + bStrideInBytes);
		}
		A = reinterpret_cast<const compv_float64_t*>(reinterpret_cast<const uint8_t*>(A) + aStrideInBytes);
		R = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(R) + rStrideInBytes);
	}
}

// We'll read beyond the end of the data which means ri and rj must be strided and NEON-aligned
void CompVMathMatrixMulGA_64f_Intrin_NEON64(COMPV_ALIGNED(NEON) compv_float64_t* ri, COMPV_ALIGNED(NEON) compv_float64_t* rj, const compv_float64_t* c1, const compv_float64_t* s1, compv_uscalar_t count)
{
	COMPV_DEBUG_INFO_CHECK_NEON();
	compv_scalar_t i, countSigned = static_cast<compv_scalar_t>(count);
	float64x2_t vecRI0, vecRI1, vecRI2, vecRI3, vecRJ0, vecRJ1, vecRJ2, vecRJ3;
	float64x2_t vec0, vec1, vec2, vec3, vec4, vec5, vec6, vec7;
	const float64x2_t vecC = vdupq_n_f64(*c1);
	const float64x2_t vecS = vdupq_n_f64(*s1);

	// Case #8
	for (i = 0; i < countSigned - 7; i += 8) {
		vecRI0 = vld1q_f64(&ri[i + 0]);
		vecRI1 = vld1q_f64(&ri[i + 2]);
		vecRI2 = vld1q_f64(&ri[i + 4]);
		vecRI3 = vld1q_f64(&ri[i + 6]);
		vecRJ0 = vld1q_f64(&rj[i + 0]);
		vecRJ1 = vld1q_f64(&rj[i + 2]);
		vecRJ2 = vld1q_f64(&rj[i + 4]);
		vecRJ3 = vld1q_f64(&rj[i + 6]);
		vec0 = vmlaq_f64(vmulq_f64(vecRI0, vecC), vecRJ0, vecS);
		vec1 = vmlaq_f64(vmulq_f64(vecRI1, vecC), vecRJ1, vecS);
		vec2 = vmlaq_f64(vmulq_f64(vecRI2, vecC), vecRJ2, vecS);
		vec3 = vmlaq_f64(vmulq_f64(vecRI3, vecC), vecRJ3, vecS);
		vec4 = vmlsq_f64(vmulq_f64(vecRJ0, vecC), vecRI0, vecS);
		vec5 = vmlsq_f64(vmulq_f64(vecRJ1, vecC), vecRI1, vecS);
		vec6 = vmlsq_f64(vmulq_f64(vecRJ2, vecC), vecRI2, vecS);
		vec7 = vmlsq_f64(vmulq_f64(vecRJ3, vecC), vecRI3, vecS);
		vst1q_f64(&ri[i + 0], vec0);
		vst1q_f64(&ri[i + 2], vec1);
		vst1q_f64(&ri[i + 4], vec2);
		vst1q_f64(&ri[i + 6], vec3);
		vst1q_f64(&rj[i + 0], vec4);
		vst1q_f64(&rj[i + 2], vec5);
		vst1q_f64(&rj[i + 4], vec6);
		vst1q_f64(&rj[i + 6], vec7);
	}

	// Case #4
	if (i < countSigned - 3) {
		vecRI0 = vld1q_f64(&ri[i + 0]);
		vecRI1 = vld1q_f64(&ri[i + 2]);
		vecRJ0 = vld1q_f64(&rj[i + 0]);
		vecRJ1 = vld1q_f64(&rj[i + 2]);
		vec0 = vmlaq_f64(vmulq_f64(vecRI0, vecC), vecRJ0, vecS);
		vec1 = vmlaq_f64(vmulq_f64(vecRI1, vecC), vecRJ1, vecS);
		vec2 = vmlsq_f64(vmulq_f64(vecRJ0, vecC), vecRI0, vecS);
		vec3 = vmlsq_f64(vmulq_f64(vecRJ1, vecC), vecRI1, vecS);
		vst1q_f64(&ri[i + 0], vec0);
		vst1q_f64(&ri[i + 2], vec1);
		vst1q_f64(&rj[i + 0], vec2);
		vst1q_f64(&rj[i + 2], vec3);
		i += 4;
	}

	// Cases #2 and #1
	for (; i < countSigned; i += 2) { // event if only #1 sample remains we can read beyond count (up to stride)
		vecRI0 = vld1q_f64(&ri[i + 0]);
		vecRJ0 = vld1q_f64(&rj[i + 0]);
		vec0 = vmlaq_f64(vmulq_f64(vecRI0, vecC), vecRJ0, vecS);
		vec1 = vmlsq_f64(vmulq_f64(vecRJ0, vecC), vecRI0, vecS);
		vst1q_f64(&ri[i + 0], vec0);
		vst1q_f64(&rj[i + 0], vec1);
	}
}

void CompVMathMatrixBuildHomographyEqMatrix_64f_Intrin_NEON64(const COMPV_ALIGNED(NEON) compv_float64_t* srcX, const COMPV_ALIGNED(NEON) compv_float64_t* srcY, const COMPV_ALIGNED(NEON) compv_float64_t* dstX, const COMPV_ALIGNED(NEON) compv_float64_t* dstY, COMPV_ALIGNED(NEON) compv_float64_t* M, COMPV_ALIGNED(NEON) compv_uscalar_t M_strideInBytes, compv_uscalar_t numPoints)
{
	COMPV_DEBUG_INFO_CHECK_NEON();
	compv_float64_t* M0_ptr = const_cast<compv_float64_t*>(M);
	compv_float64_t* M1_ptr = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(M0_ptr) + M_strideInBytes);
	const size_t M_strideInBytesTimes2 = M_strideInBytes << 1;
	const float64x2_t vecZero = vdupq_n_f64(0.);
	const float64x2_t vecMinusOne = vdupq_n_f64(-1.);
	const float64x2_t vecMinusOneZero = (float64x2_t) {-1., 0.};
	float64x2_t vecSrcXY, vecMinusXMinusY, vecDstX, vecDstY;

	for (compv_uscalar_t i = 0; i < numPoints; ++i) {
		vecSrcXY = vcombine_f64(vld1_f64(&srcX[i]), vld1_f64(&srcY[i]));
		vecDstX = vdupq_n_f64(dstX[i]);
		vecDstY = vdupq_n_f64(dstY[i]);
		vecMinusXMinusY = vnegq_f64(vecSrcXY); // -x, -y
		// First #9 contributions
		vst1q_f64(&M0_ptr[0], vecMinusXMinusY); // -x, -y
		vst1q_f64(&M0_ptr[2], vecMinusOneZero); // -1, 0
		vst1q_f64(&M0_ptr[4], vecZero); // 0, 0
		vst1q_f64(&M0_ptr[6], vmulq_f64(vecDstX, vecSrcXY)); // (dstX * srcX), (dstX * srcY)
		vst1_f64(&M0_ptr[8], vget_low_f64(vecDstX));
		// Second #9 contributions
		vst1q_f64(&M1_ptr[0], vecZero); // 0, 0
		vst1q_f64(&M1_ptr[2], vcombine_f64(vget_low_f64(vecZero), vget_low_f64(vecMinusXMinusY))); // 0, -x
		vst1q_f64(&M1_ptr[4], vcombine_f64(vget_high_f64(vecMinusXMinusY), vget_high_f64(vecMinusOne))); // -y, -1
		vst1q_f64(&M1_ptr[6], vmulq_f64(vecDstY, vecSrcXY)); // (dstY * srcX), (dstY * srcY)
		vst1_f64(&M1_ptr[8], vget_low_f64(vecDstY));
		// Move to the next point
		M0_ptr = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(M0_ptr) + M_strideInBytesTimes2);
		M1_ptr = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(M1_ptr) + M_strideInBytesTimes2);
	}
}

// A and R must have same stride
// This function returns det(A). If det(A) = 0 then, A is singluar and no inverse is computed.
void CompVMathMatrixInvA3x3_64f_Intrin_NEON64(const COMPV_ALIGNED(NEON) compv_float64_t* A, COMPV_ALIGNED(NEON) compv_float64_t* R, compv_uscalar_t strideInBytes, compv_float64_t* det1)
{
	COMPV_DEBUG_INFO_CHECK_NEON();
	const compv_float64_t* a0 = A;
	const compv_float64_t* a1 = reinterpret_cast<const compv_float64_t*>(reinterpret_cast<const uint8_t*>(a0) + strideInBytes);
	const compv_float64_t* a2 = reinterpret_cast<const compv_float64_t*>(reinterpret_cast<const uint8_t*>(a1) + strideInBytes);
    
    const float64x1_t a00 = vld1_f64(&a0[0]), a01 = vld1_f64(&a0[1]), a02 = vld1_f64(&a0[2]);
    const float64x1_t a10 = vld1_f64(&a1[0]), a11 = vld1_f64(&a1[1]), a12 = vld1_f64(&a1[2]);
    const float64x1_t a20 = vld1_f64(&a2[0]), a21 = vld1_f64(&a2[1]), a22 = vld1_f64(&a2[2]);
    
	// det(A)
	float64x2_t vec0 = vmulq_f64(vcombine_f64(a11, a01), vcombine_f64(a22, a22));
	float64x2_t vec1 = vmulq_f64(vcombine_f64(a21, a21), vcombine_f64(a12, a02));
	float64x2_t vec2 = vcombine_f64(a00, a10);
	const float64x2_t vec3 = vmulq_f64(vec2, vsubq_f64(vec0, vec1));
	const float64x1_t vec3n = vsub_f64(vget_low_f64(vec3), vget_high_f64(vec3));
	vec0 = vmulq_f64(vcombine_f64(a01, a11), vcombine_f64(a12, a02));
	float64x1_t vec0n = vsub_f64(vget_low_f64(vec0), vget_high_f64(vec0));
	vec0n = vmul_f64(vec0n, a20);
	vec0n = vadd_f64(vec0n, vec3n);
	compv_float64_t detA = vget_lane_f64(vec0n, 0);
	if (detA == 0) {
		COMPV_DEBUG_INFO_EX("CompVMathMatrixInvA3x3_64f_Intrin_NEON64", "3x3 Matrix is singluar... computing pseudoinverse instead of the inverse");
	}
	else {
		const float64x2_t vecInvDetA = vdupq_n_f64(static_cast<compv_float64_t>(1.) / detA);
		compv_float64_t* r0 = R;
		compv_float64_t* r1 = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(r0) + strideInBytes);
		compv_float64_t* r2 = reinterpret_cast<compv_float64_t*>(reinterpret_cast<uint8_t*>(r1) + strideInBytes);

		vec0 = vmulq_f64(vcombine_f64(a11, a02), vcombine_f64(a22, a21));
		vec1 = vmulq_f64(vcombine_f64(a21, a22), vcombine_f64(a12, a01));
		vst1q_f64(&r0[0], vmulq_f64(vsubq_f64(vec0, vec1), vecInvDetA));

		vec0 = vmulq_f64(vcombine_f64(a01, a12), vcombine_f64(a12, a20));
		vec1 = vmulq_f64(vcombine_f64(a11, a22), vcombine_f64(a02, a10));
		vec2 = vmulq_f64(vsubq_f64(vec0, vec1), vecInvDetA);
		vst1_f64(&r0[2], vget_low_f64(vec2));
		vst1_f64(&r1[0], vget_high_f64(vec2));

		vec0 = vmulq_f64(vcombine_f64(a00, a02), vcombine_f64(a22, a10));
		vec1 = vmulq_f64(vcombine_f64(a20, a12), vcombine_f64(a02, a00));
		vst1q_f64(&r1[1], vmulq_f64(vsubq_f64(vec0, vec1), vecInvDetA)); // asm: unaligned store

		vec0 = vmulq_f64(vcombine_f64(a10, a01), vcombine_f64(a21, a20));
		vec1 = vmulq_f64(vcombine_f64(a20, a21), vcombine_f64(a11, a00));
		vst1q_f64(&r2[0], vmulq_f64(vsubq_f64(vec0, vec1), vecInvDetA));

		vec0 = vmulq_f64(vcombine_f64(a00, a10), vcombine_f64(a11, a01));
		vec0n = vsub_f64(vget_low_f64(vec0), vget_high_f64(vec0));
		vst1_f64(&r2[2], vmul_f64(vec0n, vget_low_f64(vecInvDetA)));
	}
	*det1 = detA;
}

COMPV_NAMESPACE_END()

#endif /* COMPV_ARCH_ARM64 && COMPV_INTRINSIC */
