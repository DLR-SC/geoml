/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/

#ifndef COMPOUNDSURFACE_H
#define COMPOUNDSURFACE_H

#include "geoml_internal.h"

#include <gp_Pnt.hxx>
#include <Geom_BoundedSurface.hxx>
#include <vector>

namespace geoml
{

class CompoundSurface
{
public:
    GEOML_EXPORT CompoundSurface() = default;
    /**
     * @brief CompoundSurface
     * @param surfaces A list of (adjacent) surfaces
     * @param uparams Ordered list of parameters. Must be one entry more than surfaces
     */
    GEOML_EXPORT CompoundSurface(const std::vector<Handle(Geom_BoundedSurface)>& surfaces,
                                     const std::vector<double>& uparams);

    GEOML_EXPORT gp_Pnt Value(double u, double v) const;

    GEOML_EXPORT CompoundSurface& operator=(const CompoundSurface& other) = default;

private:
    std::vector<Handle(Geom_BoundedSurface)> m_surfaces;
    std::vector<double> m_uparams;
};

} // namespace geoml

#endif // COMPOUNDSURFACE_H
