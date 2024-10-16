/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CTrimShape.h"

#include "CBooleanOperTools.h"
#include "GEOMAlgo_Splitter.hxx"
#include "BOPBuilderShapeToBRepBuilderShapeAdapter.h"
#include "common/CommonFunctions.h"
#include "geoml/error.h"
#include "CNamedShape.h"

#include <cassert>
#include <string>

#include <BOPAlgo_PaveFiller.hxx>

#include <Precision.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <Geom_Surface.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#if OCC_VERSION_HEX < VERSION_HEX_CODE(7,3,0)
#include <BOPCol_ListOfShape.hxx>
#endif

namespace
{

    static unsigned int itrim = 0;


    // Writes shape and its central face points into brep file (for debugging purposes)
    void WriteDebugShape(const TopoDS_Shape& shape, const char * name)
    {
        TopoDS_Compound c;
        BRep_Builder b;
        b.MakeCompound(c);

        b.Add(c, shape);

        TopTools_IndexedMapOfShape map;
        TopExp::MapShapes(shape,   TopAbs_FACE, map);
        for (int i = 1; i <= map.Extent(); ++i) {
            const TopoDS_Face& face = TopoDS::Face(map(i));
            gp_Pnt p = GetCentralFacePoint(face);
            b.Add(c, BRepBuilderAPI_MakeVertex(p));
        }

        std::stringstream str;
        str << "trim_" << itrim << "_" << name << ".brep";

        BRepTools::Write(c, str.str().c_str());
    }

    TopoDS_Shape GetFacesNotInShape(BRepBuilderAPI_MakeShape& bop, const TopoDS_Shape& originalShape, const TopoDS_Shape& splittedShape, const TopoDS_Shape& shapeToExInclude, TrimOperation op)
    {

        TopoDS_Compound compound;
        BRep_Builder compoundmaker;
        compoundmaker.MakeCompound(compound);

        // add splitted faces to compound
        TopTools_IndexedMapOfShape originMap;
        TopExp::MapShapes(originalShape,   TopAbs_FACE, originMap);
        for (int i = 1; i <= originMap.Extent(); ++i) {
            const TopoDS_Face& face = TopoDS::Face(originMap(i));
            const TopTools_ListOfShape& splits = bop.Modified(face);
            TopTools_ListIteratorOfListOfShape it;
            for (it.Initialize(splits); it.More(); it.Next()) {
                // check if face is inside
                TopoDS_Face splitface = TopoDS::Face(it.Value());
                gp_Pnt p = GetCentralFacePoint(splitface);

                // check if point is in shapeToExclude
                BRepClass3d_SolidClassifier classifier;
                classifier.Load(shapeToExInclude);
                classifier.Perform(p, Precision::Confusion());


                switch (op) {
                case EXCLUDE:
                    if (classifier.State() != TopAbs_IN && classifier.State() != TopAbs_ON) {
                        compoundmaker.Add(compound, splitface);
                    }
                    break;
                case INCLUDE:
                    if (classifier.State() == TopAbs_IN) {
                        compoundmaker.Add(compound, splitface);
                    }
                    break;
                default:
                    printf("illegal operation\n");
                }
            }
        }

        // add remaining faces of original shape
        TopTools_IndexedMapOfShape splitMap;
        TopExp::MapShapes(splittedShape,   TopAbs_FACE, splitMap);
        for (int i = 1; i <= splitMap.Extent(); ++i) {
            const TopoDS_Face& face = TopoDS::Face(splitMap(i));
            // check if faces is from original shape
            Standard_Integer index = originMap.FindIndex(face);
            if (index > 0) {
                const TopoDS_Face& originalFace = TopoDS::Face(originMap.FindKey(index));
                gp_Pnt p = GetCentralFacePoint(originalFace);

                // check if point is in shapeToExclude
                BRepClass3d_SolidClassifier classifier;
                classifier.Load(shapeToExInclude);
                classifier.Perform(p, Precision::Confusion());


                switch (op) {
                case EXCLUDE:
                    if (classifier.State() != TopAbs_IN && classifier.State() != TopAbs_ON) {
                        compoundmaker.Add(compound, originalFace);
                    }
                    break;
                case INCLUDE:
                    if (classifier.State() == TopAbs_IN) {
                        compoundmaker.Add(compound, originalFace);
                    }
                    break;
                default:
                    printf("illegal operation\n");
                }
            }
        }


        return compound;
    }
} // namespace

CTrimShape::CTrimShape(const PNamedShape shape, const PNamedShape trimmingTool, TrimOperation op)
    : _operation(op), _resultshape(), _tool(trimmingTool), _source(shape), _dsfiller(NULL)
{
    _fillerAllocated = false;
    _hasPerformed = false;
}

CTrimShape::CTrimShape(const PNamedShape shape, const PNamedShape trimmingTool, const BOPAlgo_PaveFiller & filler, TrimOperation op)
    : _operation(op), _resultshape(), _tool(trimmingTool), _source(shape)
{
    _fillerAllocated = false;
    _hasPerformed = false;
    _dsfiller = (BOPAlgo_PaveFiller*) &filler;
}

CTrimShape::~CTrimShape()
{
    if (_fillerAllocated && _dsfiller) {
        delete _dsfiller;
        _dsfiller = NULL;
    }
}

CTrimShape::operator PNamedShape()
{
    return NamedShape();
}

void CTrimShape::PrepareFiller()
{
    if (!_tool || !_source) {
        return;
    }

    if (!_dsfiller) {
#if OCC_VERSION_HEX >= VERSION_HEX_CODE(7,3,0)
        TopTools_ListOfShape aLS;
#else
        BOPCol_ListOfShape aLS;
#endif
        aLS.Append(_tool->Shape());
        aLS.Append(_source->Shape());

        _dsfiller = new BOPAlgo_PaveFiller;
        _fillerAllocated = true;

        _dsfiller->SetArguments(aLS);
        _dsfiller->Perform();
    }
}

void CTrimShape::Perform()
{
    if (!_hasPerformed) {
        if (!_source) {
                throw geoml::Error("Null pointer for source argument in CTrimShape", geoml::NULL_POINTER);
        }

        if (!_tool) {
                throw geoml::Error("Null pointer for tool argument in CTrimShape", geoml::NULL_POINTER);
        }

        bool debug = (getenv("GEOML_DEBUG_BOP") != NULL);

        if (debug) {
            WriteDebugShape(_source->Shape(), "source");
            WriteDebugShape(_tool->Shape(), "tool");
        }

        PrepareFiller();
        GEOMAlgo_Splitter splitter;
        BOPBuilderShapeToBRepBuilderShapeAdapter splitAdapter(splitter);
        splitter.AddArgument(_source->Shape());
        splitter.AddTool(_tool->Shape());
        splitter.PerformWithFiller(*_dsfiller);

        if (debug) {
            WriteDebugShape(splitter.Shape(), "split");
        }

        TopoDS_Shape trimmedShape = GetFacesNotInShape(splitAdapter, _source->Shape(), splitter.Shape(), _tool->Shape(), _operation);
        _resultshape = PNamedShape(new CNamedShape(trimmedShape, _source->Name()));
        CBooleanOperTools::MapFaceNamesAfterBOP(splitAdapter, _source, _resultshape);
        CBooleanOperTools::MapFaceNamesAfterBOP(splitAdapter, _tool,   _resultshape);

        // create shell
        _resultshape = CBooleanOperTools::Shellify(_resultshape);

        if (debug) {
            WriteDebugShape(_resultshape->Shape(), "result");
        }

        itrim++;
        _hasPerformed = true;
    }
}

const PNamedShape CTrimShape::NamedShape()
{
    Perform();
    return _resultshape;
}


