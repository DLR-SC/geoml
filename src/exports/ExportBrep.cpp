/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)

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
