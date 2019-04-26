/* Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/core/features/hog/compv_core_feature_hog_std.h"
#include "compv/base/compv_gradient_fast.h"
#include "compv/base/math/compv_math_utils.h"
#include "compv/base/parallel/compv_parallel.h"

#include "compv/core/features/hog/intrin/x86/compv_core_feature_hog_common_norm_intrin_sse2.h"
#include "compv/core/features/hog/intrin/x86/compv_core_feature_hog_std_intrin_sse2.h"
#include "compv/core/features/hog/intrin/x86/compv_core_feature_hog_std_intrin_avx2.h"
#include "compv/core/features/hog/intrin/arm/compv_core_feature_hog_common_norm_intrin_neon.h"
#include "compv/core/features/hog/intrin/arm/compv_core_feature_hog_std_intrin_neon.h"

#define COMPV_THIS_CLASSNAME	"CompVHogStd"

#define COMPV_HOG_BUILD_CELL_HIST_SAMPLES_PER_THREAD			(4 * 4) // unit=cells
#define COMPV_HOG_BUILD_BLOCK_DESC_SAMPLES_PER_THREAD			(1 * 1) // unit=cells

#define COMPV_HOG_FAST_BLOCK_9									1

// Documentation (HOG Standard):
//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients
//  - https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf
//	- http://lear.inrialpes.fr/people/triggs/pubs/Dalal-cvpr05.pdf
//	- https://www.learnopencv.com/histogram-of-oriented-gradients/

COMPV_NAMESPACE_BEGIN()

#if COMPV_ASM && COMPV_ARCH_X64
COMPV_EXTERNC void CompVHogCommonNormL1_9_32f_Asm_X64_SSE2(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL1Sqrt_9_32f_Asm_X64_SSE2(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2_9_32f_Asm_X64_SSE2(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2Hys_9_32f_Asm_X64_SSE2(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
#endif /* COMPV_ASM && COMPV_ARCH_X64 */

#if COMPV_ASM && COMPV_ARCH_ARM32
COMPV_EXTERNC void CompVHogCommonNormL1_9_32f_Asm_NEON32(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL1Sqrt_9_32f_Asm_NEON32(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2_9_32f_Asm_NEON32(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2Hys_9_32f_Asm_NEON32(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
#endif /* COMPV_ASM && COMPV_ARCH_ARM32 */

#if COMPV_ASM && COMPV_ARCH_ARM64
COMPV_EXTERNC void CompVHogCommonNormL1_9_32f_Asm_NEON64(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL1Sqrt_9_32f_Asm_NEON64(compv_float32_t* inOutPtr, const compv_float32_t* eps1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2_9_32f_Asm_NEON64(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
COMPV_EXTERNC void CompVHogCommonNormL2Hys_9_32f_Asm_NEON64(compv_float32_t* inOutPtr, const compv_float32_t* eps_square1, const compv_uscalar_t count);
#endif /* COMPV_ASM && COMPV_ARCH_ARM64 */

static void CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr));

static void CompVHogStdBuildMapHistForSingleCellBilinearLUT_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr));

static void CompVHogStdBuildMapHistForSingleCellNearest_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr));

static const compv_hog_floattype_t COMPV_HOG_EPSILON = compv_hog_floattype_t(1e-6);
static const compv_hog_floattype_t COMPV_HOG_EPSILON2 = COMPV_HOG_EPSILON * COMPV_HOG_EPSILON;

CompVHogStd::CompVHogStd(const CompVSizeSz& blockSize,
	const CompVSizeSz& blockStride,
	const CompVSizeSz& cellSize,
	const size_t nbins,
	const int blockNorm,
	const bool gradientSigned,
	const int interp)
	: CompVHOG(COMPV_HOGS_ID)
	, m_szBlockSize(blockSize)
	, m_szBlockStride(blockStride)
	, m_szCellSize(cellSize)
	, m_nbins(nbins)
	, m_nBlockNorm(blockNorm)
	, m_bGradientSigned(gradientSigned)
	, m_nInterp(interp)
	, m_fptrBinning(nullptr)
{

}

CompVHogStd::~CompVHogStd()
{

}

COMPV_ERROR_CODE CompVHogStd::set(int id, const void* valuePtr, size_t valueSize) /*Overrides(CompVCaps)*/
{
	COMPV_CHECK_EXP_RETURN(!valuePtr || !valueSize, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	switch (id) {
	case COMPV_HOG_SET_BOOL_GRADIENT_SIGNED: {
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(bool), COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		const bool gradientSigned = *reinterpret_cast<const bool*>(valuePtr);
		COMPV_CHECK_CODE_RETURN(updateBilinearFastData(m_nbins, m_nInterp, gradientSigned));
		m_bGradientSigned = gradientSigned;
		return COMPV_ERROR_CODE_S_OK;
	}
	case COMPV_HOG_SET_INT_BLOCK_NORM: {
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(int), COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		const int blockNorm = *reinterpret_cast<const int*>(valuePtr);
		COMPV_CHECK_EXP_RETURN(
			blockNorm != COMPV_HOG_BLOCK_NORM_NONE &&
			blockNorm != COMPV_HOG_BLOCK_NORM_L1 &&
			blockNorm != COMPV_HOG_BLOCK_NORM_L1SQRT &&
			blockNorm != COMPV_HOG_BLOCK_NORM_L2 &&
			blockNorm != COMPV_HOG_BLOCK_NORM_L2HYS,
			COMPV_ERROR_CODE_E_INVALID_PARAMETER,
			"blockNorm must be equal to COMPV_HOG_BLOCK_NORM_xxxx"
		);
		m_nBlockNorm = blockNorm;
		return COMPV_ERROR_CODE_S_OK;
	}
	case COMPV_HOG_SET_INT_NBINS: {
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(int), COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		const int nbins = *reinterpret_cast<const int*>(valuePtr);
		COMPV_CHECK_EXP_RETURN(nbins <= 1 || nbins > 360, COMPV_ERROR_CODE_E_OUT_OF_BOUND, "NBINS must be within [2-360]");
		COMPV_CHECK_CODE_RETURN(updateBilinearFastData(nbins, m_nInterp, m_bGradientSigned));
		m_nbins = nbins;
		return COMPV_ERROR_CODE_S_OK;
	}
	case COMPV_HOG_SET_INT_INTERPOLATION: {
		COMPV_CHECK_EXP_RETURN(valueSize != sizeof(int), COMPV_ERROR_CODE_E_INVALID_PARAMETER);
		const int interp = *reinterpret_cast<const int*>(valuePtr);
		COMPV_CHECK_EXP_RETURN(
			interp != COMPV_HOG_INTERPOLATION_NEAREST &&
			interp != COMPV_HOG_INTERPOLATION_BILINEAR &&
			interp != COMPV_HOG_INTERPOLATION_BILINEAR_LUT,
			COMPV_ERROR_CODE_E_INVALID_PARAMETER,
			"interp must be equal to COMPV_HOG_INTERPOLATION_xxxx"
		);
		COMPV_CHECK_CODE_RETURN(updateFptrBinning(interp, m_szCellSize));
		COMPV_CHECK_CODE_RETURN(updateBilinearFastData(m_nbins, interp, m_bGradientSigned));
		m_nInterp = interp;
		return COMPV_ERROR_CODE_S_OK;
	}
	default: {
		COMPV_DEBUG_ERROR_EX(COMPV_THIS_CLASSNAME, "Set with id %d not implemented", id);
		return COMPV_ERROR_CODE_E_NOT_IMPLEMENTED;
	}
	}
}

COMPV_ERROR_CODE CompVHogStd::get(int id, const void** valuePtrPtr, size_t valueSize) /*Overrides(CompVCaps)*/
{
	COMPV_CHECK_EXP_RETURN(!valuePtrPtr || valueSize == 0, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
#if 0
	switch (id) {
	default:
#endif
		return CompVCaps::get(id, valuePtrPtr, valueSize);
#if 0
	}
#endif
}

// IMPORTANT: this function *must be* thread-safe (do not modify members) as the HOG descriptor
// instance will be shared (see ultimateText and ultimateADAS projects)
// input could be of any type (raw float, pixels....)
COMPV_ERROR_CODE CompVHogStd::process(const CompVMatPtr& input, CompVMatPtrPtr output) /*Overrides(CompVHOG)*/
{
	COMPV_CHECK_EXP_RETURN(!input || !output, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	COMPV_CHECK_EXP_RETURN(input->planeCount() != 1, COMPV_ERROR_CODE_E_INVALID_PARAMETER, "input must be 1D (e.g. grayscale image) - could be raw float or pixels");

	// TODO(dmi): for now we only support winSize = full image. In ultimateADAS and ultimateText the input already contains the full image to identify (thanks to connected components)
	// To add support for slidding window just split the code: "pre-process" to compute mapHist and "post-process" to compute the features.
	//		make sure not to compute the histogram map (requires magnitude and direction) several times when the windows are overlapping.
	const CompVSizeSz szWinSize = CompVSizeSz(input->cols(), input->rows());
	COMPV_CHECK_EXP_RETURN(szWinSize.width < m_szBlockSize.width || szWinSize.height < m_szBlockSize.height, COMPV_ERROR_CODE_E_INVALID_PARAMETER, "winSize < blockSize");
	if (!input->isMemoryOwed()) { // Probably called bound() to extract a window for slidding
		COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("For slidding window you should change this function to compute magnitude and direction one time");
	}

	// Compute bin width associated values
	const int32_t nBinWidth = static_cast<int32_t>((m_bGradientSigned ? 360 : 180) / m_nbins);
	const compv_hog_floattype_t fBinWidth = static_cast<compv_hog_floattype_t>(nBinWidth);
	const compv_hog_floattype_t fBinWidthScale = 1 / fBinWidth;
	const int32_t nBinIdxMax = static_cast<int32_t>(m_nbins - 1);

	const compv_hog_floattype_t thetaMax = static_cast<compv_hog_floattype_t>(m_bGradientSigned ? 360 : 180);

	COMPV_ASSERT(!(m_szCellSize.width % m_szBlockStride.width) && !(m_szCellSize.height % m_szBlockStride.height));
	const CompVSizeFloat32 szStrideInCellsCount = { (m_szBlockStride.width / float(m_szCellSize.width)), (m_szBlockStride.height / float(m_szCellSize.height)) };
	COMPV_ASSERT(szStrideInCellsCount.width <= 1 && szStrideInCellsCount.height <= 1);

	// Compute magnitudes and directions: 
	//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Gradient_computation
	//	- https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf, Gradient
	CompVMatPtr gx, gy, magnitude, direction;
	COMPV_CHECK_CODE_RETURN(CompVGradientFast::gradX<compv_hog_floattype_t>(input, &gx));
	COMPV_CHECK_CODE_RETURN(CompVGradientFast::gradY<compv_hog_floattype_t>(input, &gy));
	COMPV_CHECK_CODE_RETURN(CompVGradientFast::magnitude(gx, gy, &magnitude));
	COMPV_CHECK_CODE_RETURN(CompVGradientFast::direction(gx, gy, &direction, true));
	const size_t nMagnitudeStride = magnitude->stride();
	const size_t nDirectionStride = direction->stride();
	gx = nullptr;
	gy = nullptr;

	// Compute Orientation binning: 
	//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Orientation_binning
	//	- https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf, Cell Orientation Histograms
	int numCellsX = static_cast<int>(szWinSize.width / m_szCellSize.width / szStrideInCellsCount.width);
	int numCellsY = static_cast<int>(szWinSize.height / m_szCellSize.height / szStrideInCellsCount.height);
	CompVMatPtr mapHist;
	const size_t nMapWidth = numCellsX * m_nbins;
	const size_t nMapHeight = numCellsY;
	COMPV_CHECK_CODE_RETURN(CompVMat::newObjAligned<compv_hog_floattype_t>(&mapHist, nMapHeight, nMapWidth)); // numCellsY * numBinsX
	const size_t nMapHistStride = mapHist->stride();
	const void* bilinearFastLUT = reinterpret_cast<const void*>(m_BilinearLUT.lut());
	// Multithreading - Process
	auto funcPtrOrientationBinning = [&](const size_t cellYstart, const size_t cellYend) -> COMPV_ERROR_CODE {
		const size_t yOffset = static_cast<size_t>(m_szCellSize.height * szStrideInCellsCount.height);
		const size_t yGuard = (((cellYend - 1) * yOffset) + m_szCellSize.height) > input->rows(); // Last(y) goes beyond the end?
		const compv_hog_floattype_t Yindex = compv_hog_floattype_t(cellYstart * yOffset);
		const compv_hog_floattype_t* magPtr = magnitude->ptr<const compv_hog_floattype_t>(static_cast<size_t>(Yindex));
		const compv_hog_floattype_t* dirPtr = direction->ptr<const compv_hog_floattype_t>(static_cast<size_t>(Yindex));
		compv_hog_floattype_t* mapHistPtr = mapHist->ptr<compv_hog_floattype_t>(cellYstart);
		const size_t xOffset = static_cast<size_t>(m_szCellSize.width * szStrideInCellsCount.width);
		const size_t xGuard = (((numCellsX - 1) * xOffset) + m_szCellSize.width) > input->cols(); // Last(x) goes beyond the end?
		const size_t nMagnitudeOffset = static_cast<size_t>(nMagnitudeStride * yOffset);
		const size_t nDirectionOffset = static_cast<size_t>(nDirectionStride * yOffset);
		int i;
		size_t x;
		for (size_t j = cellYstart; j < cellYend - yGuard; ++j) {
			for (i = 0, x = 0; i < numCellsX - xGuard; ++i, x += xOffset) {
				compv_hog_floattype_t* mapHistPtr_ = &mapHistPtr[i * m_nbins];
				// Reset histogram for the current cell
				for (size_t k = 0; k < m_nbins; ++k) {
					mapHistPtr_[k] = 0;
				}
				// Process
				m_fptrBinning(
					&magPtr[x],
					&dirPtr[x],
					mapHistPtr_,
					&thetaMax,
					&fBinWidthScale,
					&nBinWidth,
					&nBinIdxMax,
					static_cast<const compv_uscalar_t>(m_szCellSize.width),
					static_cast<const compv_uscalar_t>(m_szCellSize.height),
					static_cast<const compv_uscalar_t>(nMagnitudeStride),
					static_cast<const compv_uscalar_t>(nDirectionStride),
					bilinearFastLUT
				);
			}
			mapHistPtr += nMapHistStride;
			magPtr += nMagnitudeOffset;
			dirPtr += nDirectionOffset;
		}
		return COMPV_ERROR_CODE_S_OK;
	};
#if 1
	COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
		funcPtrOrientationBinning,
		numCellsX,
		numCellsY,
		COMPV_HOG_BUILD_CELL_HIST_SAMPLES_PER_THREAD
	));
#else
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No MT-implementation could be found");
	COMPV_CHECK_CODE_RETURN(funcPtrOrientationBinning(
		0, numCellsY
	));
#endif

	// Compute output (features/descriptor) size
	size_t nOutSize;
	COMPV_CHECK_CODE_RETURN(CompVHOG::descriptorSize(
		szWinSize, // winSize
		m_szBlockSize, // blockSize,
		m_szBlockStride, // blockStride,
		m_szCellSize, // cellSize,
		m_nbins,
		&nOutSize));
	COMPV_DEBUG_VERBOSE_EX(COMPV_THIS_CLASSNAME, "Descriptor size = %zu", nOutSize);

	// Descriptor blocks + Block normalization
	//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Descriptor_blocks
	//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Block_normalization
	//	- https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf - Block Normalization
	int numBlocksX = 0;
	int numBlocksY = 0;
	for (size_t blockX = 0; blockX <= szWinSize.width - m_szBlockSize.width; blockX += m_szBlockStride.width) ++numBlocksX;
	for (size_t blockY = 0; blockY <= szWinSize.height - m_szBlockSize.height; blockY += m_szBlockStride.height) ++numBlocksY;
	const size_t numBlocks = numBlocksX * numBlocksY;
	COMPV_ASSERT(numBlocks > 0);

	COMPV_ASSERT(!(m_szBlockSize.width % m_szCellSize.width));
	COMPV_ASSERT(!(m_szBlockSize.height % m_szCellSize.height));
	const size_t numCellsPerBlockX = (m_szBlockSize.width / m_szCellSize.width);
	const size_t numCellsPerBlockY = (m_szBlockSize.height / m_szCellSize.height);
	const size_t numBinsPerBlockX = (numCellsPerBlockX * m_nbins);
	const size_t nBlockBinsCount = numCellsPerBlockY * numBinsPerBlockX;
	const size_t nBlocksTotalBinsCount = numBlocks * nBlockBinsCount;
	COMPV_ASSERT(nBlocksTotalBinsCount == nOutSize);
	const size_t nMaxBlockX = szWinSize.width - m_szBlockSize.width;

	// Create output
	CompVMatPtr output_ = *output;
	COMPV_CHECK_CODE_RETURN(CompVMat::newObjAligned<compv_hog_floattype_t>(&output_, 1, nOutSize));

	// Set norm function
	void(*fptr_norm)(compv_hog_floattype_t* inOutPtr, const compv_hog_floattype_t* eps1, const compv_uscalar_t count) = nullptr;
	const compv_hog_floattype_t eps = (m_nBlockNorm == COMPV_HOG_BLOCK_NORM_L2 || m_nBlockNorm == COMPV_HOG_BLOCK_NORM_L2HYS)
		? COMPV_HOG_EPSILON2 : COMPV_HOG_EPSILON;
	switch (m_nBlockNorm) {
	case COMPV_HOG_BLOCK_NORM_L1: fptr_norm = (nBlockBinsCount == 9) ? fptrs_norm.L1_9 : fptrs_norm.L1; break;
	case COMPV_HOG_BLOCK_NORM_L1SQRT: fptr_norm = (nBlockBinsCount == 9) ? fptrs_norm.L1Sqrt_9 : fptrs_norm.L1Sqrt; break;
	case COMPV_HOG_BLOCK_NORM_L2: fptr_norm = (nBlockBinsCount == 9) ? fptrs_norm.L2_9 : fptrs_norm.L2; break;
	case COMPV_HOG_BLOCK_NORM_L2HYS: fptr_norm = (nBlockBinsCount == 9) ? fptrs_norm.L2Hys_9 : fptrs_norm.L2Hys; break;
	case COMPV_HOG_BLOCK_NORM_NONE: break;
	default: COMPV_CHECK_CODE_RETURN(COMPV_ERROR_CODE_E_NOT_IMPLEMENTED, "Supported norm functions: none,L1"); break;
	}
	
	// Multithreading - Process
	auto funcPtrBlockDescriptorAndNorm = [&](const size_t blockYstart, const size_t blockYend) -> COMPV_ERROR_CODE {
		compv_hog_floattype_t* outputPtr = output_->ptr<compv_hog_floattype_t>(0, (blockYstart * (numBlocksX * nBlockBinsCount)));
		const size_t Yindex = blockYstart * COMPV_MATH_ROUNDFU_2_NEAREST_INT(szStrideInCellsCount.height, size_t);
		const compv_hog_floattype_t* mapHistPtr = mapHist->ptr<const compv_hog_floattype_t>(Yindex);
		const size_t xBinOffset = m_nbins * COMPV_MATH_ROUNDFU_2_NEAREST_INT(szStrideInCellsCount.width, size_t);
		COMPV_ASSERT(xBinOffset && (numBlocksX * xBinOffset) <= mapHist->cols());
		COMPV_ASSERT((numBlocksY * (numBlocksX * nBlockBinsCount)) <= output_->cols());
		for (size_t blockY = blockYstart; blockY < blockYend; ++blockY) {
			for (size_t blockX = 0, binX = 0; blockX <= nMaxBlockX; blockX += m_szBlockStride.width, binX += xBinOffset) {
				// Build vector for a block at position (blocX, blockY)
				COMPV_CHECK_CODE_RETURN(CompVHogStd::buildOutputForSingleBlock(
					&mapHistPtr[binX], outputPtr,
					numCellsPerBlockY, numBinsPerBlockX, nMapHistStride
				));
				// Normalize the output vector
				if (fptr_norm) {
					fptr_norm(outputPtr, &eps, nBlockBinsCount);
				}
				outputPtr += nBlockBinsCount;
			}
			mapHistPtr += nMapHistStride;
		}
		return COMPV_ERROR_CODE_S_OK;
	};
#if 1
	COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
		funcPtrBlockDescriptorAndNorm,
		numBlocksX,
		numBlocksY,
		COMPV_HOG_BUILD_BLOCK_DESC_SAMPLES_PER_THREAD
	));
#else
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No MT-implementation could be found");
	COMPV_CHECK_CODE_RETURN(funcPtrBlockDescriptorAndNorm(
		0, numBlocksY
	));
#endif

	*output = output_;
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVHogStd::updateFptrBinning(const int& nInterp, const CompVSizeSz& szCellSize)
{
	CompVHogStdBuildMapHistForSingleCell_32f32s binningFuncPtr =
		(nInterp == COMPV_HOG_INTERPOLATION_NEAREST)
		? CompVHogStdBuildMapHistForSingleCellNearest_32f32s_C 
		: (nInterp == COMPV_HOG_INTERPOLATION_BILINEAR) ? CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_C : CompVHogStdBuildMapHistForSingleCellBilinearLUT_32f32s_C;
	if (nInterp == COMPV_HOG_INTERPOLATION_BILINEAR) {
#		if COMPV_ARCH_X86
		if (CompVCpu::isEnabled(kCpuFlagSSE2) && COMPV_IS_ALIGNED_SSE(szCellSize.width * sizeof(compv_float32_t))) {
			COMPV_EXEC_IFDEF_INTRIN_X86(binningFuncPtr = CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_Intrin_SSE2);
		}
		if (CompVCpu::isEnabled(kCpuFlagAVX2) && COMPV_IS_ALIGNED_AVX(szCellSize.width * sizeof(compv_float32_t))) {
			COMPV_EXEC_IFDEF_INTRIN_X86(binningFuncPtr = CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_Intrin_AVX2);
		}
#		elif COMPV_ARCH_ARM
		if (CompVCpu::isEnabled(kCpuFlagARM_NEON) && COMPV_IS_ALIGNED_NEON(szCellSize.width * sizeof(compv_float32_t))) {
			COMPV_EXEC_IFDEF_INTRIN_ARM(binningFuncPtr = CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_Intrin_NEON);
		}
#		endif
	}
	m_fptrBinning = binningFuncPtr;
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVHogStd::updateBilinearFastData(const size_t& nbins, const int& interp, const bool& gradientSigned)
{
	COMPV_CHECK_CODE_RETURN(m_BilinearLUT.update(nbins, interp, gradientSigned));
	return COMPV_ERROR_CODE_S_OK;
}

// Descriptor blocks + Block normalization
//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Descriptor_blocks
//	- https://en.wikipedia.org/wiki/Histogram_of_oriented_gradients#Block_normalization
//	- https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf - Block Normalization
COMPV_ERROR_CODE CompVHogStd::buildOutputForSingleBlock(
	const compv_hog_floattype_t* mapHistPtr, compv_hog_floattype_t* outputPtr,
	const size_t& numCellsPerBlockY, const size_t& numBinsPerBlockX, const size_t& mapHistStride
)
{
	// Private function, do not check input params
	
	const size_t numBinsPerBlockX8 = numBinsPerBlockX & -8;
	size_t binX;
	for (size_t blockCellY = 0; blockCellY < numCellsPerBlockY; ++blockCellY) {
		// TODO(dmi): norm : none
		for (binX = 0; binX < numBinsPerBlockX8; binX += 8) {
			outputPtr[binX] = mapHistPtr[binX];
			outputPtr[binX + 1] = mapHistPtr[binX + 1];
			outputPtr[binX + 2] = mapHistPtr[binX + 2];
			outputPtr[binX + 3] = mapHistPtr[binX + 3];
			outputPtr[binX + 4] = mapHistPtr[binX + 4];
			outputPtr[binX + 5] = mapHistPtr[binX + 5];
			outputPtr[binX + 6] = mapHistPtr[binX + 6];
			outputPtr[binX + 7] = mapHistPtr[binX + 7];
		}
		for (; binX < numBinsPerBlockX; ++binX) {
			outputPtr[binX] = mapHistPtr[binX];
		}
		outputPtr += numBinsPerBlockX;
		mapHistPtr += mapHistStride;
	}
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVHogStd::newObj(
	CompVHOGPtrPtr hog,
	const CompVSizeSz& blockSize COMPV_DEFAULT(CompVSizeSz(16, 16)),
	const CompVSizeSz& blockStride COMPV_DEFAULT(CompVSizeSz(8, 8)),
	const CompVSizeSz& cellSize COMPV_DEFAULT(CompVSizeSz(8, 8)),
	const size_t nbins COMPV_DEFAULT(9),
	const int blockNorm COMPV_DEFAULT(COMPV_HOG_BLOCK_NORM_L2HYS),
	const bool gradientSigned COMPV_DEFAULT(true),
	const int interp COMPV_DEFAULT(COMPV_HOG_INTERPOLATION_BILINEAR))
{
	COMPV_CHECK_EXP_RETURN(!hog, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	COMPV_CHECK_CODE_RETURN(CompVHOG::checkParams(blockSize, blockStride, cellSize, nbins, blockNorm, gradientSigned));
	CompVHogStdPtr hog_ = new CompVHogStd(blockSize, blockStride, cellSize, nbins, blockNorm, gradientSigned, interp);
	COMPV_CHECK_EXP_RETURN(!hog_, COMPV_ERROR_CODE_E_OUT_OF_MEMORY);

	// Update binning function pointer
	COMPV_CHECK_CODE_RETURN(hog_->updateFptrBinning(interp, cellSize));
	COMPV_CHECK_CODE_RETURN(hog_->updateBilinearFastData(nbins, interp, gradientSigned));

#if COMPV_ARCH_X86
	if (CompVCpu::isEnabled(kCpuFlagSSE2)) {
		/* == L1 == */
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1 = CompVHogCommonNormL1_32f_Intrin_SSE2);
#		if COMPV_HOG_FAST_BLOCK_9
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_9_32f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_9_32f_Asm_X64_SSE2);
#		else
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_32f_Intrin_SSE2);
#		endif		

		/* == L1Sqrt == */
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1Sqrt = CompVHogCommonNormL1Sqrt_32f_Intrin_SSE2);
#		if COMPV_HOG_FAST_BLOCK_9
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_9_32f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_9_32f_Asm_X64_SSE2);
#		else
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_32f_Intrin_SSE2);
#		endif

		/* == L2 == */
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2 = CompVHogCommonNormL2_32f_Intrin_SSE2);
#		if COMPV_HOG_FAST_BLOCK_9
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_9_32f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_9_32f_Asm_X64_SSE2);
#		else
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_32f_Intrin_SSE2);
#		endif

		/* == L2Hys == */
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2Hys = CompVHogCommonNormL2Hys_32f_Intrin_SSE2);
#		if COMPV_HOG_FAST_BLOCK_9
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_9_32f_Intrin_SSE2);
		COMPV_EXEC_IFDEF_ASM_X64(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_9_32f_Asm_X64_SSE2);
#		else
		COMPV_EXEC_IFDEF_INTRIN_X86(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_32f_Intrin_SSE2);
#		endif
	}
#elif COMPV_ARCH_ARM
    if (CompVCpu::isEnabled(kCpuFlagARM_NEON)) {
        /* == L1 == */
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1 = CompVHogCommonNormL1_32f_Intrin_NEON);
#		if COMPV_HOG_FAST_BLOCK_9
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_9_32f_Intrin_NEON);
        COMPV_EXEC_IFDEF_ASM_ARM32(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_9_32f_Asm_NEON32);
        COMPV_EXEC_IFDEF_ASM_ARM64(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_9_32f_Asm_NEON64);
#		else
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1_9 = CompVHogCommonNormL1_32f_Intrin_NEON);
#		endif
        
        /* == L1Sqrt == */
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1Sqrt = CompVHogCommonNormL1Sqrt_32f_Intrin_NEON);
#		if COMPV_HOG_FAST_BLOCK_9
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_9_32f_Intrin_NEON);
        COMPV_EXEC_IFDEF_ASM_ARM32(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_9_32f_Asm_NEON32);
        COMPV_EXEC_IFDEF_ASM_ARM64(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_9_32f_Asm_NEON64);
#		else
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L1Sqrt_9 = CompVHogCommonNormL1Sqrt_32f_Intrin_NEON);
#		endif
        
        /* == L2 == */
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2 = CompVHogCommonNormL2_32f_Intrin_NEON);
#		if COMPV_HOG_FAST_BLOCK_9
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_9_32f_Intrin_NEON);
        COMPV_EXEC_IFDEF_ASM_ARM32(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_9_32f_Asm_NEON32);
        COMPV_EXEC_IFDEF_ASM_ARM64(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_9_32f_Asm_NEON64);
#		else
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2_9 = CompVHogCommonNormL2_32f_Intrin_NEON);
#		endif
        
        /* == L2Hys == */
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2Hys = CompVHogCommonNormL2Hys_32f_Intrin_NEON);
#		if COMPV_HOG_FAST_BLOCK_9
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_9_32f_Intrin_NEON);
        COMPV_EXEC_IFDEF_ASM_ARM32(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_9_32f_Asm_NEON32);
        COMPV_EXEC_IFDEF_ASM_ARM64(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_9_32f_Asm_NEON64);
#		else
        COMPV_EXEC_IFDEF_INTRIN_ARM(hog_->fptrs_norm.L2Hys_9 = CompVHogCommonNormL2Hys_32f_Intrin_NEON);
#		endif
    }
#endif
    
    *hog = *hog_;
    return COMPV_ERROR_CODE_S_OK;
}

static void CompVHogStdBuildMapHistForSingleCellBilinear_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr)
)
{
	// "cellWidth" and "cellHeight" are expected to be small values (<=16) which means no GPU implementation is needed
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPU implementation could be found");
	const compv_float32_t& thetaMax = *thetaMax1;
	const compv_float32_t& scaleBinWidth = *scaleBinWidth1;
	const int32_t& binWidth = *binWidth1;
	const int32_t& binIdxMax = *binIdxMax1;
	for (compv_uscalar_t j = 0; j < cellHeight; ++j) {
		for (compv_uscalar_t i = 0; i < cellWidth; ++i) {
			const compv_float32_t theta = (dirPtr[i] > thetaMax) ? (dirPtr[i] - thetaMax) : dirPtr[i];
#			if ((defined(_DEBUG) && _DEBUG != 0) || (defined(DEBUG) && DEBUG != 0))
			COMPV_ASSERT(theta >= 0 && theta <= thetaMax);
#			endif
			// Bilinear interpolation: https://www2.cs.duke.edu/courses/fall15/compsci527/notes/hog.pdf, Cell Orientation Histograms
			// Also see: https://www.learnopencv.com/histogram-of-oriented-gradients/, Step 3 : Calculate Histogram of Gradients in 8�8 cells
			// Signed grad with 9 bins (width = 40) :  
			//	- legs:	[20, 60, 100, 140, 180, 220, 260, 300, 340]
			//	- leg 20:  [0   -  40] , note: 0 is same as 360 (wraps around)
			//	- leg 60:  [40  -  80]
			//	- leg 100: [80  - 120]
			//	- leg 140: [120 - 160]
			//	- leg 180: [160 - 200]
			//	- leg 220: [200 - 240]
			//	- leg 260: [240 - 280]
			//	- leg 300: [280 - 320]
			//	- leg 340: [320 - 360] , note: 360 is the same as 0 (wraps around)
			//	with 20 degrees on the right and left of each leg				
			const int32_t binIdx = static_cast<int32_t>((theta * scaleBinWidth) - 0.5f);
#			if ((defined(_DEBUG) && _DEBUG != 0) || (defined(DEBUG) && DEBUG != 0))
			// binIdx = COMPV_MATH_CLIP3(0, binIdxMax, binIdx);
			COMPV_ASSERT(binIdx >= 0 && binIdx <= binIdxMax);
#			endif
			const compv_float32_t diff = ((theta - (binIdx * binWidth)) * scaleBinWidth) - 0.5f;
#			if 0 // This code is branchless and could be used for SIMD accel
			COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("Next (see #else) code faster");
			const compv_float32_t avv = std::abs(magPtr[i] * diff);
			const int32_t binIdxNext = binIdx + ((diff >= 0) ? 1 : -1);
			mapHistPtr[binIdxNext < 0 ? binIdxMax : (binIdxNext > binIdxMax ? 0 : binIdxNext)] += avv;
			mapHistPtr[binIdx] += (magPtr[i] - avv);
#			else
			const compv_float32_t vv = magPtr[i] * diff;
			if (diff >= 0) {
				mapHistPtr[binIdx == binIdxMax ? 0 : (binIdx + 1)] += vv;
				mapHistPtr[binIdx] += (magPtr[i] - vv);
			}
			else {
				mapHistPtr[binIdx ? (binIdx - 1) : binIdxMax] -= vv;
				mapHistPtr[binIdx] += (magPtr[i] + vv);
			}
#			endif
		}
		magPtr += magStride;
		dirPtr += dirStride;
	}
}

static void CompVHogStdBuildMapHistForSingleCellBilinearLUT_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr)
)
{
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("Not faster than BilinearFull when SIMD is enabled");
	// "cellWidth" and "cellHeight" are expected to be small values (<=16) which means no GPU implementation is needed
	const CompVHogStdBilinearLUTIdx* bilinearFastLUT_ = reinterpret_cast<const CompVHogStdBilinearLUTIdx*>(bilinearFastLUT);
	const compv_float32_t& thetaMax = *thetaMax1;
	const size_t cellWidth4 = cellWidth & -4;
	for (compv_uscalar_t j = 0; j < cellHeight; ++j) {
		/*for (compv_uscalar_t i = 0; i < cellWidth; ++i) {
			const compv_float32_t theta = (dirPtr[i] > thetaMax) ? (dirPtr[i] - thetaMax) : dirPtr[i];
#			if ((defined(_DEBUG) && _DEBUG != 0) || (defined(DEBUG) && DEBUG != 0))
			COMPV_ASSERT(theta >= 0 && theta <= thetaMax);
#			endif
			const int32_t thetaIdx = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const CompVHogStdBilinearLUTIdx& lut = bilinearFastLUT_[thetaIdx];
			const compv_float32_t avv = std::abs(magPtr[i] * lut.diff);
			mapHistPtr[lut.binIdxNext] += avv;
			mapHistPtr[lut.binIdx] += (magPtr[i] - avv);
		}*/
		compv_uscalar_t i;
		for (i = 0; i < cellWidth4; i += 4) {
			const compv_float32_t theta_0 = (dirPtr[i] > thetaMax) ? (dirPtr[i] - thetaMax) : dirPtr[i];
			const compv_float32_t theta_1 = (dirPtr[i + 1] > thetaMax) ? (dirPtr[i + 1] - thetaMax) : dirPtr[i + 1];
			const compv_float32_t theta_2 = (dirPtr[i + 2] > thetaMax) ? (dirPtr[i + 2] - thetaMax) : dirPtr[i + 2];
			const compv_float32_t theta_3 = (dirPtr[i + 3] > thetaMax) ? (dirPtr[i + 3] - thetaMax) : dirPtr[i + 3];
#			if ((defined(_DEBUG) && _DEBUG != 0) || (defined(DEBUG) && DEBUG != 0))
			COMPV_ASSERT(theta_0 >= 0 && theta_0 <= thetaMax);
			COMPV_ASSERT(theta_1 >= 0 && theta_1 <= thetaMax);
			COMPV_ASSERT(theta_2 >= 0 && theta_2 <= thetaMax);
			COMPV_ASSERT(theta_3 >= 0 && theta_3 <= thetaMax);
#			endif
			const int32_t thetaIdx_0 = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta_0 * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const int32_t thetaIdx_1 = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta_1 * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const int32_t thetaIdx_2 = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta_2 * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const int32_t thetaIdx_3 = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta_3 * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const CompVHogStdBilinearLUTIdx& lut_0 = bilinearFastLUT_[thetaIdx_0];
			const CompVHogStdBilinearLUTIdx& lut_1 = bilinearFastLUT_[thetaIdx_1];
			const CompVHogStdBilinearLUTIdx& lut_2 = bilinearFastLUT_[thetaIdx_2];
			const CompVHogStdBilinearLUTIdx& lut_3 = bilinearFastLUT_[thetaIdx_3];
			const compv_float32_t avv_0 = std::abs(magPtr[i] * lut_0.diff);
			const compv_float32_t avv_1 = std::abs(magPtr[i + 1] * lut_1.diff);
			const compv_float32_t avv_2 = std::abs(magPtr[i + 2] * lut_2.diff);
			const compv_float32_t avv_3 = std::abs(magPtr[i + 3] * lut_3.diff);
			mapHistPtr[lut_0.binIdxNext] += avv_0;
			mapHistPtr[lut_0.binIdx] += (magPtr[i] - avv_0);
			mapHistPtr[lut_1.binIdxNext] += avv_1;
			mapHistPtr[lut_1.binIdx] += (magPtr[i + 1] - avv_1);
			mapHistPtr[lut_2.binIdxNext] += avv_2;
			mapHistPtr[lut_2.binIdx] += (magPtr[i + 2] - avv_2);
			mapHistPtr[lut_3.binIdxNext] += avv_3;
			mapHistPtr[lut_3.binIdx] += (magPtr[i + 3] - avv_3);
		}
		for (; i < cellWidth; i += 1) {
			const compv_float32_t theta_0 = (dirPtr[i] > thetaMax) ? (dirPtr[i] - thetaMax) : dirPtr[i];
#			if ((defined(_DEBUG) && _DEBUG != 0) || (defined(DEBUG) && DEBUG != 0))
			COMPV_ASSERT(theta_0 >= 0 && theta_0 <= thetaMax);
#			endif
			const int32_t thetaIdx_0 = COMPV_MATH_ROUNDFU_2_NEAREST_INT((theta_0 * 10), int32_t); // e.g. if theta = 359.96 then, thetaIdx = int(359.96 * 10) = 360
			const CompVHogStdBilinearLUTIdx& lut_0 = bilinearFastLUT_[thetaIdx_0];
			const compv_float32_t avv_0 = std::abs(magPtr[i] * lut_0.diff);
			mapHistPtr[lut_0.binIdxNext] += avv_0;
			mapHistPtr[lut_0.binIdx] += (magPtr[i] - avv_0);
		}
		magPtr += magStride;
		dirPtr += dirStride;
	}
}

static void CompVHogStdBuildMapHistForSingleCellNearest_32f32s_C(
	const compv_float32_t* magPtr,
	const compv_float32_t* dirPtr,
	compv_float32_t* mapHistPtr,
	const compv_float32_t* thetaMax1,
	const compv_float32_t* scaleBinWidth1,
	const int32_t* binWidth1,
	const int32_t* binIdxMax1,
	const compv_uscalar_t cellWidth,
	const compv_uscalar_t cellHeight,
	const compv_uscalar_t magStride,
	const compv_uscalar_t dirStride,
	const void* bilinearFastLUT COMPV_DEFAULT(nullptr)
)
{
	// "cellWidth" and "cellHeight" are expected to be small values (<=16) which means no GPU implementation is needed
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("No SIMD or GPU implementation could be found");
	const compv_float32_t& thetaMax = *thetaMax1;
	const compv_float32_t& scaleBinWidth = *scaleBinWidth1;
	for (compv_uscalar_t j = 0; j < cellHeight; ++j) {
		for (compv_uscalar_t i = 0; i < cellWidth; ++i) {
			const compv_float32_t theta = (dirPtr[i] > thetaMax) ? (dirPtr[i] - thetaMax) : dirPtr[i];
			const int32_t binIdx = static_cast<int32_t>(theta * scaleBinWidth);
			mapHistPtr[binIdx] += magPtr[i];
		}
		magPtr += magStride;
		dirPtr += dirStride;
	}
}

COMPV_NAMESPACE_END()
