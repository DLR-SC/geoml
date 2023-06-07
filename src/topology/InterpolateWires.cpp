/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2014-05-26 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "InterpolateWires.h"
#include "geoml_config.h"
#include "common/CommonFunctions.h"
#include "logging/Logging.h"
#include "Debugging.h"

#include "geometry/CurvesToSurface.h"
#include "PatchShellTool.h"
#include "Debugging.h"

#include "contrib/MakePatches.hxx"
#include "TopoAlgorithms.h"
#include "geoml/error.h"


#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepAlgo.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Geom_Plane.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRep_Tool.hxx>

#include <cassert>

namespace {
    TopoDS_Shape ResortFaces(TopoDS_Shape const& shape, int nu, int nv, bool umajor2vmajor = true);
}

namespace geoml {

InterpolateWires::InterpolateWires(double tolerance, double sameKnotTolerance)
{
    _hasPerformed = false;
    _makeSolid = true;
    _result.Nullify();
    _myTolerance = tolerance;
    _mySameKnotTolerance = sameKnotTolerance;
}

InterpolateWires::InterpolateWires(const TopoDS_Shape& profiles, const TopoDS_Shape& guides, double tolerance, double sameKnotTolerance)
{
    _hasPerformed = false;
    _result.Nullify();
    _myTolerance = tolerance;
    _myTolerance = tolerance;
    _mySameKnotTolerance = sameKnotTolerance;
    addWire(profiles);
    addGuides(guides);
}

void InterpolateWires::addWire(const TopoDS_Shape &profiles)
{
    if (profiles.ShapeType() == TopAbs_COMPOUND) {
        for (TopoDS_Iterator anIter(profiles); anIter.More(); anIter.Next()) {
            TopoDS_Wire aSh = TopoDS::Wire(anIter.Value());
            this->wires.push_back(aSh);
        }
    }
    else {
        this->wires.push_back(TopoDS::Wire(profiles));
    }
}

void InterpolateWires::addGuides(const TopoDS_Shape &guides)
{
    if (guides.ShapeType() == TopAbs_COMPOUND) {
        for (TopoDS_Iterator anIter(guides); anIter.More(); anIter.Next()) {
            TopoDS_Wire aSh = TopoDS::Wire(anIter.Value());
            this->guides.push_back(aSh);
        }
    }
    else {
        this->guides.push_back(TopoDS::Wire(guides));
    }
}

TopoDS_Shape &InterpolateWires::Shape()
{
    Perform();
    
    return _result;
}

InterpolateWires::operator TopoDS_Shape &()
{
    return Shape();
}

void InterpolateWires::Perform()
{
    if (_hasPerformed) {
        return;
    }
    
    DEBUG_SCOPE(debug);
    for (size_t i = 0; i < wires.size(); ++i) {
        debug.addShape(wires[i], "profile_" + std::to_string(i));
    }

    if (guides.size() > 0) {
        for (size_t i = 0; i < guides.size(); ++i) {
            debug.addShape(guides[i], "guide_" + std::to_string(i));
        }

        // to the loft with guides
        makeLoftWithGuides();
    }
    else {
        makeLoftWithoutGuides();
    }
    _hasPerformed = true;
}

void InterpolateWires::setMakeSolid(bool enabled)
{
    _makeSolid = enabled;
}

void InterpolateWires::setMakeSmooth(bool enabled)
{
    _makeSmooth = enabled;
}

/**
 * @brief Builds the loft using profiles and guide curves
 */
void InterpolateWires::makeLoftWithGuides()
{
    BRep_Builder b;
    TopoDS_Compound cprof, cguid;
    b.MakeCompound(cprof);
    b.MakeCompound(cguid);
    
    for (unsigned int i = 0; i < wires.size(); ++i) {
        TopoDS_Wire& profile =  wires[i];
        
        // remove leading edge split in profile
        if (GetNumberOfEdges(profile) > 1) {
            profile =  BRepAlgo::ConcatenateWire(profile,GeomAbs_C1);
        }
        b.Add(cprof, profile);
    }
    for (unsigned int i = 0; i < guides.size(); ++i) {
        TopoDS_Wire& guide =  guides[i];
        b.Add(cguid, guide);
    }
    
#ifdef DEBUG
    static int iLoft = 0;
    geoml::dumpShape(cprof, "debugShapes", "profiles", iLoft);
    geoml::dumpShape(cguid, "debugShapes", "guides", iLoft);
    iLoft++;
#endif
    
    MakePatches SurfMaker(cguid, cprof);
    // Don't sew yet. We do it later in solid creation
#ifdef HAVE_OCE_COONS_PATCHED
    GeomFill_FillingStyle style = GeomFill_CoonsC2Style;

    char* c_cont = getenv("GEOML_COONS_CONTINUITY");
    if (c_cont) {
        if (strcmp(c_cont, "0") == 0) {
            style = GeomFill_StretchStyle;
        }
        else if (strcmp(c_cont, "1") == 0) {
            style = GeomFill_CoonsStyle;
        }
        else if (strcmp(c_cont, "2") == 0) {
            style = GeomFill_CoonsC2Style;
        }

        static bool has_informed = false;
        if (!has_informed) {
            std::string s_style;
            switch(style) {
            case GeomFill_StretchStyle:
                s_style = "Stretch style (C0)";
                break;
            case GeomFill_CoonsStyle:
                s_style = "Coons style (C1)";
                break;
            case GeomFill_CoonsC2Style:
            default:
                s_style = "Coons C2 style (C2)";
            }
            LOG(WARNING) << "Using user-defined surface modelling: " << s_style;
            has_informed = true;
        }

    }
#else
    GeomFill_FillingStyle style = GeomFill_CoonsStyle;
#endif
    SurfMaker.Perform(_myTolerance, _mySameKnotTolerance, style, Standard_True);
    _result = SurfMaker.Patches();
    if (SurfMaker.GetStatus() > 0) {
        LOG(ERROR) << "Could not create loft with guide curves. " << "Error code = " << SurfMaker.GetStatus();
        return;
    }
    
#ifdef DEBUG
    // store intermediate result
    std::stringstream spatches;
    spatches << "patches" << iLoft << ".brep";
    BRepTools::Write(_result, spatches.str().c_str());
#endif
    CloseShape();
}

void InterpolateWires::makeLoftWithoutGuides()
{
    // check number of edges are all same
    std::vector<unsigned int> edgeCountPerProfile;
    for (const auto& profile : wires) {
        edgeCountPerProfile.push_back(GetNumberOfEdges(profile));
    }
    if (!AllSame(edgeCountPerProfile.begin(), edgeCountPerProfile.end())) {
        throw geoml::Error("Number of edges not equal in InterpolateWires");
    }

    TopoDS_Shell faces;
    BRep_Builder builder;
    builder.MakeShell(faces);

    // get number of edges per profile wire
    // --> should be the same for all profiles
    TopTools_IndexedMapOfShape firstProfileMap;
    TopExp::MapShapes(wires[0], TopAbs_EDGE, firstProfileMap);
    int const nEdgesPerProfile = firstProfileMap.Extent();

    // skin the surface edge by adge
    // CAUTION: Here it is assumed that the edges are ordered
    // in the same way along each profile (e.g. lower edge,
    // upper edge, trailing edge for a wing)
    for ( int iE = 1; iE <= nEdgesPerProfile; ++iE ) {

        // get the curves
        std::vector<Handle(Geom_Curve)> profileCurves;
        profileCurves.reserve(wires.size());
        for (unsigned iP=0; iP<wires.size(); ++iP ) {

            TopTools_IndexedMapOfShape profileMap;
            TopExp::MapShapes(wires[iP], TopAbs_EDGE, profileMap);
            assert( profileMap.Extent() >= iE );

            TopoDS_Edge edge = TopoDS::Edge(profileMap(iE));
            profileCurves.push_back(GetBSplineCurve(edge));
        }

        // skin the curves
        geoml::CurvesToSurface surfaceSkinner(profileCurves, vparams);
        if (!_makeSmooth) {
            surfaceSkinner.SetMaxDegree(1);
        }
        Handle(Geom_BSplineSurface) surface = surfaceSkinner.Surface();

        // remember the profile parameters used for the skinning
        if (vparams.size()==0) {
            vparams = surfaceSkinner.GetParameters();
        }

        builder.Add(faces, BRepBuilderAPI_MakeFace(surface, 1e-6).Face());
    }
    _result = geoml::TopoAlgorithms::CutShellAtUVParameters(faces, {}, vparams);

    // make sure the order is the same as for the COONS Patch algorithm
    _result = ResortFaces(_result, nEdgesPerProfile, static_cast<int>(vparams.size()-1));
    _result = geoml::TopoAlgorithms::CutShellAtKinks(_result);
    CloseShape();
}

void InterpolateWires::CloseShape()
{
    geoml::PatchShellTool patcher(_result, _myTolerance);
    Standard_Boolean vClosed = (wires[0].IsSame(wires.back()));
    if ( !vClosed && _makeSolid ) {
        patcher.AddSideCap(TopoDS::Wire(wires[0]));
        patcher.AddSideCap(TopoDS::Wire(wires.back()));
    }
    patcher.SetMakeSolid(_makeSolid);
    _result = patcher.PatchedShape();
}

} // namespace geoml

namespace
{

    TopoDS_Shape ResortFaces(TopoDS_Shape const& shape, int nu, int nv, bool umajor2vmajor)
    {
        nu = (nu < 1)? 1 : nu;
        nv = (nv < 1)? 1 : nv;

        // map of in faces
        TopTools_IndexedMapOfShape map;
        TopExp::MapShapes(shape, TopAbs_FACE, map);
        int n = map.Extent();

        assert(nu*nv == n);

        TopoDS_Shell sorted;
        BRep_Builder B;
        B.MakeShell(sorted);
        if ( umajor2vmajor ) {
            for(int v = 1; v <= nv; v++) {
                for (int u = 1; u <= nu; u++) {
                    B.Add(sorted,TopoDS::Face(map((u-1)*nv + v)));
                }
            }
        }
        else
        {
            for(int u = 1; u <= nu; u++) {
                for (int v = 1; v <= nv; v++) {
                    B.Add(sorted,TopoDS::Face(map((v-1)*nu + u)));
                }
            }
        }

        return sorted;
    }
} // namespace
