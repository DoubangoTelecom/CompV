#include "../tests_common.h"

#define TAG_TEST_IMAGE_CHROMA_CONV								"TestImageChromaConv"
#if COMPV_OS_WINDOWS
#	define COMPV_TEST_IMAGE_CHROMA_CONV_IMAGE_FOLDER			"C:/Projects/GitHub/data/colorspace"
#elif COMPV_OS_OSX
#	define COMPV_TEST_IMAGE_CHROMA_CONV_IMAGE_FOLDER			"/Users/mamadou/Projects/GitHub/data/colorspace"
#else
#	define COMPV_TEST_IMAGE_CHROMA_CONV_IMAGE_FOLDER			NULL
#endif
#define COMPV_TEST_IMAGE_CHROMA_CONV_PATH_TO_FILE(filename)		compv_tests_path_from_file(filename, COMPV_TEST_IMAGE_CHROMA_CONV_IMAGE_FOLDER)

#define COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_SRC				COMPV_SUBTYPE_PIXELS_YUV444P
#define COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_DST				COMPV_SUBTYPE_PIXELS_RGB24

#if COMPV_ARCH_ARM
#	define MD5_RCP_RGBA32	"30c8439357f8b66219b849b4d6cfdb16"
#	define MD5_RCP_RGB24	"30c8439357f8b66219b849b4d6cfdb16"
#	define MD5_RCP_NV12		"021a033095979771be143f827ea4d982"
#	define MD5_RCP_NV21		"021a033095979771be143f827ea4d982",
#	define MD5_RCP_YUV420P	"021a033095979771be143f827ea4d982"
#	define MD5_RCP_YUV422P	"945a6dc3660252a132a3845280205f05"
#	define MD5_RCP_YUV444P	"d183c89d27931c74618640ceef30a5f0"
#	define MD5_RCP_YUYV422	"ddebf723b70ada9819909caacb68ea91"
#	define MD5_RCP_UYVY422	"f9f0eff6d8ec12a5c8dac61e455e0a8a"
#else
#	define MD5_RCP_RGBA32	"de27c312745692cde497a31fa0e8ff58"
#	define MD5_RCP_RGB24	"de27c312745692cde497a31fa0e8ff58"
#	define MD5_RCP_NV12		"21173b1ff6b24b58ae505a61ef426882"
#	define MD5_RCP_NV21		"21173b1ff6b24b58ae505a61ef426882",
#	define MD5_RCP_YUV420P	"21173b1ff6b24b58ae505a61ef426882"
#	define MD5_RCP_YUV422P	"aeef11484965ae5c73d85f446fec8c03"
#	define MD5_RCP_YUV444P	"5065846171fce74a7817164c4752edbd"
#	define MD5_RCP_YUYV422	"58ffe1f82d94a75ee767250846f6f542"
#	define MD5_RCP_UYVY422	"b38781fa9c086b35281399b23a1809cd"
#endif

#define COMPV_loopCount											1

static const struct compv_test_image_chroma_conv_test {
	COMPV_SUBTYPE srcPixelFormat;
	const char* srcFilename;
	size_t width;
	size_t height;
	size_t stride;
	COMPV_SUBTYPE dstPixelFormat;
	const char* dstFilename;
	const char* dstMD5;
	const char* dstMD5_rcp; // Using approximate reciprocal (SSSE3), to HSV only
}
COMPV_TEST_IMAGE_CHROMA_CONV_TESTS[] =
{
	/* to YUV444P */
	{ COMPV_SUBTYPE_PIXELS_RGBA32, "equirectangular_1282x720_rgba.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "d59738f116f59328f8e2ec80312d2ab3" },
	{ COMPV_SUBTYPE_PIXELS_ARGB32, "equirectangular_1282x720_argb.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "266a6987b353e0836426a472ea43ac82" },
	{ COMPV_SUBTYPE_PIXELS_BGRA32, "equirectangular_1282x720_bgra.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "d59738f116f59328f8e2ec80312d2ab3" },
	{ COMPV_SUBTYPE_PIXELS_RGB24, "equirectangular_1282x720_rgb.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "d59738f116f59328f8e2ec80312d2ab3" },
	{ COMPV_SUBTYPE_PIXELS_BGR24, "equirectangular_1282x720_bgr.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "d59738f116f59328f8e2ec80312d2ab3" },
	{ COMPV_SUBTYPE_PIXELS_RGB565LE, "equirectangular_1282x720_rgb565le.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "58d4da51e8af9612fa35e8028c2effec" },
	{ COMPV_SUBTYPE_PIXELS_RGB565BE, "equirectangular_1282x720_rgb565be.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "53d957241829dd00bcab558837d6e6cb" },
	{ COMPV_SUBTYPE_PIXELS_BGR565LE, "equirectangular_1282x720_bgr565le.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "7c703cad0d6696016c9dae355ee8949e" },
	{ COMPV_SUBTYPE_PIXELS_BGR565BE, "equirectangular_1282x720_bgr565be.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_YUV444P, "yuv444p.yuv", "53d957241829dd00bcab558837d6e6cb" },

	/* to Grayscale */
	{ COMPV_SUBTYPE_PIXELS_RGBA32, "equirectangular_1282x720_rgba.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "68036672dc25d5400c6fe989801791f3" },
	{ COMPV_SUBTYPE_PIXELS_ARGB32, "equirectangular_1282x720_argb.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "a2cf188bb8d06b043aa007f020b99700" },
	{ COMPV_SUBTYPE_PIXELS_BGRA32, "equirectangular_1282x720_bgra.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "68036672dc25d5400c6fe989801791f3" },
	{ COMPV_SUBTYPE_PIXELS_RGB24, "equirectangular_1282x720_rgb.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "68036672dc25d5400c6fe989801791f3" },
	{ COMPV_SUBTYPE_PIXELS_BGR24, "equirectangular_1282x720_bgr.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "68036672dc25d5400c6fe989801791f3" },
	{ COMPV_SUBTYPE_PIXELS_RGB565LE, "equirectangular_1282x720_rgb565le.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "d2f741f9a219d63f0f51be51c1f5c773" },
	{ COMPV_SUBTYPE_PIXELS_RGB565BE, "equirectangular_1282x720_rgb565be.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "f40fc9b5d0422a80c55acd3c52df6942" },
	{ COMPV_SUBTYPE_PIXELS_BGR565LE, "equirectangular_1282x720_bgr565le.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "b8250e94bdcd9ba7c99495923d0fffbf" },
	{ COMPV_SUBTYPE_PIXELS_BGR565BE, "equirectangular_1282x720_bgr565be.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "f40fc9b5d0422a80c55acd3c52df6942" },
	{ COMPV_SUBTYPE_PIXELS_Y, "equirectangular_1282x720_gray.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "8749c4d0f7730b0b92ef492a2936eb84" },
	{ COMPV_SUBTYPE_PIXELS_NV12, "equirectangular_1282x720_nv12.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_NV21, "equirectangular_1282x720_nv21.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_YUV420P, "equirectangular_1282x720_yuv420p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_YVU420P, "equirectangular_1282x720_yuv420p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_YUV422P, "equirectangular_1282x720_yuv422p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_YUV444P, "equirectangular_1282x720_yuv444p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" },
	{ COMPV_SUBTYPE_PIXELS_YUYV422, "equirectangular_1282x720_yuyv422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" }, // not planar YUV
	{ COMPV_SUBTYPE_PIXELS_UYVY422, "equirectangular_1282x720_uyvy422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_Y, "gray.yuv", "70e11d274bf329c5680956ecdf8357f3" }, // not planar YUV

	/* to RGB24 */
	{ COMPV_SUBTYPE_PIXELS_Y, "equirectangular_1282x720_gray.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "b1b9cd63c872d9e0c2d57e5be883cd0e" },
	{ COMPV_SUBTYPE_PIXELS_RGBA32, "equirectangular_1282x720_rgba.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "ff0fe95cde311c3d4509d768885a6c99" },
	{ COMPV_SUBTYPE_PIXELS_NV12, "equirectangular_1282x720_nv12.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "79d19cba673365246d4de726a32b5897" },
	{ COMPV_SUBTYPE_PIXELS_NV21, "equirectangular_1282x720_nv21.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "79d19cba673365246d4de726a32b5897" },
	{ COMPV_SUBTYPE_PIXELS_YUV420P, "equirectangular_1282x720_yuv420p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "79d19cba673365246d4de726a32b5897" },
	{ COMPV_SUBTYPE_PIXELS_YUV422P, "equirectangular_1282x720_yuv422p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "955e5abb0ad04e499bf7b2f6d9bfdf31" },
	{ COMPV_SUBTYPE_PIXELS_YUV444P, "equirectangular_1282x720_yuv444p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "e99af916cebf41afaa24d0259b7d7ab5" },
	{ COMPV_SUBTYPE_PIXELS_YUYV422, "equirectangular_1282x720_yuyv422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "8247f529fc91dfb91f3688d47846371e" }, // not planar YUV
	{ COMPV_SUBTYPE_PIXELS_UYVY422, "equirectangular_1282x720_uyvy422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGB24, "rgb24.rgb", "39f01e1555d2df71d257dbf934599764" }, // not planar YUV

	/* to RGBA32 */
	{ COMPV_SUBTYPE_PIXELS_Y, "equirectangular_1282x720_gray.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "cbf33e2b3ec35615298c963bf4a50cce" },
	{ COMPV_SUBTYPE_PIXELS_NV12, "equirectangular_1282x720_nv12.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "76d449f6bf380803878f1145999f5b41" },
	{ COMPV_SUBTYPE_PIXELS_NV21, "equirectangular_1282x720_nv21.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "76d449f6bf380803878f1145999f5b41" },
	{ COMPV_SUBTYPE_PIXELS_YUV420P, "equirectangular_1282x720_yuv420p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "76d449f6bf380803878f1145999f5b41" },
	{ COMPV_SUBTYPE_PIXELS_YUV422P, "equirectangular_1282x720_yuv422p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "82527b45e0501ad25321ff55bffcfadd" },
	{ COMPV_SUBTYPE_PIXELS_YUV444P, "equirectangular_1282x720_yuv444p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "c846ea9cb63e8182bfaacd69b1af3cd3" },
	{ COMPV_SUBTYPE_PIXELS_YUYV422, "equirectangular_1282x720_yuyv422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "6966e886a56a1344d5704e4929b9d9eb" }, // not planar YUV
	{ COMPV_SUBTYPE_PIXELS_UYVY422, "equirectangular_1282x720_uyvy422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_RGBA32, "rgba32.rgb", "afb8ed0445fd9d6e52a1e0c33c3ff2b3" }, // not planar YUV

	/* to HSV */
	{ COMPV_SUBTYPE_PIXELS_RGBA32, "equirectangular_1282x720_rgba.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "fc2deefcc66be219b1a0239c45418ce0", MD5_RCP_RGBA32 },
	{ COMPV_SUBTYPE_PIXELS_RGB24, "equirectangular_1282x720_rgb.rgb", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "fc2deefcc66be219b1a0239c45418ce0", MD5_RCP_RGB24 },
	{ COMPV_SUBTYPE_PIXELS_NV12, "equirectangular_1282x720_nv12.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "cd682d6d7f94845f1500bc9acb7b9ecd", MD5_RCP_NV12 },
	{ COMPV_SUBTYPE_PIXELS_NV21, "equirectangular_1282x720_nv21.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "cd682d6d7f94845f1500bc9acb7b9ecd", MD5_RCP_NV21 },
	{ COMPV_SUBTYPE_PIXELS_YUV420P, "equirectangular_1282x720_yuv420p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "cd682d6d7f94845f1500bc9acb7b9ecd", MD5_RCP_YUV420P },
	{ COMPV_SUBTYPE_PIXELS_YUV422P, "equirectangular_1282x720_yuv422p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "6ccc0d8f104323d2f57257d7ff251124", MD5_RCP_YUV422P },
	{ COMPV_SUBTYPE_PIXELS_YUV444P, "equirectangular_1282x720_yuv444p.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "bc82ff037edc5a6c816f54852d35692a", MD5_RCP_YUV444P },
	{ COMPV_SUBTYPE_PIXELS_YUYV422, "equirectangular_1282x720_yuyv422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "a02e42c2079cc16c16f5a18efd50bfdf", MD5_RCP_YUYV422 }, // not planar YUV
	{ COMPV_SUBTYPE_PIXELS_UYVY422, "equirectangular_1282x720_uyvy422.yuv", 1282, 720, 1282, COMPV_SUBTYPE_PIXELS_HSV, "hsv.rgb", "b6d7b878cdbeb800e004b9c6b40d1360", MD5_RCP_UYVY422 }, // not planar YUV
};
static size_t COMPV_TEST_IMAGE_CHROMA_CONV_TESTS_COUNT = sizeof(COMPV_TEST_IMAGE_CHROMA_CONV_TESTS) / sizeof(COMPV_TEST_IMAGE_CHROMA_CONV_TESTS[0]);

COMPV_ERROR_CODE chroma_conv()
{
#	define COMPV_TEST_WRITE_OUTPUT	COMPV_loopCount == 1 && !COMPV_OS_ANDROID  && !COMPV_OS_IPHONE
	COMPV_DEBUG_INFO_EX(TAG_TEST_IMAGE_CHROMA_CONV, "%s(%s -> %s)", __FUNCTION__, CompVGetSubtypeString(COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_SRC), CompVGetSubtypeString(COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_DST));
	COMPV_ERROR_CODE err;
	uint64_t timeStart, timeEnd;
	CompVMatPtr srcImage, dstImage;
	const compv_test_image_chroma_conv_test* test = NULL;
	std::string md5, xmd5;

	COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_S_OK, "Just to avoid 'bail not referenced warning'");

	// Find test
	for (size_t i = 0; i < COMPV_TEST_IMAGE_CHROMA_CONV_TESTS_COUNT; ++i) {
		if (COMPV_TEST_IMAGE_CHROMA_CONV_TESTS[i].srcPixelFormat == COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_SRC && COMPV_TEST_IMAGE_CHROMA_CONV_TESTS[i].dstPixelFormat == COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_DST) {
			test = &COMPV_TEST_IMAGE_CHROMA_CONV_TESTS[i];
			break;
		}
	}
	if (!test) {
		COMPV_DEBUG_ERROR_EX(TAG_TEST_IMAGE_CHROMA_CONV, "Failed to find test: %s -> %s", CompVGetSubtypeString(COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_SRC), CompVGetSubtypeString(COMPV_TEST_IMAGE_CHROMA_CONV_SUBTYPE_DST));
		return COMPV_ERROR_CODE_E_NOT_FOUND;
	}

	// Read source file
	COMPV_CHECK_CODE_BAIL(err = CompVImage::read(test->srcPixelFormat, test->width, test->height, test->stride, COMPV_TEST_IMAGE_CHROMA_CONV_PATH_TO_FILE(test->srcFilename).c_str(), &srcImage));

	// Perform test
	timeStart = CompVTime::nowMillis();
	for (size_t i = 0; i < COMPV_loopCount; ++i) {
		COMPV_CHECK_CODE_BAIL(err = CompVImage::convert(srcImage, test->dstPixelFormat, &dstImage));
	}
	timeEnd = CompVTime::nowMillis();
	COMPV_DEBUG_INFO_EX(TAG_TEST_IMAGE_CHROMA_CONV, "Elapsed time = [[[ %" PRIu64 " millis ]]]", (timeEnd - timeStart));

#if COMPV_TEST_WRITE_OUTPUT
	COMPV_CHECK_CODE_BAIL(err = compv_tests_write_to_file(dstImage, test->dstFilename));
#endif

	md5 = compv_tests_md5(dstImage);
	COMPV_DEBUG_INFO_EX(TAG_TEST_IMAGE_CHROMA_CONV, "MD5:%s", md5.c_str());
	xmd5 = (test->dstPixelFormat == COMPV_SUBTYPE_PIXELS_HSV && compv_tests_is_rcp()) ? test->dstMD5_rcp : test->dstMD5;
	COMPV_CHECK_EXP_BAIL(xmd5.compare(md5) != 0, (err = COMPV_ERROR_CODE_E_UNITTEST_FAILED), "MD5 mismatch");

bail:
	return err;
}
