/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2018-07-18 Jan Kleinert <Jan.Kleinert@dlr.de>
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

#include <cassert>

#include "CurvesToSurface.h"
#include "geoml/error.h"
#include "BSplineAlgorithms.h"
#include "logging/Logging.h"

#include <GeomAPI_Interpolate.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Precision.hxx>

#include "geometry/PointsToBSplineInterpolation.h"

//TODO anonymous helper functions duplicated from BSplineAlgorithms.cpp
namespace {

void clampBSpline(Handle(Geom_BSplineCurve)& curve)
{
    if (!curve->IsPeriodic()) {
        return;
    }
    curve->SetNotPeriodic();

    Handle(Geom_Curve) c = new Geom_TrimmedCurve(curve, curve->FirstParameter(), curve->LastParameter());
    curve = GeomConvert::CurveToBSplineCurve(c);
}

}

namespace geoml {

GEOML_EXPORT CurvesToSurface::CurvesToSurface(std::vector<Handle(Geom_Curve) > const& splines_vector,
                                                       bool continuousIfClosed)
    : _continuousIfClosed(continuousIfClosed)
{
    // convert all curves to bspline curves
    for (std::vector<Handle(Geom_Curve) >::const_iterator curve_iter = splines_vector.begin(); curve_iter != splines_vector.end(); ++curve_iter) {
        _inputCurves.push_back(GeomConvert::CurveToBSplineCurve(*curve_iter));
    }
    
    BSplineAlgorithms::matchDegree(_inputCurves);
    CalculateParameters(_inputCurves); //TODO this is only really needed if maxDegree > 1
}

GEOML_EXPORT CurvesToSurface::CurvesToSurface(std::vector<Handle(Geom_Curve) > const& splines_vector,
                                                       std::vector<double> const& parameters,
                                                       bool continuousIfClosed)
    : _parameters(parameters)
    , _continuousIfClosed(continuousIfClosed)
{
    // convert all curves to bspline curves
    for (std::vector<Handle(Geom_Curve) >::const_iterator curve_iter = splines_vector.begin(); curve_iter != splines_vector.end(); ++curve_iter) {
        _inputCurves.push_back(GeomConvert::CurveToBSplineCurve(*curve_iter));
    }
    
    BSplineAlgorithms::matchDegree(_inputCurves);
    if( parameters.size() == 0) {
        CalculateParameters(_inputCurves);
    }
}

GEOML_EXPORT void CurvesToSurface::SetMaxDegree(int degree)
{
    assert( degree > 0 );
    _maxDegree = degree;
    Invalidate();
}

GEOML_EXPORT void CurvesToSurface::CalculateParameters(std::vector<Handle(Geom_BSplineCurve)> const& splines_vector)
{


    // create a common knot vector for all splines
    if ( _compatibleSplines.size() == 0 ) {
        _compatibleSplines = BSplineAlgorithms::createCommonKnotsVectorCurve(splines_vector, 1e-14);
    }

    // create a matrix of control points of all B-splines (splines do have the same amount of control points now)
    TColgp_Array2OfPnt controlPoints(1, _compatibleSplines[0]->NbPoles(),
                                     1, static_cast<Standard_Integer>(_compatibleSplines.size()));

    for (unsigned int spline_idx = 1; spline_idx <= _compatibleSplines.size(); ++spline_idx) {
        for (int point_idx = 1; point_idx <= _compatibleSplines[0]->NbPoles(); ++point_idx) {
            controlPoints(point_idx, spline_idx) = _compatibleSplines[spline_idx - 1]->Pole(point_idx);
        }
    }

    std::pair<std::vector<double>, std::vector<double> > parameters
        = BSplineAlgorithms::computeParamsBSplineSurf(controlPoints);
    _parameters = parameters.second;
}

GEOML_EXPORT void CurvesToSurface::Perform()
{
    // check amount of given parameters
    if (_parameters.size() != _inputCurves.size()) {
        throw Error("The amount of given parameters has to be equal to the amount of given B-splines!", geoml::MATH_ERROR);
    }

    // check if all curves are closed
    double tolerance = BSplineAlgorithms::scale(_inputCurves) * BSplineAlgorithms::REL_TOL_CLOSED;
    bool makeClosed = _continuousIfClosed & _inputCurves.front()->IsEqual(_inputCurves.back(), tolerance);

    size_t nCurves = _inputCurves.size();

    // create a common knot vector for all splines
    if ( _compatibleSplines.size() == 0 ) {
        _compatibleSplines = BSplineAlgorithms::createCommonKnotsVectorCurve(_inputCurves, 1e-14);
    }

    const Handle(Geom_BSplineCurve)& firstCurve = _compatibleSplines[0];
    size_t numControlPointsU = firstCurve->NbPoles();

    int degreeV = 0;
    int degreeU = firstCurve->Degree();
    Handle(TColStd_HArray1OfReal) knotsV;
    Handle(TColStd_HArray1OfInteger) multsV;

    // create matrix of new control points with size which is possibly DIFFERENT from the size of controlPoints
    Handle(TColgp_HArray2OfPnt) cpSurf;
    Handle(TColgp_HArray1OfPnt) interpPointsVDir = new TColgp_HArray1OfPnt(1, static_cast<Standard_Integer>(nCurves));

    // now continue to create new control points by interpolating the remaining columns of controlPoints in Skinning direction (here v-direction) by B-splines
    for (int cpUIdx = 1; cpUIdx <= numControlPointsU; ++cpUIdx) {
        for (int cpVIdx = 1; cpVIdx <= nCurves; ++cpVIdx) {
            interpPointsVDir->SetValue(cpVIdx, _compatibleSplines[cpVIdx - 1]->Pole(cpUIdx));
        }

        PointsToBSplineInterpolation interpol(interpPointsVDir, _parameters, _maxDegree, makeClosed);
        Handle(Geom_BSplineCurve) interpSpline = interpol.Curve();

        if (makeClosed) {
            clampBSpline(interpSpline);
        }

        if (cpUIdx == 1) {
            degreeV = interpSpline->Degree();
            knotsV = new TColStd_HArray1OfReal(1, interpSpline->NbKnots());
            interpSpline->Knots(knotsV->ChangeArray1());
            multsV = new TColStd_HArray1OfInteger(1, interpSpline->NbKnots());
            interpSpline->Multiplicities(multsV->ChangeArray1());
            cpSurf = new TColgp_HArray2OfPnt(1, static_cast<Standard_Integer>(numControlPointsU), 1, interpSpline->NbPoles());
        }

        // the final surface control points are the control points resulting from
        // the interpolation
        for (int i = cpSurf->LowerCol(); i <= cpSurf->UpperCol(); ++i) {
            cpSurf->SetValue(cpUIdx, i, interpSpline->Pole(i));
        }

        // check degree always the same
        assert(degreeV == interpSpline->Degree());
    }

    TColStd_Array1OfReal knotsU(1, firstCurve->NbKnots());
    firstCurve->Knots(knotsU);
    TColStd_Array1OfInteger multsU(1, firstCurve->NbKnots());
    firstCurve->Multiplicities(multsU);

    _skinnedSurface = new Geom_BSplineSurface(cpSurf->Array2(),
                                              knotsU, knotsV->Array1(),
                                              multsU, multsV->Array1(),
                                              degreeU, degreeV);

}

}
