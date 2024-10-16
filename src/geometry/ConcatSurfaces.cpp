/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*
* Created: 2020-09-10 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "ConcatSurfaces.h"

#include <BSplineAlgorithms.h>
#include <geoml/error.h>

#include <algorithm>

namespace geoml
{

ConcatSurfaces::ConcatSurfaces(const std::vector<Handle(Geom_BSplineSurface)>& surfaces,
                                         const std::vector<double>& surfaceParams, ConcatDir dir)
    : m_surfaces(surfaces)
    , m_params(surfaceParams)
    , m_dir(dir)
{

    if (surfaces.size() + 1 != surfaceParams.size()) {
        throw Error("Surfaces dont match parameters: Number of parameters != number of surfaces + 1");
    }

    if (dir ==ConcatDir::v) {
        throw Error("ConcatSurfaces in v-direction not yet implemented");
    }

    std::sort(m_params.begin(), m_params.end());
}

void ConcatSurfaces::SetMakeKnotsUniformEnabled(unsigned int nUSeg, unsigned int nVSeg)
{
    if (nUSeg > 0 && nVSeg > 0) {
        m_approxSegments = SegmentsSize{nUSeg, nVSeg};
    }
    else {
        m_approxSegments.reset();
    }
}

Handle(Geom_BSplineSurface) ConcatSurfaces::Surface() const
{
    // create a copy of all surfaces
    std::vector<Handle(Geom_BSplineSurface)> surfaces;
    std::transform(std::begin(m_surfaces), std::end(m_surfaces), std::back_inserter(surfaces),
                   [](const Handle(Geom_BSplineSurface) & surf) {
                       return Handle(Geom_BSplineSurface)::DownCast(surf->Copy());
                   });

    if (m_approxSegments) {
        std::transform(
            std::begin(surfaces), std::end(surfaces), std::begin(surfaces), [this](Handle(Geom_BSplineSurface) & surf) {
                return BSplineAlgorithms::makeKnotsUniform(surf, m_approxSegments->nu, m_approxSegments->nv);
            });
    }

    for (size_t surfIdx = 0; surfIdx < surfaces.size(); ++surfIdx) {
        BSplineAlgorithms::reparametrizeBSpline(*surfaces[surfIdx], m_params[surfIdx], m_params[surfIdx + 1], 0.,
                                                     1., 1e-10);
    }

    for (size_t surfIdx = 1; surfIdx < surfaces.size(); ++surfIdx) {
        surfaces[0] = BSplineAlgorithms::concatSurfacesUDir(surfaces[0], surfaces[surfIdx]);
    }

    return surfaces[0];
}

} // namespace geoml
