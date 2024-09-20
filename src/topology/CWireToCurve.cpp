/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2014-11-17 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CWireToCurve.h"
#include "logging/Logging.h"
#include "geoml/error.h"

#include "BSplineAlgorithms.h"
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>

#include <BRep_Tool.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>

#include <ShapeFix_Wire.hxx>

#include <vector>


namespace geoml
{

CWireToCurve::CWireToCurve(const TopoDS_Wire& w, bool parByLength, double t)
{
    _wire = w;
    _tolerance = t;
    _parByLength = parByLength;
}


Handle(Geom_BSplineCurve) CWireToCurve::curve()
{
    if (_wire.IsNull()) {
        LOG(ERROR) << "Wire is null in CWireToCurve::curve";
        return nullptr;
    }
    
    // fix order of wires
    ShapeFix_Wire wireFixer;
    wireFixer.Load(_wire);
    wireFixer.FixReorder();
    wireFixer.Perform();
    
    TopoDS_Wire theWire = wireFixer.Wire();

    std::vector<Handle(Geom_BSplineCurve)> curves;


    // get the bsplines of each edge, 
    for (TopExp_Explorer exp(theWire, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge e = TopoDS::Edge(exp.Current());

        double u1, u2;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(e, u1, u2);
        curve = new Geom_TrimmedCurve(curve, u1, u2);
        if (e.Orientation() == TopAbs_REVERSED) {
            curve->Reverse();
        }

        // convert to bspline
        Handle(Geom_BSplineCurve) bspl = GeomConvert::CurveToBSplineCurve(curve);
        curves.push_back(bspl);
    }
    
    return BSplineAlgorithms::concatCurves(curves, _parByLength, _tolerance);

}

CWireToCurve::operator Handle(Geom_BSplineCurve)()
{
    return curve();
}

} // namespace geoml

