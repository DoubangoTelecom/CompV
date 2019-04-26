#include "../tests_common.h"

#define TAG_TEST								"TestHogStd"
#if COMPV_OS_WINDOWS
#	define COMPV_TEST_IMAGE_FOLDER				"C:/Projects/GitHub/data/test_images"
#elif COMPV_OS_OSX
#	define COMPV_TEST_IMAGE_FOLDER				"/Users/mamadou/Projects/GitHub/data/test_images"
#else
#	define COMPV_TEST_IMAGE_FOLDER				NULL
#endif
#define COMPV_TEST_PATH_TO_FILE(filename)		compv_tests_path_from_file(filename, COMPV_TEST_IMAGE_FOLDER)

#define FILE_NAME_EQUIRECTANGULAR		"equirectangular_1282x720_gray.yuv"
#define FILE_NAME_OPENGLBOOK			"opengl_programming_guide_8th_edition_200x258_gray.yuv"
#define FILE_NAME_GRIOTS				"mandekalou_480x640_gray.yuv"

#define LOOP_COUNT				1
#define FILE_NAME				FILE_NAME_EQUIRECTANGULAR
#define NORM					COMPV_HOG_BLOCK_NORM_NONE
#define INTERP					COMPV_HOG_INTERPOLATION_BILINEAR_LUT

static const struct compv_unittest_hogstd {
	const char* filename;
	size_t width;
	size_t height;
	size_t stride;
	int norm;
	int interp;
	const char* md5;
	const char* md5_fma;
}
COMPV_UNITTEST_HOGSTD[] =
{
	// == COMPV_HOG_INTERPOLATION_BILINEAR ==
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_NONE, COMPV_HOG_INTERPOLATION_BILINEAR, "b03b5891e9d1845c231b71470bf0278d", "a6ebc3cd0dbe8d65112badb9288345c0" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L1, COMPV_HOG_INTERPOLATION_BILINEAR, "e92312dfa9698006dd6499d6e54f4563", "4ae71d377810ac5d120f839c7fd05e6d" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L1SQRT, COMPV_HOG_INTERPOLATION_BILINEAR, "00cba3ac411dde65fff6d733be7289e6", "ad7342c69929eda0d45d735ffb6b7236" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L2, COMPV_HOG_INTERPOLATION_BILINEAR, "ee03be50d8f26ac0c6877bf59f9bfdc6", "f4026800dc2223ab5943bca390dd1f29" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L2HYS, COMPV_HOG_INTERPOLATION_BILINEAR, "9a3c3ade6a4e50a95ebfe226c3f1ce0f", "4dfc2e21fd381585174d8252bad7d3df" },

	// == COMPV_HOG_INTERPOLATION_BILINEAR_LUT ==
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_NONE, COMPV_HOG_INTERPOLATION_BILINEAR_LUT, "2aa07f8bba419a831cc1fbe5afdc95b7", "2aa07f8bba419a831cc1fbe5afdc95b7" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L1, COMPV_HOG_INTERPOLATION_BILINEAR_LUT, "5463c1490ebff48572f571cc7043b5b3", "5463c1490ebff48572f571cc7043b5b3" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L1SQRT, COMPV_HOG_INTERPOLATION_BILINEAR_LUT, "758004066db24e1f09b61d65c20a623a", "758004066db24e1f09b61d65c20a623a" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L2, COMPV_HOG_INTERPOLATION_BILINEAR_LUT, "e2330b2ac19149c7067e4924954b0b33", "e2330b2ac19149c7067e4924954b0b33" },
	{ FILE_NAME_EQUIRECTANGULAR, 1282, 720, 1282, COMPV_HOG_BLOCK_NORM_L2HYS, COMPV_HOG_INTERPOLATION_BILINEAR_LUT, "0e7cca02496084295d636d21adc739f8", "0e7cca02496084295d636d21adc739f8" },

};
static const size_t COMPV_UNITTEST_HOGSTD_COUNT = sizeof(COMPV_UNITTEST_HOGSTD) / sizeof(COMPV_UNITTEST_HOGSTD[0]);

COMPV_ERROR_CODE hogstd()
{
	CompVHOGPtr hogStd;
	const compv_unittest_hogstd* test = nullptr;
	CompVMatPtr image, features;

	// Find test
	for (size_t i = 0; i < COMPV_UNITTEST_HOGSTD_COUNT; ++i) {
		if (COMPV_UNITTEST_HOGSTD[i].norm == NORM && COMPV_UNITTEST_HOGSTD[i].interp == INTERP && std::string(COMPV_UNITTEST_HOGSTD[i].filename).compare(FILE_NAME) == 0) {
			test = &COMPV_UNITTEST_HOGSTD[i];
			break;
		}
	}
	if (!test) {
		COMPV_DEBUG_ERROR_EX(TAG_TEST, "Failed to find test");
		return COMPV_ERROR_CODE_E_NOT_FOUND;
	}

	COMPV_CHECK_CODE_RETURN(CompVHOG::newObj(&hogStd, COMPV_HOGS_ID,
		CompVSizeSz(8, 8), // blockSize(8, 8),
		CompVSizeSz(4, 4), // blockStride(4, 4),
		CompVSizeSz(8, 8), // cellSize(8, 8),
		9, // nbins
		test->norm, // blockNorm
		true, // gradientSigned
		test->interp // interpolation
	));
	COMPV_CHECK_CODE_RETURN(CompVImage::read(COMPV_SUBTYPE_PIXELS_Y, test->width, test->height, test->stride, COMPV_TEST_PATH_TO_FILE(test->filename).c_str(), &image));
	
	const uint64_t timeStart = CompVTime::nowMillis();
	for (size_t i = 0; i < LOOP_COUNT; ++i) {
		COMPV_CHECK_CODE_RETURN(hogStd->process(image, &features));
	}
	const uint64_t timeEnd = CompVTime::nowMillis();
	COMPV_DEBUG_INFO_EX(TAG_TEST, "HogStd Elapsed time = [[[ %" PRIu64 " millis ]]]", (timeEnd - timeStart));

	COMPV_DEBUG_INFO_EX(TAG_TEST, "MD5:%s", compv_tests_md5(features).c_str());
	COMPV_CHECK_EXP_RETURN(compv_tests_md5(features).compare(compv_tests_is_fma_enabled() ? test->md5_fma : test->md5) != 0, COMPV_ERROR_CODE_E_UNITTEST_FAILED, "S-HOG failed");

	return COMPV_ERROR_CODE_S_OK;
}
