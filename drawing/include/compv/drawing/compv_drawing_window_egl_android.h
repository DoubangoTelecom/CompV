/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_WINDOW_EGL_ANDROID_H_)
#define _COMPV_WINDOW_EGL_ANDROID_H_

#include "compv/base/compv_config.h"
#include "compv/gl/compv_gl_headers.h"
#if COMPV_OS_ANDROID && defined(HAVE_EGL)
#include "compv/drawing/compv_drawing_window_egl.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

extern const CompVWindowFactory CompVWindowFactoryEGLAndroid;

//
//	CompVWindowEGLAndroid
//
COMPV_OBJECT_DECLARE_PTRS(WindowEGLAndroid)

class CompVWindowEGLAndroid : public CompVWindowEGL
{
protected:
    CompVWindowEGLAndroid(size_t width, size_t height, const char* title);
public:
    virtual ~CompVWindowEGLAndroid();
    COMPV_OBJECT_GET_ID(CompVWindowEGLAndroid);

    virtual EGLNativeWindowType nativeWindow() override /* Overrides(CompVWindowEGL) */;

	virtual COMPV_ERROR_CODE attachToSurface(JNIEnv* jniEnv, jobject javaSurface) override /* Overrides(CompVWindow) */;

	virtual COMPV_ERROR_CODE priv_updateState(COMPV_WINDOW_STATE newState) override /*Overrides(CompVWindowPriv)*/;

    static COMPV_ERROR_CODE newObj(CompVWindowEGLAndroidPtrPtr eglWindow, size_t width, size_t height, const char* title);

private:
    EGLNativeWindowType m_pNativeWindow;
};

COMPV_NAMESPACE_END()

#endif /* COMPV_OS_ANDROID && defined(HAVE_EGL) */

#endif /* _COMPV_WINDOW_EGL_ANDROID_H_ */
