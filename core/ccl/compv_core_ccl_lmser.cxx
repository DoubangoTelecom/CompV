﻿/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/

/* @description
This class implement LMSER (Linear Time Maximally Stable Extremal Regions) algorithm.
Some literature about MSER:
- Linear Time Maximally Stable Extremal Regions: https://github.com/Stanley/043/blob/master/docs/bibl/Linear%20Time%20Maximally%20Stable%20Extremal%20Regions/53030183.pdf
*/

#include "compv/core/ccl/compv_core_ccl_lmser.h"
#include "compv/core/compv_core.h"
#include "compv/base/parallel/compv_parallel.h"
#include "compv/base/compv_cpu.h"
#include "compv/base/compv_memz.h"
#include "compv/base/compv_mem_pool_light.h"
#include "compv/base/compv_bits.h"

#define COMPV_CORE_LMSER_ACCESSIBILITY_BUILD_SAMPLES_PER_THREAD		(40 * 40)
#define COMPV_CORE_LMSER_FILL_REGIONS_SAMPLES_PER_THREAD			(1 * 1) // This is the number of regions -> use max threads

#if COMPV_ARCH_X64 || COMPV_ARCH_ARM64
#	define LMSER_USE_BSF	1
#endif /* 64bits arch */

#define LMSER_HIGHEST_GREYLEVEL		256
#define LMSER_GOTO(stepx) goto __________________________##stepx##__________________________
#define LMSER_CHECK_EDGE() { \
	const int neighbor_pixel = current_pixel + LMSER_EDGES_OFFSETS[current_edge]; \
	if (!ptr8uAccessibleRef[neighbor_pixel]) { \
		ptr8uAccessibleRef[neighbor_pixel] = 1; \
		const uint8_t neighbor_level = ptr8uPixelsRef[neighbor_pixel]; \
		if (neighbor_level >= current_level) { \
			boundaryPixelsMgr.push_back(poolBoundaryPixelsPtr, neighbor_level, neighbor_pixel); \
			if (neighbor_level < current_priority) current_priority = neighbor_level; \
		} \
		else { \
			boundaryPixelsMgr.push_back(poolBoundaryPixelsPtr, current_level, (current_pixel | (static_cast<uint32_t>(current_edge + 1) << 28))); \
			if (current_level < current_priority) current_priority = current_level; \
			current_edge = 0; \
			current_pixel = neighbor_pixel; \
			current_level = neighbor_level; \
			LMSER_GOTO(step3); \
		} \
	} \
	++current_edge; \
}

#define LMSER_BSF_UPDATE_PRIO(_prio_) { \
	if (flags[_prio_]) { \
		compv_bsf_t ret; \
		compv_bsf64(flags[_prio_], &ret); \
		current_priority = static_cast<uint16_t>(ret + (_prio_ << 6)); \
		return; \
	} \
}

#define COMPV_THIS_CLASSNAME	"CompVConnectedComponentLabelingLMSER"

COMPV_NAMESPACE_BEGIN()

typedef CompVConnectedComponentLmserLinkedListNode<uint32_t> CompVConnectedComponentLmserLinkedListNodeBoundaryPixel;
typedef CompVConnectedComponentLmserLinkedListFrwBkw<uint32_t> CompVConnectedComponentLmserLinkedListBoundaryPixel;

// A bitmask is keeping track of which out of the 256 greylevels
// have pixels waiting.This allows us to use a single instruction to find
// which is the smallest(or largest) occupied grey - level by using a machine instruction
// that retrieves the id number of the least significant or most significant
// bit set(for x86 platforms bsf and bsr, or the more general 64 - bit compliant
//	BitScanForward, BitScanReverse).
struct CompVConnectedComponentLabelingLmserBoundaryPixelsMgr {
	CompVConnectedComponentLabelingLmserBoundaryPixelsMgr(CompVConnectedComponentLmserLinkedListBoundaryPixel* boundaryPixels_)
	: boundaryPixels(boundaryPixels_) {
#if LMSER_USE_BSF
		flags[0] = 0;
		flags[1] = 0;
		flags[2] = 0;
		flags[3] = 0;
#endif /* LMSER_USE_BSF */
	}
	COMPV_ALWAYS_INLINE void push_back(CompVConnectedComponentLmserLinkedListNodeBoundaryPixel*& pool, const uint8_t level, const uint32_t pixel) {
		CompVConnectedComponentLmserLinkedListBoundaryPixel& boundaryPixels_ = boundaryPixels[level];
		pool->data = pixel;
		pool->link = boundaryPixels_.tail, boundaryPixels_.tail = pool++;
#if LMSER_USE_BSF
#	if defined(_MSC_VER)
		_bittestandset64(reinterpret_cast<LONG64*>(&flags[level >> 6]), (level & 63));
#	else
		flags[level >> 6] |= (1ull << (level & 63));
#	endif /* defined(_MSC_VER) */		
#endif /* LMSER_USE_BSF */
	}
	COMPV_ALWAYS_INLINE void pop_back(uint16_t& current_priority) {
		CompVConnectedComponentLmserLinkedListNodeBoundaryPixel*& tail = boundaryPixels[current_priority].tail;
		tail = tail->link; // pop_back()
		if (!tail) {
#if LMSER_USE_BSF
			// Clear flag
#	if defined(_MSC_VER)
			_bittestandreset64(reinterpret_cast<LONG64*>(&flags[current_priority >> 6]), (current_priority & 63));
#	else
			flags[current_priority >> 6] ^= (1ull << (current_priority & 63));
#	endif /* defined(_MSC_VER) */	
			// Move to the next priority
			switch ((current_priority >> 6)) {
				case 0: LMSER_BSF_UPDATE_PRIO(0);
				case 1: LMSER_BSF_UPDATE_PRIO(1);
				case 2: LMSER_BSF_UPDATE_PRIO(2);
				case 3: LMSER_BSF_UPDATE_PRIO(3);
			}
			current_priority = LMSER_HIGHEST_GREYLEVEL;
#else
			for (; current_priority < LMSER_HIGHEST_GREYLEVEL && !boundaryPixels[current_priority].tail; ++current_priority)
				/* do nothing */;
#endif /* LMSER_USE_BSF */
		}
	}
	CompVConnectedComponentLmserLinkedListBoundaryPixel* boundaryPixels;
#if LMSER_USE_BSF
	uint64_t flags[4];
#endif /* LMSER_USE_BSF */
};

CompVConnectedComponentLabelingLMSER::CompVConnectedComponentLabelingLMSER()
	: CompVConnectedComponentLabeling(COMPV_LMSER_ID)
{

}

CompVConnectedComponentLabelingLMSER::~CompVConnectedComponentLabelingLMSER()
{

}

COMPV_ERROR_CODE CompVConnectedComponentLabelingLMSER::set(int id, const void* valuePtr, size_t valueSize) /*Overrides(CompVConnectedComponentLabeling)*/
{
	switch (id) {
	case COMPV_CCL_SET_INT_CONNECTIVITY:
	default:
		COMPV_CHECK_CODE_RETURN(CompVConnectedComponentLabeling::set(id, valuePtr, valueSize));
		return COMPV_ERROR_CODE_S_OK;
	}
}

COMPV_ERROR_CODE CompVConnectedComponentLabelingLMSER::process(const CompVMatPtr& ptr8uImage, CompVConnectedComponentLabelingResultPtrPtr result) /*Overrides(CompVConnectedComponentLabeling)*/
{
	COMPV_CHECK_EXP_RETURN(!ptr8uImage || ptr8uImage->isEmpty() || ptr8uImage->planeCount() != 1 || ptr8uImage->elmtInBytes() != sizeof(uint8_t) || !result
		, COMPV_ERROR_CODE_E_INVALID_PARAMETER);

    // Using ANDROID guard instead of ARM because the issue doesn't happen on iOS (tested on iPad2(ARM64) and MT is faster than ST)
#if COMPV_OS_ANDROID
	COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("On ARM64/32 (MediaPad2), ST faster than MT. Not the case for Galaxy Tab6 ARM32.");
#endif /* COMPV_OS_ANDROID */

	/* Create result */
	CompVConnectedComponentLabelingResultLMSERImplPtr result_;
	if (*result && (*result)->id() == id()) {
		result_ = reinterpret_cast<CompVConnectedComponentLabelingResultLMSERImpl*>(**result);
	}
	if (!result_) {
		COMPV_CHECK_CODE_RETURN(CompVConnectedComponentLabelingResultLMSERImpl::newObj(&result_));
	}
	COMPV_CHECK_CODE_RETURN(result_->reset());
	CompVConnectedComponentLabelingRegionMserVector& vecRegions_final = result_->vecRegions();

	const int width = static_cast<int>(ptr8uImage->cols());
	const int height = static_cast<int>(ptr8uImage->rows());
	const int stride = static_cast<int>(ptr8uImage->stride());

	const bool b8Connectivity = (connectivity() == 8);
	const int maxEdges = b8Connectivity
		? 8
		: 4;
	int LMSER_EDGES_OFFSETS[8];
	if (b8Connectivity) {
		//!\\ Keep this order because it's more cache-friendly
		LMSER_EDGES_OFFSETS[0] = 1; // RIGHT
		LMSER_EDGES_OFFSETS[1] = 1 - stride; // RIGHT_TOP
		LMSER_EDGES_OFFSETS[2] = -stride; // TOP
		LMSER_EDGES_OFFSETS[3] = -stride - 1; // LEFT_TOP
		LMSER_EDGES_OFFSETS[4] = -1; // LEFT
		LMSER_EDGES_OFFSETS[5] = stride - 1; // LEFT_BOTTOM
		LMSER_EDGES_OFFSETS[6] = stride + 1; // RIGHT_BOTTOM
		LMSER_EDGES_OFFSETS[7] = stride; // BOTTOM
	}
	else {
		LMSER_EDGES_OFFSETS[0] = 1; // RIGHT
		LMSER_EDGES_OFFSETS[1] = -stride; // TOP
		LMSER_EDGES_OFFSETS[2] = -1; // LEFT
		LMSER_EDGES_OFFSETS[3] = stride; // BOTTOM
	}
	
	// Create a pool of points to speedup allocation
	CompVMatPtr poolPoints;
	COMPV_CHECK_CODE_RETURN((CompVMat::newObjStrideless<CompVConnectedComponentLmserLinkedListNodePixelIdx, COMPV_MAT_TYPE_STRUCT>(&poolPoints, 1, (width * height))));
	CompVConnectedComponentLmserLinkedListNodePixelIdx* poolPointsPtr = poolPoints->ptr<CompVConnectedComponentLmserLinkedListNodePixelIdx>();

	// Create a pool of boundary pixels to speedup allocation
	CompVMatPtr poolBoundaryPixels;
	COMPV_CHECK_CODE_RETURN((CompVMat::newObjStrideless<CompVConnectedComponentLmserLinkedListNodeBoundaryPixel, COMPV_MAT_TYPE_STRUCT>(&poolBoundaryPixels, 1, (width * height))));
	CompVConnectedComponentLmserLinkedListNodeBoundaryPixel* poolBoundaryPixelsPtr = poolBoundaryPixels->ptr<CompVConnectedComponentLmserLinkedListNodeBoundaryPixel>();

	// Create a pool of merges to speedup allocation. A region cannot be merged more than once which means: "max-merges = image size"
	CompVMatPtr poolMerges;
	COMPV_CHECK_CODE_RETURN((CompVMat::newObjStrideless<CompVConnectedComponentLmserLinkedListNodeMerge, COMPV_MAT_TYPE_STRUCT>(&poolMerges, 1, (width * height))));
	CompVConnectedComponentLmserLinkedListNodeMerge* poolMergesPtr = poolMerges->ptr<CompVConnectedComponentLmserLinkedListNodeMerge>();

	// A binary mask of accessible pixels. These are the pixels to which the water
	// already has access.
	CompVMatPtr ptr8uAccessible;
	// Finally, to avoid having to perform explicit checks for the image boundary, a
	// 	border of one pixel around the image is used, and the accessible mask is always
	// 	set when a sweep starts.
	const size_t accessibleWidth = stride + 1;
	COMPV_CHECK_CODE_RETURN(CompVMat::newObjStrideless<uint8_t>(&ptr8uAccessible, height + 2, accessibleWidth));
	COMPV_CHECK_CODE_RETURN(CompVMem::set(ptr8uAccessible->ptr<uint8_t>(), 1, ptr8uAccessible->dataSizeInBytes()));
	uint8_t* ptr8uAccessibleRef = ptr8uAccessible->ptr<uint8_t>() + accessibleWidth;
	auto funcPtrSetAccessibility = [&](const size_t ystart, const size_t yend) -> COMPV_ERROR_CODE {
		uint8_t* mt_ptr8uAccessibleRef = ptr8uAccessibleRef + (ystart * stride);
		const int width64_ = (width & -8);
		int x;
		for (size_t y = ystart; y < yend; ++y) {
			for (x = 0; x < width64_; x += 8) {
				*reinterpret_cast<uint64_t*>(&mt_ptr8uAccessibleRef[x]) = 0ull;
			}
			for (; x < width; ++x) {
				mt_ptr8uAccessibleRef[x] = 0;
			}
			mt_ptr8uAccessibleRef += stride;
		}
		return COMPV_ERROR_CODE_S_OK;
	};
	COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
		funcPtrSetAccessibility,
		width,
		height,
		std::max(static_cast<size_t>(COMPV_CORE_LMSER_ACCESSIBILITY_BUILD_SAMPLES_PER_THREAD), static_cast<size_t>(width << 2)) // At least #4 lines per thread
	));
	

	// A priority queue of boundary pixels, where priority is minus the grey - level.
	// These pixels can be thought of as partially flooded pixels in the sense that
	// water has access to the pixel in question, but has either not yet entered, or
	// not yet explored all the edges out from the pixel. Along with the pixel id,
	// an edge number indicating the next edge to be explored can be stored.
	CompVConnectedComponentLmserLinkedListBoundaryPixel boundaryPixels[256];
	CompVConnectedComponentLabelingLmserBoundaryPixelsMgr boundaryPixelsMgr(boundaryPixels);
	uint16_t current_priority = LMSER_HIGHEST_GREYLEVEL;

	// A stack C of component information.Each entry holds the pixels in a component
	// and / or the first and second order moments of the pixels in the component,
	// as well as the size history of the component and the current grey - level
	// at which the component is being processed.The maximum number of entries
	// on the stack will be the number of grey - levels.
	CompVConnectedComponentLmserRefVector stackC;

	// The heap could be dynamically allocated and implemented as linked lists, but
	// we prefer to count the number pixels at each grey - level in a single pre - sweep of
	// the image, just like the standard algorithm does. This allows us to both preallocate
	// and use fixed arrays for the stacks without stacks ever running into one
	// another.The total number of entries in the stacks is the number of pixels plus
	// the number of grey - levels(due to one stop - element for each stack).
	CompVConnectedComponentLabelingLMSERStackMem& stackMem = result_->stackMem();
	CompVConnectedComponentLmserRef stackMemRef_;

	// 1. Clear the accessible pixel mask, the heap of boundary pixels and the component
	// 	stack.Push a dummy - component onto the stack, with grey - level higher
	// 	than any allowed in the image.
	COMPV_CHECK_CODE_RETURN(stackMem.requestNewItem(&stackMemRef_, LMSER_HIGHEST_GREYLEVEL));
	stackC.push_back(stackMemRef_);

	// 2. Make the source pixel(with its first edge) the current pixel, mark it as
	// 	accessible and store the grey - level of it in the variable current_level.
	const uint8_t* ptr8uPixelsRef = ptr8uImage->ptr<const uint8_t>();
	int current_pixel = 0;
	int current_edge = 0;
	ptr8uAccessibleRef[0] = 1;
	uint8_t current_level = ptr8uPixelsRef[0];

__________________________step3__________________________:
	// 3. Push an empty component with current level onto the component stack.
	COMPV_CHECK_CODE_RETURN(stackMem.requestNewItem(&stackMemRef_, current_level));
	stackC.push_back(stackMemRef_);

	// 4. Explore the remaining edges to the neighbors of the current pixel, in order,
	// as follows : For each neighbor, check if the neighbor is already accessible. If it
	// is not, mark it as accessible and retrieve its grey - level.If the grey - level is not
	// lower than the current one, push it onto the heap of boundary pixels. If on
	// the other hand the grey - level is lower than the current one, enter the current
	// pixel back into the queue of boundary pixels for later processing(with the
	// next edge number), consider the new pixel and its grey - level and go to 3.
	do {
		if (current_edge < maxEdges) {
			switch (current_edge) {
			case 0: LMSER_CHECK_EDGE();
			case 1: LMSER_CHECK_EDGE();
			case 2: LMSER_CHECK_EDGE();
			case 3: LMSER_CHECK_EDGE(); if (!b8Connectivity) break;
			case 4: LMSER_CHECK_EDGE();
			case 5: LMSER_CHECK_EDGE();
			case 6: LMSER_CHECK_EDGE();
			case 7: LMSER_CHECK_EDGE();
			}
		}
		  // 5. Accumulate the current pixel to the component at the top of the stack(water
		  // 	saturates the current pixel).
		CompVConnectedComponentLmserRef& top = stackC.back();
		++top->area;
		poolPointsPtr->data = current_pixel;
		poolPointsPtr->link = top->linked_list_points_head, top->linked_list_points_head = poolPointsPtr++; // push_front(current_pixel)

		// 6. Pop the heap of boundary pixels. If the heap is empty, we are done. If the
		// 	returned pixel is at the same grey - level as the previous, go to 4.
		if (current_priority == LMSER_HIGHEST_GREYLEVEL) {
			LMSER_GOTO(we_are_done);
		}

		CompVConnectedComponentLmserLinkedListNodeBoundaryPixel* tail = boundaryPixels[current_priority].tail;
		current_pixel = tail->data & 0xfffffff;
		current_edge = tail->data >> 28;
		boundaryPixelsMgr.pop_back(current_priority);

		// 7. The returned pixel is at a higher grey - level, so we must now process all
		// 	components on the component stack until we reach the higher grey - level.
		// 	This is done with the ProcessStack sub - routine, see below.Then go to 4.
		const uint8_t new_pixel_grey_level = ptr8uPixelsRef[current_pixel];
		if (new_pixel_grey_level != current_level) {
			current_level = new_pixel_grey_level;

			///////////////////////////////////////////////////////
			// The ProcessStack sub - routine is as follows :
			// Sub - routine ProcessStack(new pixel grey level)
			///////////////////////////////////////////////////////
			do {
				// 1. Process component on the top of the stack.The next grey - level is the minimum
				// 	of new pixel grey level and the grey - level for the second component on
				// 	the stack.
				CompVConnectedComponentLmserRef top = stackC.back();
				stackC.pop_back();

				// 2. If new pixel grey level is smaller than the grey - level on the second component
				// on the stack, set the top of stack grey - level to new pixel grey level and return
				// from sub - routine(This occurs when the new pixel is at a grey - level for which
				// there is not yet a component instantiated, so we let the top of stack be that
				// level by just changing its grey - level.
				if (new_pixel_grey_level < stackC.back()->greyLevel) {
					COMPV_CHECK_CODE_RETURN(stackMem.requestNewItem(&stackMemRef_, new_pixel_grey_level));
					stackMemRef_->merge(poolMergesPtr, top);
					stackC.push_back(stackMemRef_);
					break;
				}

				// 3. Remove the top of stack and merge it into the second component on stack
				// as follows : Add the first and second moment accumulators together and / or
				// join the pixel lists.Either merge the histories of the components, or take the
				// history from the winner.Note here that the top of stack should be considered
				// one ’time - step’ back, so its current size is part of the history.Therefore the
				// top of stack would be the winner if its current size is larger than the previous
				// size of second on stack.
				CompVConnectedComponentLmserRef back = stackC.back();
				back->merge(poolMergesPtr, top);

			} while (new_pixel_grey_level > stackC.back()->greyLevel); // 4. If(new pixel grey level>top of stack grey level) go to 1.
		}
	} while (true);

__________________________we_are_done__________________________:
	// Compute stability and collect regions
	CompVConnectedComponentLmserRef master = stackC.back();
	const int input_area = width * height;
	const int min_area_ = static_cast<int>(input_area * minArea());
	const int max_area_ = static_cast<int>(input_area * maxArea());
	const double one_minus_min_diversity = 1.0 - minDiversity();
	const double one_minus_min_diversity_scale = 1.0 / one_minus_min_diversity;
	size_t totalTemporaryStableRegions = 0, totalFinalStableRegions = 0;
	COMPV_CHECK_CODE_RETURN(stackMem.computeVariation(delta())); // MT-friendly
	COMPV_CHECK_CODE_RETURN(stackMem.computeStability(min_area_, max_area_, maxVariation(), totalTemporaryStableRegions)); // MT-friendly
	stackC.resize(totalTemporaryStableRegions);
	master->collectStableRegions(one_minus_min_diversity, one_minus_min_diversity_scale, stackC, totalFinalStableRegions); // MT-unfriendly
	stackC.resize(totalFinalStableRegions);

	/* Building final points from stable regions */
	if (totalFinalStableRegions) {
		const float stride_scale = 1.f / float(stride);
		vecRegions_final.resize(totalFinalStableRegions);
		CompVConnectedComponentLmserRefVector::const_iterator it_src = stackC.begin();
		CompVConnectedComponentLabelingRegionMserVector::iterator it_dst = vecRegions_final.begin();
		auto funcPtrFill = [&](const size_t start, const size_t end) -> COMPV_ERROR_CODE {
			for (size_t i = start; i < end; ++i) {
				size_t index = 0;
				it_dst[i].points.resize(static_cast<size_t>(it_src[i]->area));
				it_src[i]->computeFinalPoints(it_dst[i], index, stride, stride_scale);
			}
			return COMPV_ERROR_CODE_S_OK;
		};
		COMPV_CHECK_CODE_RETURN(CompVThreadDispatcher::dispatchDividingAcrossY(
			funcPtrFill,
			1,
			totalFinalStableRegions,
			COMPV_CORE_LMSER_FILL_REGIONS_SAMPLES_PER_THREAD
		));
	}

	*result = *result_;

	return COMPV_ERROR_CODE_S_OK;
}

COMPV_ERROR_CODE CompVConnectedComponentLabelingLMSER::newObj(CompVConnectedComponentLabelingPtrPtr ccl)
{
	COMPV_CHECK_EXP_RETURN(!CompVCore::isInitialized(), COMPV_ERROR_CODE_E_NOT_INITIALIZED);
	COMPV_CHECK_EXP_RETURN(!ccl, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
	CompVConnectedComponentLabelingPtr _ccl = new CompVConnectedComponentLabelingLMSER();
	COMPV_CHECK_EXP_RETURN(!_ccl, COMPV_ERROR_CODE_E_OUT_OF_MEMORY);
	*ccl = *_ccl;
	return COMPV_ERROR_CODE_S_OK;
}

COMPV_NAMESPACE_END()
