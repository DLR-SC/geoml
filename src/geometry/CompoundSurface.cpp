/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/

#include "CompoundSurface.h"

#include "geoml/error.h"
#include <algorithm>

namespace geoml
{

CompoundSurface::CompoundSurface(const std::vector<Handle(Geom_BoundedSurface)>& surfaces, const std::vector<double>& uparams)
    : m_surfaces(surfaces)
    , m_uparams(uparams)
{
    if (m_surfaces.size() + 1 != m_uparams.size()) {
        throw Error("Wrong number of parameters in CompoundSurface");
    }

    if (!std::is_sorted(std::begin(m_uparams), std::end(m_uparams))) {
        throw Error("Parameters not sorted in CompoundSurface");
    }
}

gp_Pnt CompoundSurface::Value(double u, double v) const
{
    double tol = 1e-12;
    for (size_t idx = 0; idx < m_surfaces.size(); ++idx) {
        if (m_uparams[idx] - tol <= u && u <= m_uparams[idx+1] + tol) {
            const auto& surface = m_surfaces[idx];

            double u1, u2, v1, v2;
            surface->Bounds(u1, u2, v1, v2);

            double us = u1 + (u - m_uparams[idx])/(m_uparams[idx+1]-m_uparams[idx])*(u2 - u1);
            double vs = (1.-v) * v1 + v*v2;
            return surface->Value(us, vs);
        }
    }
    
    throw Error("U pararmeter out of bounds in CompoundSurface::Value");
}


} // namespace geoml
