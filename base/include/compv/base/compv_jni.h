/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_JNI_H_)
#define _COMPV_BASE_JNI_H_

#include "compv/base/compv_config.h"
#if defined(HAVE_JNI_H) || COMPV_OS_ANDROID
#include <jni.h>

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

class COMPV_BASE_API CompVJNI
{
public:
    static std::string toString(JNIEnv* jEnv, jstring jstr);
	static std::string toString(JNIEnv* jEnv, jthrowable exc);

private:
	COMPV_VS_DISABLE_WARNINGS_BEGIN(4251 4267)
		COMPV_VS_DISABLE_WARNINGS_END()
};

#define COMPV_jni_checkException(env, occurred) \
	*occurred = false; \
	if ((env)) { \
		jthrowable exc = (env)->ExceptionOccurred(); \
		if (exc) { \
			*occurred = true; \
			(env)->ExceptionDescribe(); \
			(env)->ExceptionClear(); \
			const std::string jniError = CompVJNI::toString(jEnv, exc); \
			COMPV_DEBUG_ERROR("JNI exception: %s", jniError.c_str()); \
		} \
	}
#define COMPV_jni_checkException1(env) { static bool __COMPV_occurred; COMPV_jni_checkException((env), &__COMPV_occurred) }
	

#define COMPV_jni_DeleteLocalRef(env, localRef) if (localRef) { env->DeleteLocalRef(localRef); localRef = NULL; }
#define COMPV_jni_DeleteGlobalRef(env, globalRef) if (globalRef) { env->DeleteGlobalRef(globalRef); globalRef = NULL; }

COMPV_NAMESPACE_END()

#endif /* defined(HAVE_JNI_H) || COMPV_OS_ANDROID */

#endif /* _COMPV_BASE_JNI_H_ */
