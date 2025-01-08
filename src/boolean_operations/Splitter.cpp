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


#include "Splitter.h"
#include "geoml/error.h"
#include "Logging.h"

#include <sstream>
#include <Standard_Failure.hxx>
#include <Precision.hxx>
#include <Standard_Version.hxx>
#include <TopTools_ListOfShape.hxx>

namespace geoml {

Splitter::Splitter(Shape const& shape, Shape const& cutting_tool) 
    : BRepBuilderAPI_MakeShape_Operation(&m_splitter, {shape, cutting_tool})
    , m_splitter()
{
    TopTools_ListOfShape inputs;
    inputs.Append(shape);
    inputs.Append(cutting_tool);
    m_splitter.SetTools(inputs);
    m_splitter.SetFuzzyValue(Precision::Confusion());
}

Shape Splitter::perform() {

    try {
        m_splitter.Build();
    }
    catch (const Standard_Failure& f) {
        std::stringstream ss;
        ss << "ERROR: splitting of shapes failed: " << f.GetMessageString();
        LOG(ERROR) << ss.str();
        throw geoml::Error(ss.str());
    }

    if (m_splitter.HasErrors()) {
        std::ostringstream oss;
        m_splitter.GetReport()->Dump(oss);
        LOG(ERROR) << "unable to split passed shapes: " << oss.str();
        throw geoml::Error("unable to split passed shapes: " + oss.str());
    }

    return m_splitter.Shape();
}

} // namespace geoml