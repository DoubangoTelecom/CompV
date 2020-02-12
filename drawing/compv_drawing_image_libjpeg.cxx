/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/drawing/compv_drawing_image_libjpeg.h"

#if defined(HAVE_JPEGLIB_H)

#include "compv/base/compv_mem.h"
#include "compv/base/compv_buffer.h"
#include "compv/base/compv_debug.h"
#include "compv/base/compv_fileutils.h"
#include "compv/base/math/compv_math.h"
#include "compv/base/image/compv_image.h"

// 'jmorecfg.h' is patched to avoid "JPEG parameter struct mismatch: library thinks size is 632, caller expects 600" error, by using correct typedef for 'boolean'
extern "C" {
#include <jpeglib.h>
}
#include <setjmp.h>

#define kModuleNameLibjpeg "libjpeg"
#define kReadDataTrue	true
#define kReadDataFalse	false

COMPV_NAMESPACE_BEGIN()

// Private function
static COMPV_ERROR_CODE decode_jpeg(const char* filename, bool readData, uint8_t** rawdata, int32_t *width, int32_t *stride, int32_t *height, COMPV_SUBTYPE* pixelFormat);

COMPV_ERROR_CODE libjpegDecodeFile(const char* filePath, CompVMatPtrPtr mat)
{
    COMPV_CHECK_EXP_RETURN(!filePath || !mat, COMPV_ERROR_CODE_E_INVALID_PARAMETER);
    COMPV_ERROR_CODE err_ = COMPV_ERROR_CODE_S_OK;
    uint8_t* rawdata_ = NULL;
    int32_t width_, stride_, height_;
    COMPV_SUBTYPE pixelFormat_;
    CompVImageInfo info = {};

    // Get image data without decoding
    COMPV_CHECK_CODE_BAIL(err_ = libjpegDecodeInfo(filePath, info));
    // Alloc image data
    COMPV_CHECK_CODE_BAIL(err_ = CompVImage::newObj8u(mat, info.pixelFormat, static_cast<size_t>(info.width), static_cast<size_t>(info.height)));
    rawdata_ = const_cast<uint8_t*>((*mat)->ptr<uint8_t>());
    width_ = static_cast<int32_t>((*mat)->cols());
    height_ = static_cast<int32_t>((*mat)->rows());
    stride_ = static_cast<int32_t>((*mat)->strideInBytes());
    pixelFormat_ = static_cast<COMPV_SUBTYPE>((*mat)->subType());
    COMPV_CHECK_CODE_BAIL(err_ = decode_jpeg(filePath, kReadDataTrue, &rawdata_, &width_, &stride_, &height_, &pixelFormat_));

bail:
    if (COMPV_ERROR_CODE_IS_NOK(err_)) {
        *mat = NULL;
    }
    return err_;
}

COMPV_ERROR_CODE libjpegDecodeInfo(const char* filePath, CompVImageInfo& info)
{
    COMPV_ERROR_CODE err_ = COMPV_ERROR_CODE_S_OK;

    if (!filePath) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Invalid parameter");
        COMPV_CHECK_CODE_BAIL(err_ = COMPV_ERROR_CODE_E_INVALID_PARAMETER);
    }
    COMPV_CHECK_CODE_BAIL(err_ = decode_jpeg(filePath, kReadDataFalse, NULL, &info.width, &info.stride, &info.height, &info.pixelFormat));
    info.format = COMPV_IMAGE_FORMAT_JPEG;

bail:
    return err_;
}

struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */

    jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

static void my_error_exit(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

#if 0
    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);
#else
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message) (cinfo, buffer);
    COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "error message: %s", buffer);
#endif
    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

static COMPV_ERROR_CODE decode_jpeg(const char* filename, bool readData, uint8_t** rawdata, int32_t *width, int32_t *stride, int32_t *height, COMPV_SUBTYPE* pixelFormat)
{
    struct jpeg_decompress_struct cinfo = { 0 };
    bool cinfo_created = false;
    bool rawdata_allocated = false;
    /* We use our private extension JPEG error handler.
    * Note that this struct must live as long as the main JPEG parameter
    * struct, to avoid dangling-pointer problems.
    */
    struct my_error_mgr jerr = {  };
    /* More stuff */
    FILE * infile = NULL;		/* source file */
    JSAMPARRAY buffer = NULL;		/* Output row buffer */
    int row_width_bytes;		/* physical row width in output buffer */
    int row_stride_bytes;
    COMPV_ERROR_CODE err = COMPV_ERROR_CODE_S_OK;

    /* In this example we want to open the input file before doing anything else,
    * so that the setjmp() error recovery below can assume the file is open.
    * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
    * requires it in order to read binary files.
    */

    if ((readData && !rawdata) || !width || !height) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Invalid parameter");
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_INVALID_PARAMETER);
    }

    if ((infile = CompVFileUtils::open(filename, "rb")) == NULL) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Can't open %s", filename);
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_FAILED_TO_OPEN_FILE); // can safely return
    }

    /* Step 1: allocate and initialize JPEG decompression object */

    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "setjmp failed");
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_SYSTEM);
    }
    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);
    cinfo_created = true;

    /* Step 2: specify data source (eg, a file) */

    jpeg_stdio_src(&cinfo, infile);

    /* Step 3: read file parameters with jpeg_read_header() */

    if (!jpeg_read_header(&cinfo, TRUE)) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "jpeg_read_header failed");
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_THIRD_PARTY_LIB);
    }

    // For now only RGB is supported
    switch (cinfo.out_color_space) {
    case JCS_GRAYSCALE:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_Y;
        break;
    case JCS_RGB:
    case JCS_EXT_RGB:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_RGB24;
        break;
    case JCS_EXT_BGR:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_BGR24;
        break;
    case JCS_EXT_RGBA:
    case JCS_EXT_RGBX:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_RGBA32;
        break;
    case JCS_EXT_BGRA:
    case JCS_EXT_BGRX:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_BGRA32;
        break;
    case JCS_EXT_ABGR:
    case JCS_EXT_XBGR:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_ABGR32;
        break;
    case JCS_EXT_ARGB:
    case JCS_EXT_XRGB:
        *pixelFormat = COMPV_SUBTYPE_PIXELS_ARGB32;
        break;
    default:
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Invalid color space %d", cinfo.out_color_space);
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_INVALID_PIXEL_FORMAT);
        break;
    }

    /* Step 5: Start decompressor */

    if (!jpeg_start_decompress(&cinfo)) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "jpeg_start_decompress failed");
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_THIRD_PARTY_LIB);
    }

    // When rawdata is already allocated then, we must have valid size
    if (readData && *rawdata && (static_cast<JDIMENSION>(*width) != cinfo.output_width || static_cast<JDIMENSION>(*height) != cinfo.output_height || static_cast<JDIMENSION>(*stride) < cinfo.output_width)) {
        COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Data already allocated, you must define correct size");
        COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_INVALID_PARAMETER);
    }

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    if (static_cast<JDIMENSION>(*stride) < cinfo.output_width) { // do not override if data already defined
        *stride = static_cast<int32_t>(CompVMem::alignForward((cinfo.output_width * sizeof(uint8_t)), COMPV_ALIGNV_SIMD_DEFAULT));
    }

    if (readData) {
        /* We may need to do some setup of our own at this point before reading
        * the data.  After jpeg_start_decompress() we have the correct scaled
        * output image dimensions available, as well as the output colormap
        * if we asked for color quantization.
        * In this example, we need to make an output work buffer of the right size.
        */
        /* JSAMPLEs per row in output buffer */
        row_width_bytes = cinfo.output_width * cinfo.output_components;
        row_stride_bytes = (*stride) * cinfo.output_components;
        /* Make a one-row-high sample array that will go away when done with image */
        buffer = (*cinfo.mem->alloc_sarray)
                 ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_width_bytes, 1);

        if (!*rawdata) {
            *rawdata = (uint8_t*)CompVMem::malloc(row_stride_bytes * cinfo.output_height);
            rawdata_allocated = true;
        }
        if (!*rawdata) {
            COMPV_DEBUG_ERROR_EX(kModuleNameLibjpeg, "Failed to allocate %d bytes", row_width_bytes * cinfo.output_height);
            COMPV_CHECK_CODE_BAIL(err = COMPV_ERROR_CODE_E_OUT_OF_MEMORY);
        }

        /* Step 6: while (scan lines remain to be read) */
        /*           jpeg_read_scanlines(...); */

        /* Here we use the library's state variable cinfo.output_scanline as the
        * loop counter, so that we don't have to keep track ourselves.
        */
        while (cinfo.output_scanline < cinfo.output_height) {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
            * Here the array is only one element long, but you could ask for
            * more than one scanline at a time if that's more convenient.
            */
            (void)jpeg_read_scanlines(&cinfo, buffer, 1);
            CompVMem::copy((*rawdata) + ((cinfo.output_scanline - 1) * row_stride_bytes), buffer[0], row_width_bytes);
        }
    }

bail:
    if (cinfo_created) { // null exception in libjpeg if create is nok
        /* Step 7: Finish decompression */

        if (readData) {
            (void)jpeg_finish_decompress(&cinfo);
        }
        /* We can ignore the return value since suspension is not possible
        * with the stdio data source.
        */
        jpeg_destroy_decompress(&cinfo);
    }
    if (infile) {
        fclose(infile);
    }

    if (COMPV_ERROR_CODE_IS_NOK(err) && rawdata_allocated) {
        CompVMem::free((void**)rawdata);
    }

    return err;
}

COMPV_NAMESPACE_END()

#endif /* defined(HAVE_JPEGLIB_H) */
