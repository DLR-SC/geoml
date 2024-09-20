/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/


#include "TopoAlgorithms.h"

#include "BSplineAlgorithms.h"
#include "geoml/error.h"
#include "common/CommonFunctions.h"

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shell.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomConvert.hxx>
#include <algorithm>

namespace
{
    void CutSurfaceAtUVParametersImpl(Handle(Geom_Surface) surface, std::vector<double> uparams, std::vector<double> vparams, TopoDS_Shell& shell)
    {
        if (surface.IsNull()) {
            return;
        }

        std::sort(std::begin(uparams), std::end(uparams));
        std::sort(std::begin(vparams), std::end(vparams));

        Standard_Real u1, u2, v1, v2;
        surface->Bounds(u1, u2, v1, v2);

        const double tol=1e-6;

        auto process_params = [tol](std::vector<double>& params, double min, double max) {
            if (params.empty()) {
                params = {min, max};
            }
            else {
                params.erase(std::remove_if(std::begin(params), std::end(params), [min, max](double par) {
                   return par < min || par > max;
                }), params.end());

                if (fabs(min - params.front()) < tol) {
                    params.front() = min;
                }
                else {
                    params.insert(params.begin(), min);
                }
                if (fabs(max - params.back()) < tol) {
                    params.back() = max;
                }
                else {
                    params.push_back(max);
                }
            }
        };

        process_params(uparams, u1, u2);
        process_params(vparams, v1, v2);

        BRep_Builder builder;

        for (size_t vidx = 1; vidx < vparams.size(); ++vidx) {
            for (size_t uidx = 1; uidx < uparams.size(); ++uidx) {
                auto trimmedSurface = geoml::BSplineAlgorithms::trimSurface(surface, uparams[uidx-1], uparams[uidx], vparams[vidx-1], vparams[vidx]);
                BRepBuilderAPI_MakeFace faceMaker(trimmedSurface, 1e-10);
                builder.Add(shell, faceMaker.Face());
            }
        }

    }
}

namespace geoml
{

TopoDS_Shape TopoAlgorithms::CutShellAtUVParameters(TopoDS_Shape const& shape, std::vector<double> uparams, std::vector<double> vparams)
{

    bool cutInUDirection = (uparams.size() > 0);
    bool cutInVDirection = (vparams.size() > 0);

    if ( !cutInUDirection && !cutInVDirection ) {
        //nothing to do
        return shape;
    }

    // sort parameter vectors if they are not sorted
    if (cutInUDirection && !std::is_sorted(uparams.begin(), uparams.end()) ) {
        std::sort(uparams.begin(), uparams.end());
    }
    if (cutInVDirection && !std::is_sorted(vparams.begin(), vparams.end()) ) {
        std::sort(vparams.begin(), vparams.end());
    }

    TopoDS_Shell cutShape;
    BRep_Builder builder;
    builder.MakeShell(cutShape);

    for (TopExp_Explorer faces(shape, TopAbs_FACE); faces.More(); faces.Next()) {
        // trim each face/surface of the compound at the uv parameters in the parameter vectors
        auto surface = BRep_Tool::Surface(TopoDS::Face(faces.Current()));
        CutSurfaceAtUVParametersImpl(surface, uparams, vparams, cutShape);
    }
    return std::move(cutShape);
}

TopoDS_Shape TopoAlgorithms::CutShellAtKinks(TopoDS_Shape const& shape)
{

    TopoDS_Shell cutShape;
    BRep_Builder builder;
    builder.MakeShell(cutShape);

    for (TopExp_Explorer faces(shape, TopAbs_FACE); faces.More(); faces.Next()) {
        // trim each face/surface of the compound at the uv parameters in the parameter vectors
        auto surface = BRep_Tool::Surface(TopoDS::Face(faces.Current()));
        CutSurfaceAtKinks(GeomConvert::SurfaceToBSplineSurface(surface), cutShape);
    }
    return std::move(cutShape);
}

TopoDS_Shell TopoAlgorithms::CutSurfaceAtUVParameters(Handle(Geom_Surface) surface, std::vector<double> uparams, std::vector<double> vparams)
{
    if (!surface) {
        throw Error("Null pointer surface in TopoAlgorithms::CutSurfaceAtUVParameters", geoml::NULL_POINTER);
    }

    BRep_Builder builder;
    TopoDS_Shell shell;
    builder.MakeShell(shell);
    CutSurfaceAtUVParametersImpl(surface, uparams, vparams, shell);
    return shell;
}

void TopoAlgorithms::CutSurfaceAtUVParameters(Handle(Geom_Surface) surface, std::vector<double> uparams, std::vector<double> vparams, TopoDS_Shell& shell)
{
    if (!surface) {
        throw Error("Null pointer surface in TopoAlgorithms::CutSurfaceAtUVParameters", geoml::NULL_POINTER);
    }

    CutSurfaceAtUVParametersImpl(surface, uparams, vparams, shell);
}



void TopoAlgorithms::CutSurfaceAtKinks(Handle(Geom_BSplineSurface) surface, TopoDS_Shell& shell)
{
    if (!surface) {
        throw Error("Null pointer surface in TopoAlgorithms::CutSurfaceAtKinks", geoml::NULL_POINTER);
    }
    
    auto kinks = BSplineAlgorithms::getKinkParameters(surface);
    return CutSurfaceAtUVParametersImpl(surface, kinks.u, kinks.v, shell);
}

TopoDS_Shell TopoAlgorithms::CutSurfaceAtKinks(Handle(Geom_BSplineSurface) surface)
{
    if (!surface) {
        throw Error("Null pointer surface in TopoAlgorithms::CutSurfaceAtKinks", geoml::NULL_POINTER);
    }

    BRep_Builder builder;
    TopoDS_Shell shell;
    builder.MakeShell(shell);
    
    TopoAlgorithms::CutSurfaceAtKinks(surface, shell);
    return shell;
}


bool TopoAlgorithms::IsDegenerated(const TopoDS_Wire& wire)
{
    bool isDegen = Standard_True;

    double length = 0.;
    TopoDS_Iterator iter(wire);
    for (; iter.More(); iter.Next())
    {
        const TopoDS_Edge& anEdge = TopoDS::Edge(iter.Value());
        if (!BRep_Tool::Degenerated(anEdge)) {
            isDegen = Standard_False;
        }
        length += GetLength(anEdge);
    }

    if (length < Precision::Confusion()) {
        isDegen = true;
    }

    if (!isDegen) {
        BRepBuilderAPI_MakeFace facemaker(wire);
        if (!facemaker.IsDone()) {
            isDegen = true;
        }
        else {
            isDegen = false;
        }
    }

    return isDegen;
}

} // namespace geoml
