/*
* Copyright (C) 2007-2025 German Aerospace Center (DLR/SC)
*
* Created: 2025-01-08 Jan Kleinert <Jan.Kleinert@dlr.de>
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

#pragma once 

#include "geoml/naming_choosing/Shape.h"
#include "topology/BRepBuilderAPI_MakeShape_Operation.h"
#include "geoml/geoml.h"
#include "BRepAlgoAPI_Splitter.hxx"

namespace geoml {

class Splitter : public BRepBuilderAPI_MakeShape_Operation
{
public:
    GEOML_API_EXPORT Splitter(Shape const& shape, Shape const& cutting_tool);

    GEOML_API_EXPORT Shape perform() override final;

private:
    BRepAlgoAPI_Splitter m_splitter;
};

} // namespace geoml