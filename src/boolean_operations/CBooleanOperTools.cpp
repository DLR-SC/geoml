/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CBooleanOperTools.h"
#include "CNamedShape.h"
#include "BRepSewingToBRepBuilderShapeAdapter.h"

#include <cassert>

#include <TopExp.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

// finds out for each face of the splitted result, what the name of the parent face was
void CBooleanOperTools::MapFaceNamesAfterBOP(BRepBuilderAPI_MakeShape& bop, const PNamedShape source, PNamedShape result)
{
    TopTools_IndexedMapOfShape shapeMapSplit;
    TopExp::MapShapes(result->Shape(),   TopAbs_FACE, shapeMapSplit);

    TopTools_IndexedMapOfShape sourceMap;
    TopExp::MapShapes(source->Shape(),   TopAbs_FACE, sourceMap);

    // find newly created from source
    for (int iface = 1; iface <= sourceMap.Extent(); ++iface) {
        const TopoDS_Face& face =  TopoDS::Face(sourceMap(iface));
        const TopTools_ListOfShape& splits = bop.Modified(face);
        TopTools_ListIteratorOfListOfShape it;
        for (it.Initialize(splits); it.More(); it.Next()) {
            unsigned int index = shapeMapSplit.FindIndex(it.Value());

            if (index > 0) {
                result->FaceTraits(index-1).SetDerivedFromShape(source, iface-1);
            }
        }
    }

    // now apply all non-modified face names
    AppendNamesToShape(source, result);
}

/**
 * @brief AppendNamesToShape searches for each target face, if it
 * can be found in the source shape. If so, it applies the name of
 * the source face to the target face
 */
void CBooleanOperTools::AppendNamesToShape(const PNamedShape source, PNamedShape target)
{
    TopTools_IndexedMapOfShape targetMap, sourceMap;
    TopExp::MapShapes(target->Shape(),   TopAbs_FACE, targetMap);
    TopExp::MapShapes(source->Shape(),   TopAbs_FACE, sourceMap);

    for (int iface = 1; iface <= sourceMap.Extent(); ++iface) {
        const TopoDS_Face& face =  TopoDS::Face(sourceMap(iface));
        unsigned int index = targetMap.FindIndex(face);
        if (index > 0) {
            target->FaceTraits(index-1).SetDerivedFromShape(source, iface-1);
        }
    }
}

/**
 * @brief Tries to sew adjacent faces to create a shell (equivalent to create wires)
 */
PNamedShape CBooleanOperTools::Shellify(PNamedShape shape)
{
    BRepBuilderAPI_Sewing shellMaker;
    shellMaker.Add(shape->Shape());
    shellMaker.Perform();
    BRepSewingToBRepBuilderShapeAdapter sewerAdapter(shellMaker);

    // map names to shell
    PNamedShape resultShell(new CNamedShape(*shape));
    resultShell->SetShape(shellMaker.SewedShape());
    PNamedShape tmpshape(new CNamedShape(*shape));
    tmpshape->SetShape(shellMaker.ModifiedSubShape(shape->Shape()));
    CBooleanOperTools::MapFaceNamesAfterBOP(sewerAdapter, tmpshape, resultShell);
    
    return resultShell;
}
