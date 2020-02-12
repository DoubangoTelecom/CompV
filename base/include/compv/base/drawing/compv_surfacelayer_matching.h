/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#if !defined(_COMPV_BASE_DRAWING_SURFACE_LAYER_MATCHING_H_)
#define _COMPV_BASE_DRAWING_SURFACE_LAYER_MATCHING_H_

#include "compv/base/compv_config.h"
#include "compv/base/compv_common.h"
#include "compv/base/drawing/compv_surfacelayer.h"

COMPV_NAMESPACE_BEGIN()

COMPV_OBJECT_DECLARE_PTRS(MatchingSurfaceLayer)

class COMPV_BASE_API CompVMatchingSurfaceLayer : public CompVSurfaceLayer
{
protected:
    CompVMatchingSurfaceLayer();
public:
    virtual ~CompVMatchingSurfaceLayer();
	
    virtual COMPV_ERROR_CODE drawMatches(const CompVMatPtr& trainImage, const CompVMatPtr& trainGoodMatches, const CompVMatPtr& queryImage, const CompVMatPtr& queryGoodMatches, const CompVDrawingOptions* options = NULL) = 0;
	virtual CompVSurfacePtr surface() = 0;
	virtual CompVSurfacePtr surfaceTrain() = 0;
	virtual CompVSurfacePtr surfaceQuery() = 0;

private:
    COMPV_VS_DISABLE_WARNINGS_BEGIN(4251 4267)

    COMPV_VS_DISABLE_WARNINGS_END()
};

COMPV_NAMESPACE_END()

#endif /* _COMPV_BASE_DRAWING_SURFACE_LAYER_MATCHING_H_ */
