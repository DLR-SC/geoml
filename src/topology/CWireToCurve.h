/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2014-11-17 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef WIRETOCURVE_H
#define WIRETOCURVE_H

#include <TopoDS_Wire.hxx>
#include <Geom_BSplineCurve.hxx>

#include "geoml_internal.h"
#include <vector>

namespace geoml
{

/**
 * @brief This class converts a TopoDS_Wire objects to a bspline curve
 * by converting all edges to b-spline concatenating all splines to
 * a resulting b-spline
 */
class CWireToCurve
{
public:
    /**
     * @brief CWireToCurve constructor
     * @param parByLength If true, the resulting B-Spline parameter sub-ranges will be proportional
     *        to the sub-curve lengths. I.e. a long edge will have a larger parameter range than a short
     *        one. If false, the sub-parameter ranges will be the same as before, i.e. a curve with a large
     *        parameter range will result in a large range in the resulting curve.
     * @param tolerance Maximum distance of the connection point of two edges. If distance is larger than 
     *        tolerance, the class returns a NULL Handle.
     */
    GEOML_EXPORT CWireToCurve(const TopoDS_Wire&, bool parByLength=true, double tolerance = 1e-6);

    /// Returns the resulting spline
    /// Returns NULL Handle in case of an error
    GEOML_EXPORT operator Handle(Geom_BSplineCurve) ();
    GEOML_EXPORT Handle(Geom_BSplineCurve) curve();

private:
    TopoDS_Wire _wire;
    double _tolerance;
    bool _parByLength;
};

} // namespacae geoml

#endif // WIRETOCURVE_H
