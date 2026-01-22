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
                                     const std::vector<Standard_Real>& uparams);

    GEOML_EXPORT gp_Pnt Value(Standard_Real u, Standard_Real v) const;

    GEOML_EXPORT CompoundSurface& operator=(const CompoundSurface& other) = default;

private:
    std::vector<Handle(Geom_BoundedSurface)> m_surfaces;
    std::vector<Standard_Real> m_uparams;
};

} // namespace geoml

#endif // COMPOUNDSURFACE_H
