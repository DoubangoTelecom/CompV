/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/

#include "compv/gl/compv_gl.h"
#include "compv/gl/compv_gl_headers.h"
#include "compv/gl/compv_gl_vao.h"
#include "compv/gl/compv_gl_utils.h"
#include "compv/gl/compv_gl_common.h"
#include "compv/gl/compv_gl_freetype.h"
#include "compv/base/compv_debug.h"
#include "compv/base/compv_base.h"

#define COMPV_THIS_CLASSNAME	"CompVGL"

COMPV_NAMESPACE_BEGIN()

bool CompVGL::s_bInitialized = false;

COMPV_ERROR_CODE CompVGL::init()
{
    if (CompVGL::isInitialized()) {
        return COMPV_ERROR_CODE_S_OK;
    }

    COMPV_ERROR_CODE err = COMPV_ERROR_CODE_S_OK;

    COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "Initializing [gl] module (v %s)...", COMPV_VERSION_STRING);

#if COMPV_GL_DEBUG
    COMPV_DEBUG_INFO_CODE_NOT_OPTIMIZED("COMPV_GL_DEBUG enabled, OpenGL operations will be very slooooow. Sad!!");
#endif

    /* Print Android API version */
#if COMPV_OS_ANDROID
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "android API version: %d", __ANDROID_API__);
#endif

#if defined(HAVE_GL_GLEW_H)
    COMPV_DEBUG_INFO("GLEW version being used: %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
#endif /* HAVE_GL_GLEW_H */

#if	defined(HAVE_OPENGLES)
    COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "OpenGL-ES implementation enabled");
#	if defined(GL_ES_VERSION_3_0) && GL_ES_VERSION_3_0
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "OpenGL-ES 3.0 API");
#	elif defined(GL_ES_VERSION_2_0) && GL_ES_VERSION_2_0
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "OpenGL-ES 2.0 API");
#	else
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "OpenGL-ES 1.0 API");
#	endif
#elif defined(HAVE_OPENGL)
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "OpenGL implementation enabled");
#endif

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
    COMPV_CHECK_CODE_BAIL(err = CompVGLVAO::init());
#	if HAVE_FREETYPE
	COMPV_CHECK_CODE_BAIL(err = CompVGLFreeType::init());
#	endif /* HAVE_FREETYPE */
#endif

#if 1 // Just to make sure "bail"
	COMPV_CHECK_CODE_BAIL(err);
#endif

    CompVGL::s_bInitialized = true;
	COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "GL module initialized");

bail:
    if (COMPV_ERROR_CODE_IS_NOK(err)) {
        COMPV_CHECK_CODE_NOP(CompVGL::deInit());
    }

    return err;
}

COMPV_ERROR_CODE CompVGL::deInit()
{
    COMPV_CHECK_CODE_ASSERT(CompVBase::deInit());

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
    COMPV_CHECK_CODE_ASSERT(CompVGLVAO::deInit());
#	if HAVE_FREETYPE
	COMPV_CHECK_CODE_ASSERT(CompVGLFreeType::deInit());
#	endif /* HAVE_FREETYPE */
#endif
	
    CompVGL::s_bInitialized = false;

    return COMPV_ERROR_CODE_S_OK;
}

#if defined(HAVE_GL_GLEW_H)
COMPV_ERROR_CODE CompVGL::glewInit()
{
    static bool sbGlewInitialized = false;
    if (!sbGlewInitialized) {
        COMPV_CHECK_EXP_RETURN(!CompVGLUtils::isGLContextSet(), COMPV_ERROR_CODE_E_GL_NO_CONTEXT);
        GLenum glewErr = ::glewInit();
        if (GLEW_OK != glewErr) {
            COMPV_DEBUG_ERROR_EX(COMPV_THIS_CLASSNAME, "glewInit for [gl] module failed: %s", glewGetErrorString(glewErr));
            COMPV_CHECK_CODE_RETURN(COMPV_ERROR_CODE_E_GLEW);
        }
        COMPV_DEBUG_INFO_EX(COMPV_THIS_CLASSNAME, "glewInit for [gl] module succeeded");
        sbGlewInitialized = true;
    }
    return COMPV_ERROR_CODE_S_OK;
}
#endif /* HAVE_GL_GLEW_H */

COMPV_NAMESPACE_END()

