/*
* Copyright (C) 2017 German Aerospace Center (DLR/SC)
*
* Created: 2017-05-24 Merlin Pelz <Merlin.Pelz@dlr.de>
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

#include "IntersectBSplines.h"
#include "BSplineAlgorithms.h"
#include "geometry/CurvesToSurface.h"
#include "geoml/error.h"
#include "BSplineApproxInterp.h"
#include "PointsToBSplineInterpolation.h"
#include "common/CommonFunctions.h"
#include "Debugging.h"
#include "logging/Logging.h"

#include <Standard_Version.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <BSplCLib.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <cassert>

namespace
{

    class helper_function_unique
    {
    public:
        helper_function_unique(double tolerance = 1e-15)
            : _tol(tolerance)
        {}

        // helper function for std::unique
        bool operator()(double a, double b)
        {
            return (fabs(a - b) < _tol);
        }
    private:
        double _tol;
    };
    
    enum SurfAdapterDir
    {
        udir = 0,
        vdir = 1
    };
    
    class SurfAdapterView
    {
    public:
        SurfAdapterView(Handle(Geom_BSplineSurface) surf, SurfAdapterDir dir)
            : _surf(surf), _dir(dir)
        {
        }

        void insertKnot(double knot, int mult, double tolerance=1e-15)
        {
            if (_dir == udir) {
                _surf->InsertUKnot(knot, mult, tolerance, false);
            }
            else {
                _surf->InsertVKnot(knot, mult, tolerance, false);
            }
        }

        double getKnot(int idx) const
        {
            if (_dir == udir) {
                return _surf->UKnot(idx);
            }
            else {
                return _surf->VKnot(idx);
            }
        }

        int getMult(int idx) const
        {
            if (_dir == udir) {
                return _surf->UMultiplicity(idx);
            }
            else {
                return _surf->VMultiplicity(idx);
            }
        }

        int getNKnots() const
        {
            if (_dir == udir) {
                return _surf->NbUKnots();
            }
            else {
                return _surf->NbVKnots();
            }
        }

        int getDegree() const
        {
            if (_dir == udir) {
                return _surf->UDegree();
            }
            else {
                return _surf->VDegree();
            }
        }
        
        void setDir(SurfAdapterDir dir)
        {
            _dir = dir;
        }

        operator const Handle(Geom_BSplineSurface)&() const
        {
            return _surf;
        }
    private:
        Handle(Geom_BSplineSurface) _surf;
        SurfAdapterDir _dir;
    };
    
    class CurveAdapterView
    {
    public:
        CurveAdapterView(Handle(Geom_BSplineCurve) curve)
            : _curve(curve)
        {
        }

        void insertKnot(double knot, int mult, double tolerance=1e-15)
        {
            _curve->InsertKnot(knot, mult, tolerance, false);
        }

        double getKnot(int idx) const
        {
            return _curve->Knot(idx);
        }

        int getMult(int idx) const
        {
            return _curve->Multiplicity(idx);
        }

        int getNKnots() const
        {
            return _curve->NbKnots();
        }

        int getDegree() const
        {
            return _curve->Degree();
        }

        operator const Handle(Geom_BSplineCurve)&() const
        {
            return _curve;
        }
 
    private:
        Handle(Geom_BSplineCurve) _curve;
    };

    template <class SplineAdapter>
    bool haveSameRange(const std::vector<SplineAdapter>& splines_vector, double par_tolerance)
    {
        double begin_param_dir = splines_vector[0].getKnot(1);
        double end_param_dir = splines_vector[0].getKnot(splines_vector[0].getNKnots());
        for (unsigned int spline_idx = 1; spline_idx < splines_vector.size(); ++spline_idx) {
            const SplineAdapter& curSpline = splines_vector[spline_idx];
            double begin_param_dir_surface = curSpline.getKnot(1);
            double end_param_dir_surface = curSpline.getKnot(curSpline.getNKnots());
            if (std::abs(begin_param_dir_surface - begin_param_dir) > par_tolerance || std::abs(end_param_dir_surface - end_param_dir) > par_tolerance) {
                return false;
            }
        }
        return true;
    }

    template <class SplineAdapter>
    bool haveSameDegree(const std::vector<SplineAdapter>& splines)
    {
        int degree = splines[0].getDegree();
        for (unsigned int splineIdx = 1; splineIdx < splines.size(); ++splineIdx) {
            if (splines[splineIdx].getDegree() != degree) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief createCommonKnotsVectorImpl:
     *          Creates a common knot vector in u- or v-direction of the given vector of B-splines
     *          The common knot vector contains all knots in u- or v-direction of all splines with the highest multiplicity of all splines.
     * @param old_splines_vector:
     *          the given vector of B-spline splines that could have a different knot vector in u- or v-direction
     */
    template <class SplineAdapter>
    void makeGeometryCompatibleImpl(std::vector<SplineAdapter>& splines_vector, double par_tolerance)
    {
        // all B-spline splines must have the same parameter range in the chosen direction
        if (!haveSameRange(splines_vector, par_tolerance)) {
            throw geoml::Error("B-splines don't have the same parameter range at least in one direction (u / v) in method createCommonKnotsVectorImpl!", geoml::MATH_ERROR);
        }

        // all B-spline splines must have the same degree in the chosen direction
        if (!haveSameDegree(splines_vector)) {
            throw geoml::Error("B-splines don't have the same degree at least in one direction (u / v) in method createCommonKnotsVectorImpl!", geoml::MATH_ERROR);
        }

        // The parametric tolerance must be smaller than half of the minimum knot distance
        for (typename std::vector<SplineAdapter>::const_iterator splineIt = splines_vector.begin(); splineIt != splines_vector.end(); ++splineIt) {
            const SplineAdapter& spline = *splineIt;
            for (int iKnot = 1; iKnot < spline.getNKnots(); ++iKnot) {
                double knotDist = spline.getKnot(iKnot+1) - spline.getKnot(iKnot);
                par_tolerance = std::min(par_tolerance, knotDist / 2.);
            }
        }

        // insert all knots in first spline
        SplineAdapter& firstSpline = splines_vector[0];
        for (typename std::vector<SplineAdapter>::const_iterator splineIt = splines_vector.begin()+1; splineIt != splines_vector.end(); ++splineIt) {
            const SplineAdapter& spline = *splineIt;
            for (int knot_idx = 2; knot_idx < spline.getNKnots(); ++knot_idx) {
                double knot = spline.getKnot(knot_idx);
                int mult = spline.getMult(knot_idx);
                firstSpline.insertKnot(knot, mult, par_tolerance);
            }
        }


        // now insert knots from first into all others
        for (typename std::vector<SplineAdapter>::iterator splineIt = splines_vector.begin()+1; splineIt != splines_vector.end(); ++splineIt) {
            SplineAdapter& spline = *splineIt;
            for (int knot_idx = 2; knot_idx < firstSpline.getNKnots(); ++knot_idx) {
                double knot = firstSpline.getKnot(knot_idx);
                int mult = firstSpline.getMult(knot_idx);
                spline.insertKnot(knot, mult, par_tolerance);
            }
            if (spline.getNKnots() != firstSpline.getNKnots()) {
                throw geoml::Error("Unexpected error in Algorithm makeGeometryCompatibleImpl.\nPlease contact the developers.");
            }
        }


    } // makeGeometryCompatibleImpl
    
    template <class OccMatrix, class OccVector, class OccHandleVector>
    OccHandleVector array2GetColumn(const OccMatrix& matrix, int colIndex)
    {
        OccHandleVector colVector =  new OccVector(matrix.LowerRow(), matrix.UpperRow());

        for (int rowIdx = matrix.LowerRow(); rowIdx <= matrix.UpperRow(); ++rowIdx) {
            colVector->SetValue(rowIdx, matrix(rowIdx, colIndex));
        }

        return colVector;
    }
    
    template <class OccMatrix, class OccVector, class OccHandleVector>
    OccHandleVector array2GetRow(const OccMatrix& matrix, int rowIndex)
    {
        OccHandleVector rowVector = new OccVector(matrix.LowerCol(), matrix.UpperCol());
        
        for (int colIdx = matrix.LowerCol(); colIdx <= matrix.UpperCol(); ++colIdx) {
            rowVector->SetValue(colIdx, matrix(rowIndex, colIdx));
        }
        
        return rowVector;
    }

    Handle_TColgp_HArray1OfPnt pntArray2GetColumn(const TColgp_Array2OfPnt& matrix, int colIndex)
    {
        return array2GetColumn<TColgp_Array2OfPnt, TColgp_HArray1OfPnt, Handle_TColgp_HArray1OfPnt>(matrix, colIndex);
    }

    Handle_TColgp_HArray1OfPnt pntArray2GetRow(const TColgp_Array2OfPnt& matrix, int rowIndex)
    {
        return array2GetRow<TColgp_Array2OfPnt, TColgp_HArray1OfPnt, Handle_TColgp_HArray1OfPnt>(matrix, rowIndex);
    }
    		
} // namespace


namespace geoml
{

const double BSplineAlgorithms::REL_TOL_CLOSED = 1e-8;

bool BSplineAlgorithms::isUDirClosed(const TColgp_Array2OfPnt& points, double tolerance)
{
    bool uDirClosed = true;
    int ulo = points.LowerRow();
    int uhi = points.UpperRow();
    // check that first row and last row are the same
    for (int v_idx = points.LowerCol(); v_idx <= points.UpperCol(); ++v_idx) {
        gp_Pnt pfirst = points.Value(ulo, v_idx);
        gp_Pnt pLast = points.Value(uhi, v_idx);
        uDirClosed = uDirClosed & pfirst.IsEqual(pLast, tolerance);
    }
    return uDirClosed;
}

bool BSplineAlgorithms::isVDirClosed(const TColgp_Array2OfPnt& points, double tolerance)
{
    bool vDirClosed = true;
    int vlo = points.LowerCol();
    int vhi = points.UpperCol();
    for (int u_idx = points.LowerRow(); u_idx <= points.UpperRow(); ++u_idx) {
        vDirClosed = vDirClosed & points.Value(u_idx, vlo).IsEqual(points.Value(u_idx, vhi), tolerance);
    }
    return vDirClosed;
}

std::vector<double> BSplineAlgorithms::knotsFromCurveParameters(std::vector<double> &params, unsigned int degree, bool closedCurve)
{
    if (params.size() < 2) {
        throw Error("Parameters must contain two or more elements.");
    }

    size_t nCP = params.size();
    if (closedCurve) {
        // For each continuity condition, we have to add one control point
        nCP += degree - 1;
    }
    size_t nInnerKnots = nCP - degree + 1;

    std::vector<double> innerKnots(nInnerKnots);
    innerKnots.front() = params.front();
    innerKnots.back() = params.back();

    std::vector<double> knots;

    if (closedCurve && degree % 2 == 0) {
        size_t m = params.size() - 2;

        // build difference vector
        std::vector<double> dparm(m + 1, 0.);
        for (size_t iparm = 0; iparm <= m; ++iparm) {
            dparm[iparm] = (params[iparm+1] - params[iparm]);
        }

        innerKnots[1] = innerKnots[0] + 0.5 * (dparm[0] + dparm[m]);
        for (size_t iparm = 1; iparm < m; ++iparm) {
            innerKnots[iparm + 1] = innerKnots[iparm] + 0.5 * (dparm[iparm-1] + dparm[iparm]);
        }

        // shift parameters
        for (size_t iparm = 0; iparm < params.size(); ++iparm) {
            params[iparm] += dparm[m] / 2.;
        }

    }
    else if (closedCurve) {
        assert(innerKnots.size() == params.size());
        innerKnots = params;
    }
    else {
        // averaging
        for (size_t j = 1; j < params.size() - degree; ++j) {
            double sum = 0.;
            // average
            for (size_t i = j; i <= j + degree - 1; ++i) {
                sum += params[i];
            }

            innerKnots[j] = sum / static_cast<double>(degree);
        }
    }

    if (closedCurve) {
        double offset = innerKnots[0] - innerKnots[nInnerKnots -1];
        for (size_t iknot = 0; iknot < degree; ++iknot) {
            knots.push_back(offset + innerKnots[nInnerKnots - degree -1 + iknot]);
        }
        for (size_t iknot = 0; iknot < nInnerKnots; ++iknot) {
            knots.push_back(innerKnots[iknot]);
        }
        for (size_t iknot = 0; iknot < degree; ++iknot) {
            knots.push_back(-offset + innerKnots[iknot + 1]);
        }
    }
    else {
        for (size_t iknot = 0; iknot < degree; ++iknot) {
            knots.push_back(innerKnots[0]);
        }
        for (size_t iknot = 0; iknot < nInnerKnots; ++iknot) {
            knots.push_back(innerKnots[iknot]);
        }
        for (size_t iknot = 0; iknot < degree; ++iknot) {
            knots.push_back(innerKnots[nInnerKnots - 1]);
        }
    }

    if (closedCurve && degree <= 1) {
        size_t nKnots = knots.size();
        knots[0] = knots[1];
        knots[nKnots - 1] = knots[nKnots - 2];
    }

    return knots;
}

double BSplineAlgorithms::scale(const TColgp_Array2OfPnt& points)
{
    double theScale = 0.;
    for (int uidx = points.LowerRow(); uidx <= points.UpperRow(); ++uidx) {
        gp_Pnt pFirst = points.Value(uidx, points.LowerCol());
        for (int vidx = points.LowerCol() + 1; vidx <= points.UpperCol(); ++vidx) {
            double dist = pFirst.Distance(points.Value(uidx, vidx));
            theScale = std::max(theScale, dist);
        }
    }
    return theScale;
}

double BSplineAlgorithms::scale(const TColgp_Array1OfPnt& points)
{
    double theScale = 0.;

    for (int i = points.Lower(); i <= points.Upper(); ++i) {
        for (int j = i + 1; j < points.Upper(); ++j) {
            double dist = points.Value(i).Distance(points.Value(j));
            theScale = std::max(theScale, dist);
        }
    }
    return theScale;
}

std::vector<double> BSplineAlgorithms::computeParamsBSplineCurve(const Handle(TColgp_HArray1OfPnt)& points, const double alpha)
{
    return computeParamsBSplineCurve(points, 0., 1., alpha);
}

std::vector<double> BSplineAlgorithms::computeParamsBSplineCurve(const Handle(TColgp_HArray1OfPnt)& points, double umin, double umax, const double alpha)
{
    if ( umax <= umin ) {
        throw geoml::Error("The specified start parameter is larger than the specified end parameter");
    }
    
    std::vector<double> parameters(static_cast<size_t>(points->Length()));

    parameters[0] = 0.;

    for (size_t i = 1; i < parameters.size(); ++i) {
        int iArray = static_cast<int>(i) + points->Lower();
        double length = pow(points->Value(iArray).SquareDistance(points->Value(iArray - 1)), alpha / 2.);
        parameters[i] = parameters[i - 1] + length;
    }

    double totalLength = parameters.back();


    for (size_t i = 0; i < parameters.size(); ++i) {
        double ratio = 0.;
        if (totalLength < 1e-10) {
            ratio = static_cast<double>(i) / static_cast<double>(parameters.size()-1);
        }
        else {
            ratio = parameters[i] / totalLength;
        }
        parameters[i] = (umax - umin) * ratio + umin;
    }

    return parameters;
}

std::pair<std::vector<double>, std::vector<double> >
BSplineAlgorithms::computeParamsBSplineSurf(const TColgp_Array2OfPnt& points, double alpha)
{
    // first for parameters in u-direction:
    std::vector<double> paramsU(static_cast<size_t>(points.ColLength()), 0.);
    for (int vIdx = points.LowerCol(); vIdx <= points.UpperCol(); ++vIdx) {
        std::vector<double> parameters_u_line = computeParamsBSplineCurve(pntArray2GetColumn(points, vIdx), alpha);

        // average over columns
        for (size_t uIdx = 0; uIdx < parameters_u_line.size(); ++uIdx) {
            paramsU[uIdx] += parameters_u_line[uIdx]/(double)points.RowLength();
        }
    }


    // now for parameters in v-direction:
    std::vector<double> paramsV(static_cast<size_t>(points.RowLength()), 0.);
    for (int uIdx = points.LowerRow(); uIdx <= points.UpperRow(); ++uIdx) {
        std::vector<double> parameters_v_line = computeParamsBSplineCurve(pntArray2GetRow(points, uIdx), alpha);

        // average over rows
        for (size_t vIdx = 0; vIdx < parameters_v_line.size(); ++vIdx) {
            paramsV[vIdx] += parameters_v_line[vIdx]/(double)points.ColLength();
        }
    }

    // put computed parameters for both u- and v-direction in output tuple
    return std::make_pair(paramsU, paramsV);

}


std::vector<Handle(Geom_BSplineCurve)> BSplineAlgorithms::createCommonKnotsVectorCurve(const std::vector<Handle(Geom_BSplineCurve)>& splines_vector, double tol)
{
    // Match parameter range
    matchParameterRange(splines_vector, tol);

    // Create a copy that we can modify
    std::vector<CurveAdapterView> splines_adapter;
    for (size_t i = 0; i < splines_vector.size(); ++i) {
        splines_adapter.push_back(Handle(Geom_BSplineCurve)::DownCast(splines_vector[i]->Copy()));
    }

    makeGeometryCompatibleImpl(splines_adapter, tol);

    return std::vector<Handle(Geom_BSplineCurve)>(splines_adapter.begin(), splines_adapter.end());
}

std::vector<Handle(Geom_BSplineSurface) > BSplineAlgorithms::createCommonKnotsVectorSurface(const std::vector<Handle(Geom_BSplineSurface) >& old_surfaces_vector, SurfaceDirection dir)
{
    // Create a copy that we can modify
    std::vector<SurfAdapterView> adapterSplines;
    for (size_t i = 0; i < old_surfaces_vector.size(); ++i) {
        adapterSplines.push_back(SurfAdapterView(Handle(Geom_BSplineSurface)::DownCast(old_surfaces_vector[i]->Copy()), udir));
    }

    if (dir == SurfaceDirection::u || dir == SurfaceDirection::both) {
        // first in u direction
        makeGeometryCompatibleImpl(adapterSplines, 1e-14);
    }

    if (dir == SurfaceDirection::v || dir == SurfaceDirection::both) {
         // now in v direction
        for (size_t i = 0; i < old_surfaces_vector.size(); ++i) adapterSplines[i].setDir(vdir);
        makeGeometryCompatibleImpl(adapterSplines, 1e-14);
    }

    return std::vector<Handle(Geom_BSplineSurface)>(adapterSplines.begin(), adapterSplines.end());
}

void BSplineAlgorithms::matchParameterRange(std::vector<Handle(Geom_BSplineCurve)> const& bsplines, double tolerance)
{
    Standard_Real umin = bsplines[0]->FirstParameter();
    Standard_Real umax = bsplines[0]->LastParameter();
    for (unsigned iP=1; iP<bsplines.size(); ++iP) {
        Handle(Geom_BSplineCurve) bspl = bsplines[iP];
        if (fabs(bspl->FirstParameter() - umin) > tolerance ||
            fabs(bspl->LastParameter() - umax) > tolerance ) {
            geoml::BSplineAlgorithms::reparametrizeBSpline(*bspl, umin, umax, tolerance);
        }
    }
}

void BSplineAlgorithms::matchDegree(const std::vector<Handle(Geom_BSplineCurve) >& bsplines)
{
    int maxDegree = 0;
    for (std::vector<Handle(Geom_BSplineCurve) >::const_iterator it = bsplines.begin(); it != bsplines.end(); ++it) {
        int curDegree = (*it)->Degree();
        if (curDegree > maxDegree) {
            maxDegree = curDegree;
        }
    }

    for (std::vector<Handle(Geom_BSplineCurve) >::const_iterator it = bsplines.begin(); it != bsplines.end(); ++it) {
        int curDegree = (*it)->Degree();
        if (curDegree < maxDegree) {
            (*it)->IncreaseDegree(maxDegree);
        }
    }
}

ApproxResult BSplineAlgorithms::reparametrizeBSplineContinuouslyApprox(const Handle(Geom_BSplineCurve) spline,
                                                                                 const std::vector<double>& old_parameters,
                                                                                 const std::vector<double>& new_parameters,
                                                                                 size_t n_control_pnts)
{
    if (old_parameters.size() != new_parameters.size()) {
        throw Error("parameter sizes dont match");
    }

    // create a B-spline as a function for reparametrization
    Handle(TColgp_HArray1OfPnt2d) old_parameters_pnts = new TColgp_HArray1OfPnt2d(1, static_cast<Standard_Integer>(old_parameters.size()));
    for (size_t parameter_idx = 0; parameter_idx < old_parameters.size(); ++parameter_idx) {
        int occIdx = static_cast<int>(parameter_idx + 1);
        old_parameters_pnts->SetValue(occIdx, gp_Pnt2d(old_parameters[parameter_idx], 0));
    }

    Geom2dAPI_Interpolate interpolationObject(old_parameters_pnts, OccFArray(new_parameters), false, 1e-15);
    interpolationObject.Perform();

    // check that interpolation was successful
    if (!interpolationObject.IsDone()) {
        throw Error("Cannot reparametrize", geoml::MATH_ERROR);
    }

    Handle(Geom2d_BSplineCurve) reparametrizing_spline = interpolationObject.Curve();

    // Create a vector of parameters including the intersection parameters
    std::vector<double> breaks;
    for (size_t ipar = 1; ipar < new_parameters.size() - 1; ++ipar) {
        breaks.push_back(new_parameters[ipar]);
    }

    double par_tol = 1e-10;

#define MODEL_KINKS
#ifdef MODEL_KINKS
    // remove kinks from breaks
    std::vector<double> kinks = BSplineAlgorithms::getKinkParameters(spline);
    // convert kink parameters into reparametrized parameter using the
    // inverse reparametrization function
    for (size_t ikink = 0; ikink < kinks.size(); ++ikink) {
        kinks[ikink] = Geom2dAPI_ProjectPointOnCurve(gp_Pnt2d(kinks[ikink], 0.), reparametrizing_spline)
                           .LowerDistanceParameter();
    }

    for (size_t ikink = 0; ikink < kinks.size(); ++ikink) {
        double kink = kinks[ikink];
        std::vector<double>::iterator it = std::find_if(breaks.begin(), breaks.end(), IsInsideTolerance(kink, par_tol));
        if (it != breaks.end()) {
            breaks.erase(it);
        }
    }
#endif

    // create equidistance array of parameters, including the breaks
    std::vector<double> parameters = LinspaceWithBreaks(new_parameters.front(),
                                                        new_parameters.back(),
                                                        std::max(static_cast<size_t>(101), n_control_pnts*2),
                                                        breaks);
#ifdef MODEL_KINKS
    // insert kinks into parameters array at the correct position
    for (size_t ikink = 0; ikink < kinks.size(); ++ikink) {
        double kink = kinks[ikink];
        parameters.insert( 
            std::upper_bound( parameters.begin(), parameters.end(), kink),
            kink);
    }
#endif

    // Compute points on spline at the new parameters
    // Those will be approximated later on
    TColgp_Array1OfPnt points(1, static_cast<Standard_Integer>(parameters.size()));
    for (size_t i = 1; i <= parameters.size(); ++i) {
        double oldParameter = reparametrizing_spline->Value(parameters[i-1]).X();
        points(static_cast<Standard_Integer>(i)) = spline->Value(oldParameter);
    }

    bool makeContinuous = spline->IsClosed() &&
            spline->DN(spline->FirstParameter(), 1).Angle(spline->DN(spline->LastParameter(), 1)) < 6. / 180. * M_PI;

    // Create the new spline as a interpolation of the old one
    BSplineApproxInterp approximationObj(points, static_cast<int>(n_control_pnts), 3, makeContinuous);

    breaks.insert(breaks.begin(), new_parameters.front());
    breaks.push_back(new_parameters.back());
    // Interpolate points at breaking parameters (required for gordon surface)
    for (size_t ibreak = 0; ibreak < breaks.size(); ++ibreak) {
        double thebreak = breaks[ibreak];
        size_t idx = static_cast<size_t>(
            std::find_if(parameters.begin(), parameters.end(), IsInsideTolerance(thebreak)) -
            parameters.begin());
        approximationObj.InterpolatePoint(idx);
    }

#ifdef MODEL_KINKS
    for (size_t ikink = 0; ikink < kinks.size(); ++ikink) {
        double kink = kinks[ikink];
        size_t idx = static_cast<size_t>(
            std::find_if(parameters.begin(), parameters.end(), IsInsideTolerance(kink, par_tol)) -
            parameters.begin());
        approximationObj.InterpolatePoint(idx, true);
    }
#endif
    
    ApproxResult result = approximationObj.FitCurveOptimal(parameters);

    assert(!result.curve.IsNull());

    return result;
}

Handle(Geom_BSplineSurface) BSplineAlgorithms::flipSurface(const Handle(Geom_BSplineSurface) surface)
{
    Handle(Geom_BSplineSurface) result = Handle(Geom_BSplineSurface)::DownCast(surface->Copy());
    result->ExchangeUV();
    return result;
}

Handle(Geom_BSplineSurface) BSplineAlgorithms::pointsToSurface(const TColgp_Array2OfPnt& points,
                                                                    const std::vector<double>& uParams,
                                                                    const std::vector<double>& vParams,
                                                                    bool uContinuousIfClosed, bool vContinuousIfClosed)
{

    double tolerance = REL_TOL_CLOSED * scale(points);
    bool makeVDirClosed = vContinuousIfClosed & isVDirClosed(points, tolerance);
    bool makeUDirClosed = uContinuousIfClosed & isUDirClosed(points, tolerance);

    // first interpolate all points by B-splines in u-direction
    std::vector<Handle(Geom_Curve)> uSplines;
    for (int cpVIdx = points.LowerCol(); cpVIdx <= points.UpperCol(); ++cpVIdx) {
        Handle_TColgp_HArray1OfPnt points_u = pntArray2GetColumn(points, cpVIdx);
        PointsToBSplineInterpolation interpolationObject(points_u, uParams, 3, makeUDirClosed);

        Handle(Geom_Curve) curve = interpolationObject.Curve();
        uSplines.push_back(curve);
    }

    // now create a skinned surface with these B-splines which represents the interpolating surface
    CurvesToSurface skinner(uSplines, vParams, makeVDirClosed );
    Handle(Geom_BSplineSurface) interpolatingSurf = skinner.Surface();

    return interpolatingSurf;
}


std::vector<std::pair<double, double> > BSplineAlgorithms::intersections(const Handle(Geom_BSplineCurve) spline1, const Handle(Geom_BSplineCurve) spline2, double tolerance) {

    // find out the average scale of the two B-splines in order to being able to handle a more approximate curves and find its intersections
    double splines_scale = (BSplineAlgorithms::scale(spline1) + BSplineAlgorithms::scale(spline2)) / 2.;

    std::vector<std::pair<double, double> > intersection_params_vector;

    auto results = geoml::IntersectBSplines(spline1, spline2, tolerance*splines_scale);
    for (const auto& r : results) {
        intersection_params_vector.push_back({r.parmOnCurve1, r.parmOnCurve2});
    }

    return intersection_params_vector;
}

double BSplineAlgorithms::scale(const std::vector<Handle(Geom_BSplineCurve)>& splines_vector)
{
    double maxScale = 0.;
    for (std::vector<Handle(Geom_BSplineCurve)>::const_iterator it = splines_vector.begin(); it != splines_vector.end(); ++it) {
        maxScale = std::max(scale(*it), maxScale);
    }

    return maxScale;
}

double BSplineAlgorithms::scale(const Handle(Geom_BSplineCurve)& spline)
{
    double scale = 0.;
    gp_Pnt first_ctrl_pnt = spline->Pole(1);
    for (int ctrl_pnt_idx = 2; ctrl_pnt_idx <= spline->NbPoles(); ++ctrl_pnt_idx) {
        // compute distance of the first control point to the others and save biggest distance
        double distance = first_ctrl_pnt.Distance(spline->Pole(ctrl_pnt_idx));

        scale = std::max(scale, distance);
    }
    return scale;
}

void BSplineAlgorithms::reparametrizeBSpline(Geom_BSplineCurve& spline, double umin, double umax, double tol)
{
    if (std::abs(spline.Knot(1) - umin) > tol || std::abs(spline.Knot(spline.NbKnots()) - umax) > tol) {
        TColStd_Array1OfReal aKnots (1, spline.NbKnots());
        spline.Knots (aKnots);
        BSplCLib::Reparametrize (umin, umax, aKnots);
        spline.SetKnots (aKnots);
    }
}

void BSplineAlgorithms::reparametrizeBSpline(Geom_BSplineSurface& spline, double umin, double umax, double vmin, double vmax, double tol)
{
    if (std::abs(spline.UKnot(1) - umin) > tol || std::abs(spline.UKnot(spline.NbUKnots()) - umax) > tol) {
        TColStd_Array1OfReal aKnots (1, spline.NbUKnots());
        spline.UKnots (aKnots);
        BSplCLib::Reparametrize (umin, umax, aKnots);
        spline.SetUKnots (aKnots);
    }

    if (std::abs(spline.VKnot(1) - vmin) > tol || std::abs(spline.VKnot(spline.NbVKnots()) - vmax) > tol) {
        TColStd_Array1OfReal aKnots (1, spline.NbVKnots());
        spline.VKnots (aKnots);
        BSplCLib::Reparametrize (vmin, vmax, aKnots);
        spline.SetVKnots (aKnots);
    }
}

ApproxResult BSplineAlgorithms::reparametrizeBSplineNiceKnots(Handle(Geom_BSplineCurve) spline)
{
    if (spline.IsNull()) {
        throw Error("Null Pointer curve", geoml::NULL_POINTER);
    }

    double umin = spline->FirstParameter();
    double umax = spline->LastParameter();

    // compute the number of desired knot segments

    int nSegmentsOld = spline->NbPoles() - spline->Degree();
    // The new number of segments is a power of two
    int nSegmentsNew = (int)pow(2.0, static_cast<int>(log2(static_cast<float>(nSegmentsOld))));

    // we are using at least 8 segments to be safe with accuracy
    nSegmentsNew = std::max(nSegmentsNew, 8);

    // The reparametrization results in a degree 3 spline
    int target_degree = 3;
    return BSplineAlgorithms::reparametrizeBSplineContinuouslyApprox(spline, {umin, umax}, {umin, umax}, nSegmentsNew + target_degree);
}

math_Matrix BSplineAlgorithms::bsplineBasisMat(int degree, const TColStd_Array1OfReal& knots, const TColStd_Array1OfReal& params, unsigned int derivOrder)
{
    Standard_Integer ncp = knots.Length() - degree - 1;
    math_Matrix mx(1, params.Length(), 1, ncp);
    mx.Init(0.);
    math_Matrix bspl_basis(1, derivOrder + 1, 1, degree + 1);
    bspl_basis.Init(0.);
    for (Standard_Integer iparm = 1; iparm <= params.Length(); ++iparm) {
        Standard_Integer basis_start_index = 0;
#if OCC_VERSION_HEX >= VERSION_HEX_CODE(7,1,0)
        BSplCLib::EvalBsplineBasis(derivOrder, degree + 1, knots, params.Value(iparm), basis_start_index, bspl_basis);
#else
        BSplCLib::EvalBsplineBasis(1, derivOrder, degree + 1, knots, params.Value(iparm), basis_start_index, bspl_basis);
#endif
        if(derivOrder > 0) {
            math_Vector help_vector(1, ncp);
            help_vector.Init(0.);
            help_vector.Set(basis_start_index, basis_start_index + degree, bspl_basis.Row(derivOrder + 1));
            mx.SetRow(iparm, help_vector);
        }
        else {
            mx.Set(iparm, iparm, basis_start_index, basis_start_index + degree, bspl_basis);
        }
    }
    return mx;
}

std::vector<double> BSplineAlgorithms::getKinkParameters(const Handle(Geom_BSplineCurve)& curve)
{
    if (curve.IsNull()) {
        throw Error("Null Pointer curve", geoml::NULL_POINTER);
    }

    double eps = 1e-8;

    std::vector<double> kinks;
    for (int knotIndex = 2; knotIndex < curve->NbKnots(); ++knotIndex) {
        if (curve->Multiplicity(knotIndex) == curve->Degree()) {
            double knot = curve->Knot(knotIndex);
            // check if really a kink
            double angle = curve->DN(knot + eps, 1).Angle(curve->DN(knot - eps, 1));
            if (angle > 6./180. * M_PI) {
                kinks.push_back(knot);
            }
        }
    }

    return kinks;
}

BSplineAlgorithms::SurfaceKinks BSplineAlgorithms::getKinkParameters(const Handle(Geom_BSplineSurface)& surface)
{
    if (surface.IsNull()) {
        throw Error("Null Pointer curve", geoml::NULL_POINTER);
    }

    SurfaceKinks kinks;

    for (int knotIndex = 2; knotIndex < surface->NbUKnots(); ++knotIndex) {
        if (surface->UMultiplicity(knotIndex) == surface->UDegree()) {
            double knot = surface->UKnot(knotIndex);
            kinks.u.push_back(knot);
        }
    }

    for (int knotIndex = 2; knotIndex < surface->NbVKnots(); ++knotIndex) {
        if (surface->VMultiplicity(knotIndex) == surface->VDegree()) {
            double knot = surface->VKnot(knotIndex);
            kinks.v.push_back(knot);
        }
    }

    return kinks;
}

Handle(Geom_BSplineSurface) BSplineAlgorithms::trimSurface(const Handle(Geom_Surface)& surface, double umin, double umax, double vmin, double vmax)
{

    Handle(Geom_BSplineSurface) trimmedSurface = GeomConvert::SurfaceToBSplineSurface(surface);

    trimmedSurface->SetUNotPeriodic();
    trimmedSurface->SetVNotPeriodic();

    // workaround for OCCT bug https://tracker.dev.opencascade.org/view.php?id=31402
    // We set the trimming parameters to a knot, if they are close to them
    double parTol = 1e-10;
    int i1, i2;
    trimmedSurface->LocateU(umin, parTol, i1, i2);
    if (i1 == i2) {
        // v is a knot
        umin = trimmedSurface->UKnot(i1);
    }
    trimmedSurface->LocateU(umax, parTol, i1, i2);
    if (i1 == i2) {
        // v is a knot
        umax = trimmedSurface->UKnot(i1);
    }
    trimmedSurface->LocateV(vmin, parTol, i1, i2);
    if (i1 == i2) {
        // v is a knot
        vmin = trimmedSurface->VKnot(i1);
    }
    trimmedSurface->LocateV(vmax, parTol, i1, i2);
    if (i1 == i2) {
        // v is a knot
        vmax = trimmedSurface->VKnot(i1);
    }

    // Perform the trimming
    trimmedSurface->Segment(umin, umax, vmin, vmax);

#ifdef DEBUG
    double u1, u2, v1, v2;
    trimmedSurface->Bounds(u1, u2, v1, v2);

    double tol_check = 1e-12;
    auto intol = [tol_check](double a, double b) {
        return fabs(a - b) <= tol_check;
    };

    assert(intol(u1, umin) && intol(u2, umax) && intol(v1, vmin) && intol(v2, vmax));
#endif
    return trimmedSurface;
}

Handle(Geom_BSplineCurve) BSplineAlgorithms::trimCurve(const Handle(Geom_BSplineCurve)& curve, double umin, double umax)
{
    Handle(Geom_BSplineCurve) copy = Handle(Geom_BSplineCurve)::DownCast(curve->Copy());
    copy->Segment(umin, umax);
    return copy;
}

Handle(Geom_BSplineCurve) BSplineAlgorithms::concatCurves(std::vector<Handle(Geom_BSplineCurve)> curves,
                                                               bool parByLength, double tolerance)
{

    if (curves.size() == 0) {
        LOG(ERROR) << "Empty curve vector in BSplineAlgorithms::concatCurves";
        return nullptr;
    }
    else if (curves.size() == 1) {
        return curves[0];
    }

    std::vector<double> lengths;
    double totalLen  = 0;
    int    maxDegree = 0;

    // get the bsplines of each edge,
    // compute the lengths of each edge,
    // determine maximum degree of the curves
    for (auto curve : curves) {

        if (parByLength) {
            // find out length of current curve
            Standard_Real umin = curve->FirstParameter();
            Standard_Real umax = curve->LastParameter();
            GeomAdaptor_Curve adaptorCurve(curve, umin, umax);
            double len = GCPnts_AbscissaPoint::Length(adaptorCurve, umin, umax);
            lengths.push_back(len);
            totalLen += len;
        }

        // find out maximum degree
        if (curve->Degree() > maxDegree) {
            maxDegree = curve->Degree();
        }
    }


    // check connectivities
    for (unsigned int icurve = 1; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) c1 = curves[icurve-1];
        Handle(Geom_BSplineCurve) c2 = curves[icurve];

        gp_Pnt p1 = c1->Pole(c1->NbPoles());
        gp_Pnt p2 = c2->Pole(1);

        if (p1.Distance(p2) > tolerance) {
            // error
            LOG(ERROR) << "Curves not connected within tolerance in concatCurves";
            return nullptr;
        }
    }

    // elevate degree of all curves to maxDegree
    for (unsigned int icurve = 0; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) curve = curves[icurve];
        curve->IncreaseDegree(maxDegree);
    }

#ifdef DEBUG
    // check that each curve is at maxDegree
    for (unsigned int icurve = 0; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) curve = curves[icurve];
        assert(curve->Degree() == maxDegree);
    }
#endif

    // shift knots of curves
    double startPar = 0;
    for (unsigned int icurve = 0; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) curve = curves[icurve];
        double stopPar = startPar;
        if (parByLength) {
            stopPar += lengths[icurve]/totalLen;
        }
        else {
            stopPar += curve->LastParameter() - curve->FirstParameter();
        }
        BSplineAlgorithms::reparametrizeBSpline(*curve, startPar, stopPar);
        curves[icurve] = curve;

        startPar = stopPar;
    }

    // count number of knots and control points for the final b-spline
    int nbknots = 1;
    int nbcp    = 1;
    for (unsigned int icurve = 0; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) curve = curves[icurve];
        nbknots += curve->NbKnots() - 1;
        nbcp    += curve->NbPoles() - 1;
    }

    // allocate arrays
    TColgp_Array1OfPnt      cpoints(1, nbcp);
    TColStd_Array1OfReal    weights(1, nbcp);
    TColStd_Array1OfReal    knots(1, nbknots);
    TColStd_Array1OfInteger mults(1, nbknots);

    // concatenate everything
    int iknotT = 1, imultT = 1, icpT = 1, iweightT = 1;
    for (unsigned int icurve = 0; icurve < curves.size(); ++icurve) {
        Handle(Geom_BSplineCurve) curve = curves[icurve];

        // special handling of the first knot, control point
        knots.SetValue(iknotT++, curve->Knot(1));
        if (icurve == 0) {
            // we just copy the data of the very first point/knot
            mults.SetValue(imultT++, curve->Multiplicity(1));
            cpoints.SetValue(icpT++, curve->Pole(1));
            weights.SetValue(iweightT++, curve->Weight(1));
        }
        else {
            // set multiplicity to maxDegree to allow c0 concatenation
            mults.SetValue(imultT++, maxDegree);

            // compute midpoint between endpoint of previous
            // curve and startpoint of current curve
            Handle(Geom_BSplineCurve) lastCurve = curves[icurve-1];
            gp_Pnt endPoint   = lastCurve->Pole(lastCurve->NbPoles());
            gp_Pnt startPoint = curve->Pole(1);
            gp_Pnt midPoint = (endPoint.XYZ() + startPoint.XYZ())/2.;
            cpoints.SetValue(icpT++, midPoint);

            // we use the average weight of previous curve and current curve
            // This is probably wrong and could change the shape of the curve
            // Instead, one could scale all weights of the curve to match the weight of
            // the previous curve
            weights.SetValue(iweightT++, (lastCurve->Weight(lastCurve->NbPoles()) + curve->Weight(1))/2.);
        }

        // just copy control points, weights, knots and multiplicities
        for (int iknot = 2; iknot < curve->NbKnots(); ++iknot) {
            knots.SetValue(iknotT++, curve->Knot(iknot));
            mults.SetValue(imultT++, curve->Multiplicity(iknot));
        }
        for (int icp = 2; icp < curve->NbPoles(); ++icp) {
            cpoints.SetValue(icpT++, curve->Pole(icp));
            weights.SetValue(iweightT++, curve->Weight(icp));
        }

    }

    // special handling of the last point and knot
    Handle(Geom_BSplineCurve) lastCurve = curves[curves.size()-1];
    knots.SetValue(iknotT, lastCurve->Knot(lastCurve->NbKnots()));
    mults.SetValue(imultT,  lastCurve->Multiplicity(lastCurve->NbKnots()));
    cpoints.SetValue(icpT, lastCurve->Pole(lastCurve->NbPoles()));
    weights.SetValue(iweightT, lastCurve->Weight(lastCurve->NbPoles()));

#ifdef DEBUG
    // check that we have the correct number of knots, control points etc...
    int nkn = 0;
    for (int ik = knots.Lower(); ik <= knots.Upper(); ++ik) {
        nkn += mults.Value(ik);
    }

    // check validity of bspline
    assert (cpoints.Length() + maxDegree + 1 == nkn);
#endif

    // build the resulting B-Spline
    Handle(Geom_BSplineCurve) result = new Geom_BSplineCurve(cpoints, weights, knots, mults, maxDegree, false);
    return result;
}

Handle(Geom_BSplineSurface) BSplineAlgorithms::concatSurfacesUDir(Handle(Geom_BSplineSurface) bspl1,
                                                                       Handle(Geom_BSplineSurface) bspl2,
                                                                       double space_tol)
{

    DEBUG_SCOPE(debug);
    debug.addShape(BRepBuilderAPI_MakeFace(bspl1, 1e-6).Face(), "surf1");
    debug.addShape(BRepBuilderAPI_MakeFace(bspl2, 1e-6).Face(), "surf2");

    // the surfaces must not be periodic in u direction
    assert (bspl1->IsUPeriodic() == false);
    assert (bspl2->IsUPeriodic() == false);

    // we dont have nurbs implemented right now
    assert (bspl1->IsURational() == false);
    assert (bspl2->IsURational() == false);
    assert (bspl1->IsVRational() == false);
    assert (bspl2->IsVRational() == false);

    double param_tol = 1e-14;

    // check that surfaces are following parametrically
    double umin1, umax1, vmin1, vmax1;
    bspl1->Bounds(umin1, umax1, vmin1, vmax1);
    double umin2, umax2, vmin2, vmax2;
    bspl2->Bounds(umin2, umax2, vmin2, vmax2);

    if (std::abs(umax1 - umin2) > param_tol) {
        std::stringstream str;
        str << "Surfaces do not follow in u-parametric direction in BSplineAlgorithms::concatSurfacesUDir. ";
        str << "Surface 1 ends at " << umax1 << ", Surface 2 begins at " << umin2 << "!";
        throw Error(str.str(), geoml::MATH_ERROR);
    }

    bspl1->SetVNotPeriodic();
    bspl2->SetVNotPeriodic();

    auto u_degree = std::max(bspl1->UDegree(), bspl2->UDegree());
    auto v_degree = std::max(bspl1->VDegree(), bspl2->VDegree());

    bspl1->IncreaseDegree(u_degree, v_degree);
    bspl2->IncreaseDegree(u_degree, v_degree);

    // check that corner points match
    auto leftCornerDist = bspl1->Value(umax1, vmin1).Distance(bspl2->Value(umin2, vmin2));
    auto rightCornerDist = bspl1->Value(umax1, vmax1).Distance(bspl2->Value(umin2, vmax2)) ;
    if (leftCornerDist > space_tol || rightCornerDist > space_tol) {
        throw Error("Surfaces don't match within tolerances in BSplineAlgorithms::concatSurfacesUDir.", geoml::MATH_ERROR);
    }
    
    auto spl_vec = BSplineAlgorithms::createCommonKnotsVectorSurface({bspl1, bspl2}, SurfaceDirection::v);
    bspl1 = spl_vec[0];
    bspl2 = spl_vec[1];

    assert(bspl1->NbVPoles() == bspl2->NbVPoles());

    // concat control points
    TColgp_Array2OfPnt cpsNew(1, bspl1->NbUPoles() + bspl2->NbUPoles() - 1, 1, bspl1->NbVPoles());
    for (int iv = 0; iv < bspl1->NbVPoles(); ++iv) {
        for (int iu = 0; iu < bspl1->NbUPoles() - 1; ++iu) {
            cpsNew.SetValue(iu+1, iv+1, bspl1->Pole(iu+1, iv+1));
        }
        auto offset = bspl1->NbUPoles();
        for (int iu = 0; iu < bspl2->NbUPoles(); ++iu) {
            cpsNew.SetValue(iu + offset, iv+1, bspl2->Pole(iu+1, iv+1));
        }
    }

    // concat knots and mults
    TColStd_Array1OfReal uknots_new(1, bspl1->NbUKnots() + bspl2->NbUKnots() - 1);
    TColStd_Array1OfInteger umults_new(1, bspl1->NbUKnots() + bspl2->NbUKnots() - 1);

    for (int i = 0; i < bspl1->NbUKnots()-1; ++i) {
        uknots_new.SetValue(i+1, bspl1->UKnot(i+1));
        umults_new.SetValue(i+1, bspl1->UMultiplicity(i+1));
    }

    int middleIdx = bspl1->NbUKnots();
    uknots_new.SetValue(middleIdx, 0.5 * (bspl1->UKnot(middleIdx) + bspl2->UKnot(1)));
    umults_new.SetValue(middleIdx, bspl1->UMultiplicity(middleIdx) - 1);

    for (int i = 1; i < bspl2->NbUKnots(); ++i) {
        uknots_new.SetValue(i + middleIdx, bspl2->UKnot(i+1));
        umults_new.SetValue(i + middleIdx, bspl2->UMultiplicity(i+1));
    }

    // we simply take the v-knots of the first spline
    TColStd_Array1OfReal vknots_new(1, bspl1->NbVKnots());
    TColStd_Array1OfInteger vmults_new(1, bspl1->NbVKnots());
    bspl1->VKnots(vknots_new);
    bspl1->VMultiplicities(vmults_new);

#ifdef DEBUG
    int sum_umults = 0;
    for (int i = umults_new.Lower(); i <= umults_new.Upper(); ++i) {
        sum_umults += umults_new.Value(i);
    }

    int sum_vmults = 0;
    for (int i = vmults_new.Lower(); i <= vmults_new.Upper(); ++i) {
        sum_vmults += vmults_new.Value(i);
    }

    assert(cpsNew.ColLength() + u_degree + 1 == sum_umults);
    assert(cpsNew.RowLength() + v_degree + 1 == sum_vmults);
#endif

    return new Geom_BSplineSurface(cpsNew, uknots_new, vknots_new,
                                   umults_new, vmults_new,
                                   u_degree, v_degree, false, false);
}

Handle(Geom_BSplineSurface) BSplineAlgorithms::makeKnotsUniform(Handle(Geom_BSplineSurface) surf, unsigned int nseg_u, unsigned int nseg_v)
{
    double u1, u2, v1, v2;
    surf->Bounds(u1, u2, v1, v2);

    // we create a degree 3 approximation in all directions
    auto ncp_u = std::max(4, static_cast<int>(nseg_u + 1));
    auto ncp_v = std::max(4, static_cast<int>(nseg_v + 1));

    auto u = LinspaceWithBreaks(u1, u2, ncp_u, {});
    auto v = LinspaceWithBreaks(v1, v2, ncp_v, {});

    TColgp_Array2OfPnt points(1, ncp_u, 1, ncp_v);

    for (size_t i = 0; i < u.size(); ++i) {
        for (size_t j = 0; j < v.size(); ++j) {
            auto pnt = surf->Value(u[i], v[j]);
            points.SetValue(static_cast<Standard_Integer>(i+1), static_cast<Standard_Integer>(j+1), pnt);
        }
    }
    
    return BSplineAlgorithms::pointsToSurface(points, u, v, true, true);
}

} // namespace geoml
