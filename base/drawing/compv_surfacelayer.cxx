/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/drawing/compv_surfacelayer.h"

COMPV_NAMESPACE_BEGIN()

compv_surfacelayer_id_t CompVSurfaceLayer::s_nSurfaceLayerId = 0;

CompVSurfaceLayer::CompVSurfaceLayer()
    : CompVObj()
    , m_nId(compv_atomic_add(&CompVSurfaceLayer::s_nSurfaceLayerId, 1))
{

}

CompVSurfaceLayer::~CompVSurfaceLayer()
{

}

COMPV_NAMESPACE_END()

