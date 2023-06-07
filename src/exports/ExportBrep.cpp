/* 
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-04-22 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "ExportBrep.h"

// geoml includes
#include "logging/Logging.h"
#include "CNamedShape.h"
#include "ExporterFactory.h"
#include "system/TypeRegistry.h"

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Compound.hxx>

// c includes
#include <cassert>

namespace
{
    template <class T>
    bool toBool(T v)
    {
        return !!v;
    }
}

namespace geoml
{

AUTORUN(ExportBrep)
{
    static CCADExporterBuilder<ExportBrep> brepExporterBuilder;
    ExporterFactory::Instance().RegisterExporter(&brepExporterBuilder, BRepOptions());
    return true;
}


ExporterOptions ExportBrep::GetDefaultOptions() const
{
    return BRepOptions();
}

ShapeExportOptions ExportBrep::GetDefaultShapeOptions() const
{
    return ShapeExportOptions();
}

bool ExportBrep::WriteImpl(const std::string& filename) const
{
    if (filename.empty()) {
       LOG(ERROR) << "Error: Empty filename in ExportBrep::Write.";
       return false;
    }

    if (NShapes() > 1) {
        TopoDS_Compound c;
        BRep_Builder b;
        b.MakeCompound(c);

        for (size_t ishape = 0; ishape < NShapes(); ++ishape) {
            PNamedShape shape = GetShape(ishape);
            if (shape) {
                b.Add(c, shape->Shape());
            }
        }

        // write the file
        return toBool(BRepTools::Write(c, filename.c_str()));
    }
    else if (NShapes() == 1) {
        PNamedShape shape = GetShape(0);
        return toBool(BRepTools::Write(shape->Shape(), filename.c_str()));
    }
    else {
        LOG(WARNING) << "No shapes defined in BRep export. Abort!";
        return false;
    }
}

} // namespace geoml
