/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_GL_SURFACE_LAYER_GL_SINGLE_H_)
#define _COMPV_GL_SURFACE_LAYER_GL_SINGLE_H_

#include "compv/gl/compv_gl_config.h"
#include "compv/gl/compv_gl_headers.h"
#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
#include "compv/base/drawing/compv_surfacelayer_single.h"
#include "compv/gl/compv_gl_surface.h"

#if defined(_COMPV_API_H_)
#error("This is a private file and must not be part of the API")
#endif

COMPV_NAMESPACE_BEGIN()

class CompVGLWindow;

COMPV_OBJECT_DECLARE_PTRS(GLSingleSurfaceLayer)

class CompVGLSingleSurfaceLayer : public CompVSingleSurfaceLayer
{
protected:
    CompVGLSingleSurfaceLayer();
public:
    virtual ~CompVGLSingleSurfaceLayer();
    COMPV_OBJECT_GET_ID(CompVGLSingleSurfaceLayer);

    // Overrides(CompSurfaceLayer)
	virtual CompVSurfacePtr cover() override;
    virtual COMPV_ERROR_CODE blit() override;

    COMPV_ERROR_CODE updateSize(size_t newWidth, size_t newHeight);
    COMPV_ERROR_CODE close();

    static COMPV_ERROR_CODE newObj(CompVGLSingleSurfaceLayerPtrPtr layer, size_t width, size_t height);

private:
    COMPV_VS_DISABLE_WARNINGS_BEGIN(4251 4267)
    CompVGLSurfacePtr m_ptrSurfaceGL;
    COMPV_VS_DISABLE_WARNINGS_END()
};

COMPV_NAMESPACE_END()

#endif /* defined(HAVE_OPENGL) || defined(HAVE_OPENGLES) */

#endif /* _COMPV_GL_SURFACE_LAYER_GL_SINGLE_H_ */
