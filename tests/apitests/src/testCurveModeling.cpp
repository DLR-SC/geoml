//#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <filesystem>
#include <iostream>
#include <cmath>
#include <vector>
#include <geoml/surfaces/modeling.h>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include "Geom_Curve.hxx"
#include <GeomAPI_PointsToBSpline.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "STEPControl_Writer.hxx"
