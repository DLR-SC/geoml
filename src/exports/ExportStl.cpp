/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
*/
/**
* @file
* @brief  Export routines for CPACS configurations.
*/

#include "ExportStl.h"
#include "ExporterFactory.h"
#include "system/TypeRegistry.h"
#include "CNamedShape.h"

#include "TopoDS_Shape.hxx"
#include "Standard_CString.hxx"
#include "ShapeFix_Shape.hxx"
#include "BRep_Builder.hxx"
#include "BRepTools.hxx"
#include "BRepMesh_IncrementalMesh.hxx"
#include "StlAPI_Writer.hxx"
#include "Interface_Static.hxx"
#include "StlAPI.hxx"

#include <cassert>

namespace geoml 
{

AUTORUN(ExportStl)
{
    static CCADExporterBuilder<ExportStl> stlExporterBuilder;
    ExporterFactory::Instance().RegisterExporter(&stlExporterBuilder, StlOptions());
    return true;
}

// Constructor
ExportStl::ExportStl(const ExporterOptions& opt)
    : CADExporter(opt)
{
}

ExporterOptions ExportStl::GetDefaultOptions() const
{
    return StlOptions();
}

ShapeExportOptions ExportStl::GetDefaultShapeOptions() const
{
    return TriangulatedExportOptions(0.001);
}

bool ExportStl::WriteImpl(const std::string& filename) const
{
    for (size_t ishape = 0; ishape < NShapes(); ++ishape) {
        PNamedShape shape = GetShape(ishape);
        if (shape) {
            BRepMesh_IncrementalMesh(shape->Shape(), GetOptions(ishape).Get<double>("Deflection"));
        }
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
        StlAPI_Writer StlWriter;
        StlWriter.Write(c, const_cast<char*>(filename.c_str()));
        
        return true;
    }
    else if (NShapes() == 1) {
        PNamedShape shape = GetShape(0);
        StlAPI_Writer StlWriter;
        StlWriter.Write(shape->Shape(), const_cast<char*>(filename.c_str()));
        
        return true;
    }
    else {
        return false;
    }
}

} // end namespace geoml
