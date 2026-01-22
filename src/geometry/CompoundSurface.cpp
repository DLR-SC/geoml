/*
* Copyright (C) 2020 German Aerospace Center (DLR/SC)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "CompoundSurface.h"

#include "geoml/error.h"
#include <algorithm>

namespace geoml
{

CompoundSurface::CompoundSurface(const std::vector<Handle(Geom_BoundedSurface)>& surfaces, const std::vector<Standard_Real>& uparams)
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

gp_Pnt CompoundSurface::Value(Standard_Real u, Standard_Real v) const
{
    Standard_Real tol = 1e-12;
    for (size_t idx = 0; idx < m_surfaces.size(); ++idx) {
        if (m_uparams[idx] - tol <= u && u <= m_uparams[idx+1] + tol) {
            const auto& surface = m_surfaces[idx];

            Standard_Real u1, u2, v1, v2;
            surface->Bounds(u1, u2, v1, v2);

            Standard_Real us = u1 + (u - m_uparams[idx])/(m_uparams[idx+1]-m_uparams[idx])*(u2 - u1);
            Standard_Real vs = (1.-v) * v1 + v*v2;
            return surface->Value(us, vs);
        }
    }
    
    throw Error("U pararmeter out of bounds in CompoundSurface::Value");
}


} // namespace geoml
