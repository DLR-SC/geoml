/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*
* Created: 2020-09-10 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CONCATSURFACES_H
#define CONCATSURFACES_H

#include "geoml_internal.h"
#include <vector>
#include <Geom_BSplineSurface.hxx>
#include <optional>

namespace geoml
{

enum class ConcatDir
{
    u,
    v
};

class ConcatSurfaces
{
public:
    /**
     * @brief Concatenes multiple surfaces along the given directions
     *
     * Note: The surfaces must follow in the correct order in the specified direction
     *
     * @param surfaces      The surfaces to concatenate
     * @param surfaceParams A vector of parameters that define the parameters at the final surface, at which
     *                      the input surfaces are stitched.
     * @param dir           Direction to concatenate
     */
    GEOML_EXPORT ConcatSurfaces(const std::vector<Handle(Geom_BSplineSurface)>& surfaces,
                                    const std::vector<double>& surfaceParams, ConcatDir dir);

    /**
     * @brief This allows the algorithm to make the knot sequence of each input
     * surface uniform.
     *
     * This might be useful, if the input surfaces have very different knot distributions
     * leaving a resulting in a bad balanced knot vector of the final surfaces.
     * This approximation comes however with an error.
     *
     * @param nUSeg Number of knot segments in u dir to approximate
     * @param nVSeg Number of knot segments in v dir to approximate
     */
    GEOML_EXPORT void SetMakeKnotsUniformEnabled(unsigned int nUSeg, unsigned int nVSeg);

    /**
     + @brief Returns the final surface
     */
    GEOML_EXPORT Handle(Geom_BSplineSurface) Surface() const;

private:
    std::vector<Handle(Geom_BSplineSurface)> m_surfaces;
    std::vector<double> m_params;
    ConcatDir m_dir;

    struct SegmentsSize {
        unsigned int nu, nv;
    };
    std::optional<SegmentsSize> m_approxSegments;
};

} // namespace geoml

#endif // CONCATSURFACES_H
