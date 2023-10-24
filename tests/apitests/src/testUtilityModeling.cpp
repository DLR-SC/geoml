#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>
#include <geoml/utility/modeling.h>

#include <gtest/gtest.h>
#include <filesystem>
#include <cmath>
#include <vector>

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
#include <BRepBuilderAPI_MakeFace.hxx>
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
#include <TColgp_Array2OfPnt.hxx>


TEST(control_point_handling, extract_control_points_from_b_spline_surface)
{        
// define profile curve for the revolving surface (cylinder):

// control points:
gp_Pnt start_point_profile(7000., 0., 2000.);
gp_Pnt end_point_profile(23000., 0., 2000.); 

TColgp_Array1OfPnt profile_points_cylinder(1,2);
profile_points_cylinder.SetValue(1,start_point_profile);
profile_points_cylinder.SetValue(2,end_point_profile);

// degree:
Standard_Integer degree_profile = 1;

// weights:
TColStd_Array1OfReal weights_profile(1, 2);
weights_profile.SetValue(1, 1.0);  
weights_profile.SetValue(2, 1.0);  

// knots:
TColStd_Array1OfReal knots_profile(1,2); 
knots_profile.SetValue(1,0.0);           
knots_profile.SetValue(2,1.0);

// multiplicities: 
TColStd_Array1OfInteger mults_profile(1,2); 
mults_profile.SetValue(1,2);                
mults_profile.SetValue(2,2);

// create the profile curve:
Handle(Geom_BSplineCurve) profile_curve_cylinder 
	= geoml::b_spline_curve(profile_points_cylinder, weights_profile, 
							knots_profile, mults_profile, degree_profile);

// use this profile curve to define the cylinder:

Handle(Geom_BSplineSurface) fuselage_cylinder 
	= geoml::revolving_surface(profile_curve_cylinder, 
							 gp_Ax1(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.)));

// extract the control points:
TColgp_Array2OfPnt test_cpoints = geoml::extract_control_points_surface(fuselage_cylinder);

EXPECT_EQ(test_cpoints.RowLength(), 2); // V direction
EXPECT_EQ(test_cpoints.ColLength(), 6); // U direction

EXPECT_TRUE(fuselage_cylinder->IsUPeriodic());

fuselage_cylinder->SetUNotPeriodic();

TColgp_Array2OfPnt test_cpoints_2 = geoml::extract_control_points_surface(fuselage_cylinder);

EXPECT_EQ(test_cpoints_2.RowLength(), 2);
EXPECT_EQ(test_cpoints_2.ColLength(), 7); // one more than before

}


TEST(control_point_handling, extract_control_points_column_row_from_b_spline_surface)
{        
// define profile curve for the revolving surface (cylinder):

// control points:
gp_Pnt start_point_profile(7000., 0., 2000.);
gp_Pnt end_point_profile(23000., 0., 2000.); 

TColgp_Array1OfPnt profile_points_cylinder(1,2);
profile_points_cylinder.SetValue(1,start_point_profile);
profile_points_cylinder.SetValue(2,end_point_profile);

// degree:
Standard_Integer degree_profile = 1;

// weights:
TColStd_Array1OfReal weights_profile(1, 2);
weights_profile.SetValue(1, 1.0);  
weights_profile.SetValue(2, 1.0);  

// knots:
TColStd_Array1OfReal knots_profile(1,2); 
knots_profile.SetValue(1,0.0);           
knots_profile.SetValue(2,1.0);

// multiplicities: 
TColStd_Array1OfInteger mults_profile(1,2); 
mults_profile.SetValue(1,2);                
mults_profile.SetValue(2,2);

// create the profile curve:
Handle(Geom_BSplineCurve) profile_curve_cylinder 
	= geoml::b_spline_curve(profile_points_cylinder, weights_profile, 
							knots_profile, mults_profile, degree_profile);

// use this profile curve to define the cylinder:

Handle(Geom_BSplineSurface) fuselage_cylinder 
	= geoml::revolving_surface(profile_curve_cylinder, 
							 gp_Ax1(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.)));

// extract the 2nd control point column (U-direction) of fuselage_cylinder:

TColgp_Array1OfPnt cp_column = geoml::extract_control_point_column_row(fuselage_cylinder,1,2);

EXPECT_TRUE(fuselage_cylinder->IsUPeriodic());

EXPECT_NEAR(cp_column.Value(1).X(), 23000,0.1);
EXPECT_NEAR(cp_column.Value(1).Y(), 0, 0.1);
EXPECT_NEAR(cp_column.Value(1).Z(), 2000, 0.1);

EXPECT_NEAR(cp_column.Value(2).X(), 23000, 0.1);
EXPECT_NEAR(cp_column.Value(2).Y(), -3464.1, 0.1);
EXPECT_NEAR(cp_column.Value(2).Z(), 2000, 0.1);

EXPECT_NEAR(cp_column.Value(3).X(), 23000, 0.1);
EXPECT_NEAR(cp_column.Value(3).Y(), -1732.05, 0.1);
EXPECT_NEAR(cp_column.Value(3).Z(), -1000, 0.1);

EXPECT_NEAR(cp_column.Value(4).X(), 23000, 0.1);
EXPECT_NEAR(cp_column.Value(4).Y(), 0, 0.1);
EXPECT_NEAR(cp_column.Value(4).Z(), -4000, 0.1);

EXPECT_NEAR(cp_column.Value(5).X(), 23000, 0.1);
EXPECT_NEAR(cp_column.Value(5).Y(), 1732.05, 0.1);
EXPECT_NEAR(cp_column.Value(5).Z(), -1000, 0.1);

EXPECT_NEAR(cp_column.Value(6).X(), 23000, 0.1);
EXPECT_NEAR(cp_column.Value(6).Y(), 3464.1, 0.1);
EXPECT_NEAR(cp_column.Value(6).Z(), 2000, 0.1);

// in the periodic case, as so far, there is no 7th point in this column

fuselage_cylinder->SetUNotPeriodic();

TColgp_Array1OfPnt cp_column_2 = geoml::extract_control_point_column_row(fuselage_cylinder,1,2);

EXPECT_NEAR(cp_column_2.Value(1).X(), 23000,0.1);
EXPECT_NEAR(cp_column_2.Value(1).Y(), 0, 0.1);
EXPECT_NEAR(cp_column_2.Value(1).Z(), 2000, 0.1);

EXPECT_NEAR(cp_column_2.Value(2).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(2).Y(), -3464.1, 0.1);
EXPECT_NEAR(cp_column_2.Value(2).Z(), 2000, 0.1);

EXPECT_NEAR(cp_column_2.Value(3).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(3).Y(), -1732.05, 0.1);
EXPECT_NEAR(cp_column_2.Value(3).Z(), -1000, 0.1);

EXPECT_NEAR(cp_column_2.Value(4).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(4).Y(), 0, 0.1);
EXPECT_NEAR(cp_column_2.Value(4).Z(), -4000, 0.1);

EXPECT_NEAR(cp_column_2.Value(5).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(5).Y(), 1732.05, 0.1);
EXPECT_NEAR(cp_column_2.Value(5).Z(), -1000, 0.1);

EXPECT_NEAR(cp_column_2.Value(6).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(6).Y(), 3464.1, 0.1);
EXPECT_NEAR(cp_column_2.Value(6).Z(), 2000, 0.1);

EXPECT_NEAR(cp_column_2.Value(7).X(), 23000, 0.1);
EXPECT_NEAR(cp_column_2.Value(7).Y(), 0, 0.1);
EXPECT_NEAR(cp_column_2.Value(7).Z(), 2000, 0.1);


}
