#include <iostream>
#include <cmath>
#include <vector>

#include <geoml/curves/modeling.h>
#include <geoml/surfaces/modeling.h>
#include <geoml/utility/modeling.h>


#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include "Geom_Curve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include <TopoDS_Shape.hxx>
#include "STEPControl_Writer.hxx"

#include <GeomAPI_PointsToBSpline.hxx>
//#include <NCollection_Array1.hxx> // for using TColgp_Array1OfPnt
#include <TColgp_Array1OfPnt.hxx>

#include <BRepPrimAPI_MakeRevol.hxx>
#include <GC_MakeSegment.hxx>

#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"

#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Circ.hxx>

#include <GeomConvert.hxx>

#include <NCollection_Array2.hxx>

#include <BRepBuilderAPI_NurbsConvert.hxx>

#include <GeomConvert.hxx>

#include <TColgp_Array2OfPnt.hxx>
#include <gp_Vec.hxx>

#include <GC_MakeArcOfCircle.hxx>

#include <Geom_TrimmedCurve.hxx>

#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepBuilderAPI_Sewing.hxx>

#include <gp_Pln.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <BRepAlgoAPI_Cut.hxx>

#include <BRepPrimAPI_MakeSphere.hxx>
#include <initializer_list>

// define a function that writes curves to step files:
void writeToStp(Handle_Geom_Curve cur, std::string const& name_file)
{
BRepBuilderAPI_MakeEdge edgeMaker;
edgeMaker.Init(cur);
TopoDS_Shape edge = edgeMaker.Shape();

STEPControl_Writer aStepWriter;
aStepWriter.Transfer(edge,STEPControl_AsIs);
aStepWriter.Write(name_file.c_str());
} 


void writeGeomEntityToStepFile(Handle_Geom_Surface surface, std::string fileName)
{
	BRepBuilderAPI_MakeFace faceMaker;
	faceMaker.Init(surface,true,1E-6);	
	TopoDS_Shape face = faceMaker.Shape();

	STEPControl_Writer writer;
	writer.Transfer(face,STEPControl_AsIs);
	writer.Write(fileName.c_str());
	return;
}


void writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName)
{
STEPControl_Writer writer;
writer.Transfer(my_shape,STEPControl_AsIs);
writer.Write(fileName.c_str());
}

int main(){

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////																		   /////////
/////////					        // Configuration //							   /////////
/////////							///////////////////							   /////////
/////////																		   /////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////  
//////////   														
//////////	Fuselage 	 
//////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////  


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Mid part
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


// define profile curve for the revolving surface (cylinder):

// control points:
gp_Pnt start_point_profile(7000., 0., 2000.); // original: z = 2000
gp_Pnt end_point_profile(23000., 0., 2000.);  //

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
gp_Ax1 rot_axis(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.));

Handle(Geom_BSplineSurface) fuselage_cylinder =
	geoml::revolving_surface(profile_curve_cylinder, 
							 rot_axis);

// write the surface fuselage_cylinder to file:
writeGeomEntityToStepFile(fuselage_cylinder, "config_fuselage_cylinder.stp");


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Mid part
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


// extract the 2nd control point column (U-direction) of fuselage_cylinder: (control points
// of rear edge)

TColgp_Array1OfPnt cp_column = geoml::extract_control_point_column_row(fuselage_cylinder,1,2);

std::cout << "first point: " << std::endl;
std::cout << cp_column.Value(1).X() << std::endl;
std::cout << cp_column.Value(1).Y() << std::endl;
std::cout << cp_column.Value(1).Z() << std::endl;

std::cout << "second point: " << std::endl;
std::cout << cp_column.Value(2).X() << std::endl;
std::cout << cp_column.Value(2).Y() << std::endl;
std::cout << cp_column.Value(2).Z() << std::endl;

std::cout << "third point: " << std::endl;
std::cout << cp_column.Value(3).X() << std::endl;
std::cout << cp_column.Value(3).Y() << std::endl;
std::cout << cp_column.Value(3).Z() << std::endl;

std::cout << "fourth point: " << std::endl;
std::cout << cp_column.Value(4).X() << std::endl;
std::cout << cp_column.Value(4).Y() << std::endl;
std::cout << cp_column.Value(4).Z() << std::endl;

std::cout << "fifth point: " << std::endl;
std::cout << cp_column.Value(5).X() << std::endl;
std::cout << cp_column.Value(5).Y() << std::endl;
std::cout << cp_column.Value(5).Z() << std::endl;

std::cout << "sixth point: " << std::endl;
std::cout << cp_column.Value(6).X() << std::endl;
std::cout << cp_column.Value(6).Y() << std::endl;
std::cout << cp_column.Value(6).Z() << std::endl;

std::cout << "seventh point: not for the periodic case " << std::endl;

fuselage_cylinder->SetUNotPeriodic();

TColgp_Array1OfPnt cp_column_2 
	= geoml::extract_control_point_column_row(fuselage_cylinder,1,2);

std::cout << "first point: " << std::endl;
std::cout << cp_column_2.Value(1).X() << std::endl;
std::cout << cp_column_2.Value(1).Y() << std::endl;
std::cout << cp_column_2.Value(1).Z() << std::endl;

std::cout << "second point: " << std::endl;
std::cout << cp_column_2.Value(2).X() << std::endl;
std::cout << cp_column_2.Value(2).Y() << std::endl;
std::cout << cp_column_2.Value(2).Z() << std::endl;

std::cout << "third point: " << std::endl;
std::cout << cp_column_2.Value(3).X() << std::endl;
std::cout << cp_column_2.Value(3).Y() << std::endl;
std::cout << cp_column_2.Value(3).Z() << std::endl;

std::cout << "fourth point: " << std::endl;
std::cout << cp_column_2.Value(4).X() << std::endl;
std::cout << cp_column_2.Value(4).Y() << std::endl;
std::cout << cp_column_2.Value(4).Z() << std::endl;

std::cout << "fifth point: " << std::endl;
std::cout << cp_column_2.Value(5).X() << std::endl;
std::cout << cp_column_2.Value(5).Y() << std::endl;
std::cout << cp_column_2.Value(5).Z() << std::endl;

std::cout << "sixth point: " << std::endl;
std::cout << cp_column_2.Value(6).X() << std::endl;
std::cout << cp_column_2.Value(6).Y() << std::endl;
std::cout << cp_column_2.Value(6).Z() << std::endl;

std::cout << "seventh point: " << std::endl;
std::cout << cp_column_2.Value(7).X() << std::endl;
std::cout << cp_column_2.Value(7).Y() << std::endl;
std::cout << cp_column_2.Value(7).Z() << std::endl;

// now, create two copies of the periodic rear control points cp_column and move them
// in x-direction of the world coordinate system, relative to each other:

// define the direction vector:
gp_Vec move_vec(1.0, 0.0, 0.0); 

// define the factor multiplied to this vector for the first move:
double move_factor_1(2500.0); // 

// move the control points cp_column (for the periodic case):
TColgp_Array1OfPnt moved_cp_points_1 = geoml::move(cp_column, move_vec, move_factor_1);

// see, what the output is:
std::cout << "first moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(1).X() << std::endl;
std::cout << moved_cp_points_1.Value(1).Y() << std::endl;
std::cout << moved_cp_points_1.Value(1).Z() << std::endl;

std::cout << "second moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(2).X() << std::endl;
std::cout << moved_cp_points_1.Value(2).Y() << std::endl;
std::cout << moved_cp_points_1.Value(2).Z() << std::endl;

std::cout << "third moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(3).X() << std::endl;
std::cout << moved_cp_points_1.Value(3).Y() << std::endl;
std::cout << moved_cp_points_1.Value(3).Z() << std::endl;

std::cout << "fourth moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(4).X() << std::endl;
std::cout << moved_cp_points_1.Value(4).Y() << std::endl;
std::cout << moved_cp_points_1.Value(4).Z() << std::endl;

std::cout << "fifth moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(5).X() << std::endl;
std::cout << moved_cp_points_1.Value(5).Y() << std::endl;
std::cout << moved_cp_points_1.Value(5).Z() << std::endl;

std::cout << "sixth moved point: " << std::endl;
std::cout << moved_cp_points_1.Value(6).X() << std::endl;
std::cout << moved_cp_points_1.Value(6).Y() << std::endl;
std::cout << moved_cp_points_1.Value(6).Z() << std::endl;

std::cout << "seventh moved point: not for the periodic case " << std::endl;

// move the control points moved_cp_points_1 (still the periodic case):

// define the factor multiplied to move_vec for the second (relative) move:
double move_factor_2(3000.0);

TColgp_Array1OfPnt moved_cp_points_2 
	= geoml::move(moved_cp_points_1, move_vec, move_factor_2);

// see, what the output is:
std::cout << "first moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(1).X() << std::endl;
std::cout << moved_cp_points_2.Value(1).Y() << std::endl;
std::cout << moved_cp_points_2.Value(1).Z() << std::endl;

std::cout << "second moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(2).X() << std::endl;
std::cout << moved_cp_points_2.Value(2).Y() << std::endl;
std::cout << moved_cp_points_2.Value(2).Z() << std::endl;

std::cout << "third moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(3).X() << std::endl;
std::cout << moved_cp_points_2.Value(3).Y() << std::endl;
std::cout << moved_cp_points_2.Value(3).Z() << std::endl;

std::cout << "fourth moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(4).X() << std::endl;
std::cout << moved_cp_points_2.Value(4).Y() << std::endl;
std::cout << moved_cp_points_2.Value(4).Z() << std::endl;

std::cout << "fifth moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(5).X() << std::endl;
std::cout << moved_cp_points_2.Value(5).Y() << std::endl;
std::cout << moved_cp_points_2.Value(5).Z() << std::endl;

std::cout << "sixth moved point (2): " << std::endl;
std::cout << moved_cp_points_2.Value(6).X() << std::endl;
std::cout << moved_cp_points_2.Value(6).Y() << std::endl;
std::cout << moved_cp_points_2.Value(6).Z() << std::endl;

// next, define two hand picked 1-d collections of points; these will be the two last
// control point columns of the fuselage rear surface:

// second last column of control points (periodic case) - hand selected:
gp_Pnt sl_pt_1(31500.0, 0.0, 1992.0);
gp_Pnt sl_pt_2(31500.0, -3464.0, 1992.0);
gp_Pnt sl_pt_3(31500.0, -1732.0, -679.0);
gp_Pnt sl_pt_4(31500.0, 0.0, -3350.0);
gp_Pnt sl_pt_5(31500.0, 1732.0, -679.0);
gp_Pnt sl_pt_6(31500.0, 3464.0, 1992.0);

TColgp_Array1OfPnt moved_cp_points_3(1,6);

moved_cp_points_3.SetValue(1, sl_pt_1);
moved_cp_points_3.SetValue(2, sl_pt_2);
moved_cp_points_3.SetValue(3, sl_pt_3);
moved_cp_points_3.SetValue(4, sl_pt_4);
moved_cp_points_3.SetValue(5, sl_pt_5);
moved_cp_points_3.SetValue(6, sl_pt_6);

// last column of control points (periodic case):
gp_Pnt l_pt_1(37500.0, 0.0, 1337.0);
gp_Pnt l_pt_2(37500.0, -631.0, 1337.0);
gp_Pnt l_pt_3(37500.0, -316.0, 795.0);
gp_Pnt l_pt_4(37500.0, 0.0, 254.0);
gp_Pnt l_pt_5(37500.0, 316.0, 795.0);
gp_Pnt l_pt_6(37500.0, 631.0, 1337.0);

TColgp_Array1OfPnt moved_cp_points_4(1,6);

moved_cp_points_4.SetValue(1, l_pt_1);
moved_cp_points_4.SetValue(2, l_pt_2);
moved_cp_points_4.SetValue(3, l_pt_3);
moved_cp_points_4.SetValue(4, l_pt_4);
moved_cp_points_4.SetValue(5, l_pt_5);
moved_cp_points_4.SetValue(6, l_pt_6);

// create a control point net consisting of the defined four columns:
std::vector<TColgp_Array1OfPnt> my_point_lists;

my_point_lists.push_back(cp_column);
my_point_lists.push_back(moved_cp_points_1);
my_point_lists.push_back(moved_cp_points_2);
my_point_lists.push_back(moved_cp_points_3);
my_point_lists.push_back(moved_cp_points_4);

// the point arrays should be the columns of the control point net:
TColgp_Array2OfPnt rear_srf_cp_net = geoml::create_point_net_from_arrays (my_point_lists, 2);

std::cout << "rear_srf_cp_net.ColLength(): " << rear_srf_cp_net.ColLength() << std::endl;
std::cout << "rear_srf_cp_net.RowLength(): " << rear_srf_cp_net.RowLength() << std::endl;

// set the degree in U (columns) and V (rows) direction:
// degree:
Standard_Integer degree_U = 2;
Standard_Integer degree_V = 4;

// define the weight net for the fuselage rear surface:
TColStd_Array2OfReal rear_srf_weight_net(1, rear_srf_cp_net.ColLength(), 1, 
												rear_srf_cp_net.RowLength()); // is this use correct?

std::cout << "rear_srf_weight_net.ColLength(): " << rear_srf_weight_net.ColLength() << std::endl;
std::cout << "rear_srf_weight_net.RowLength(): " << rear_srf_weight_net.RowLength() << std::endl;

rear_srf_weight_net.SetValue(1, 1, 1.0);
rear_srf_weight_net.SetValue(2, 1, 0.5);
rear_srf_weight_net.SetValue(3, 1, 1.0);
rear_srf_weight_net.SetValue(4, 1, 0.5);
rear_srf_weight_net.SetValue(5, 1, 1.0);
rear_srf_weight_net.SetValue(6, 1, 0.5);

rear_srf_weight_net.SetValue(1, 2, 1.0);
rear_srf_weight_net.SetValue(2, 2, 0.5);
rear_srf_weight_net.SetValue(3, 2, 1.0);
rear_srf_weight_net.SetValue(4, 2, 0.5);
rear_srf_weight_net.SetValue(5, 2, 1.0);
rear_srf_weight_net.SetValue(6, 2, 0.5);

rear_srf_weight_net.SetValue(1, 3, 1.0);
rear_srf_weight_net.SetValue(2, 3, 0.5);
rear_srf_weight_net.SetValue(3, 3, 1.0);
rear_srf_weight_net.SetValue(4, 3, 0.5);
rear_srf_weight_net.SetValue(5, 3, 1.0);
rear_srf_weight_net.SetValue(6, 3, 0.5);

rear_srf_weight_net.SetValue(1, 4, 1.0);
rear_srf_weight_net.SetValue(2, 4, 0.5);
rear_srf_weight_net.SetValue(3, 4, 1.0);
rear_srf_weight_net.SetValue(4, 4, 0.5);
rear_srf_weight_net.SetValue(5, 4, 1.0);
rear_srf_weight_net.SetValue(6, 4, 0.5);

rear_srf_weight_net.SetValue(1, 5, 1.0);
rear_srf_weight_net.SetValue(2, 5, 0.5);
rear_srf_weight_net.SetValue(3, 5, 1.0);
rear_srf_weight_net.SetValue(4, 5, 0.5);
rear_srf_weight_net.SetValue(5, 5, 1.0);
rear_srf_weight_net.SetValue(6, 5, 0.5);

// set the U-knots (columns):
TColStd_Array1OfReal knots_U(1,4); 

knots_U.SetValue(1,0.0);           
knots_U.SetValue(2,1.0);
knots_U.SetValue(3,2.0);
knots_U.SetValue(4,3.0);

// set the V-knots (rows):
TColStd_Array1OfReal knots_V(1,2); 

knots_V.SetValue(1,0.0);           
knots_V.SetValue(2,1.0);

// multiplicities U-direction: 
TColStd_Array1OfInteger mults_U(1,4); // as we are in the periodic case, the mults must 	
								      // maybe be adapted...(!)
mults_U.SetValue(1,2);                
mults_U.SetValue(2,2);
mults_U.SetValue(3,2);
mults_U.SetValue(4,2);

// multiplicities V-direction: 
TColStd_Array1OfInteger mults_V(1,2); 

mults_V.SetValue(1,5);                
mults_V.SetValue(2,5);

// define the rear surface:
Handle(Geom_BSplineSurface) rear_surface
	= geoml::b_spline_surface( rear_srf_cp_net, 
                 		rear_srf_weight_net, 
                 		knots_U, 
                 		knots_V, 
                 		mults_U, 
                 		mults_V, 
                 		degree_U, 
                 		degree_V, 
                 		Standard_True ); 
                                     
// write the surface rear_surface to file:
writeGeomEntityToStepFile(rear_surface, "config_fuselage_rear.stp");

// rear cap:

// first edge:

// the first edge is given by the last control point row of the rear surface: moved_cp_points_4
// the second edge by 6 overlapping points cp_points_overl. We work in the periodic case.

gp_Pnt ovl_pt( (moved_cp_points_4.Value(1).X() + moved_cp_points_4.Value(3).X() + moved_cp_points_4.Value(5).X())/3,
			   (moved_cp_points_4.Value(1).Y() + moved_cp_points_4.Value(3).Y() + moved_cp_points_4.Value(5).Y())/3,
			   (moved_cp_points_4.Value(1).Z() + moved_cp_points_4.Value(3).Z() + moved_cp_points_4.Value(5).Z())/3 );

TColgp_Array1OfPnt ovl_cp_points(1,6);

ovl_cp_points.SetValue(1, ovl_pt);
ovl_cp_points.SetValue(2, ovl_pt);
ovl_cp_points.SetValue(3, ovl_pt);
ovl_cp_points.SetValue(4, ovl_pt);
ovl_cp_points.SetValue(5, ovl_pt);
ovl_cp_points.SetValue(6, ovl_pt);

// create a control point net consisting of the defined two columns:
std::vector<TColgp_Array1OfPnt> my_point_lists_rear_cap;

my_point_lists_rear_cap.push_back(moved_cp_points_4);
my_point_lists_rear_cap.push_back(ovl_cp_points);

// the point arrays should be the columns of the control point net:
TColgp_Array2OfPnt rear_srf_cap_cp_net = geoml::create_point_net_from_arrays (my_point_lists_rear_cap, 2);

// define the weight net for the fuselage rear surface:
TColStd_Array2OfReal rear_srf_cap_weight_net(1, rear_srf_cap_cp_net.ColLength(), 1, 
												rear_srf_cap_cp_net.RowLength()); 

rear_srf_cap_weight_net.SetValue(1, 1, 1.0);
rear_srf_cap_weight_net.SetValue(2, 1, 0.5);
rear_srf_cap_weight_net.SetValue(3, 1, 1.0);
rear_srf_cap_weight_net.SetValue(4, 1, 0.5);
rear_srf_cap_weight_net.SetValue(5, 1, 1.0);
rear_srf_cap_weight_net.SetValue(6, 1, 0.5);

rear_srf_cap_weight_net.SetValue(1, 2, 1.0);
rear_srf_cap_weight_net.SetValue(2, 2, 0.5);
rear_srf_cap_weight_net.SetValue(3, 2, 1.0);
rear_srf_cap_weight_net.SetValue(4, 2, 0.5);
rear_srf_cap_weight_net.SetValue(5, 2, 1.0);
rear_srf_cap_weight_net.SetValue(6, 2, 0.5);

// the knots, mults in U direction as in the rear fuselage surface:
// the V direction needs some adjustment:
Standard_Integer cap_degree_V = 1;

// set the V-knots (rows):
TColStd_Array1OfReal cap_knots_V(1,2); 

cap_knots_V.SetValue(1,0.0);           
cap_knots_V.SetValue(2,1.0);

// multiplicities V-direction: 
TColStd_Array1OfInteger cap_mults_V(1,2); 

cap_mults_V.SetValue(1,2);                
cap_mults_V.SetValue(2,2);

// define the rear surface:
Handle(Geom_BSplineSurface) rear_surface_cap
	= geoml::b_spline_surface( rear_srf_cap_cp_net, 
                 		rear_srf_cap_weight_net, 
                 		knots_U, 
                 		cap_knots_V, 
                 		mults_U, 
                 		cap_mults_V, 
                 		degree_U, 
                 		cap_degree_V, 
                 		Standard_True ); 
                                     
// write the surface rear_surface to file:
writeGeomEntityToStepFile(rear_surface_cap, "rear_surface_cap.stp");



/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Nose of fuselage
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


// start with the start point (start_point_profile) and start 
// vector of the mid-fuselage profile curve (profile_curve_cylinder):

gp_Pnt end_point_arc(2000.0, 0.0, 1000.0);
double start_param = profile_curve_cylinder->FirstParameter();
gp_Pnt start_point_arc;
gp_Vec tangent_vector;
profile_curve_cylinder->D1(start_param, start_point_arc, tangent_vector);
tangent_vector.Reverse();

GC_MakeArcOfCircle arc_maker(start_point_arc, tangent_vector, end_point_arc);

Handle(Geom_TrimmedCurve) circular_arc = arc_maker.Value();

Handle(Geom_BSplineCurve) arc_b_spline_curve = GeomConvert::CurveToBSplineCurve(circular_arc);

// now, create a surface of revolution around the profile curve arc_b_spline_curve:
Handle(Geom_BSplineSurface) nose_ogive_srf
	= geoml::revolving_surface(arc_b_spline_curve,
                  rot_axis);

nose_ogive_srf->ExchangeUV();

// write the surface rear_surface to file:
writeGeomEntityToStepFile(nose_ogive_srf, "config_fuselage_nose_ogive.stp");

// cap for nose surface:

// define a profile curve tangential to the profile curve of 
// nose_ogive_srf (arc_b_spline_curve):

// last control point of the cap-profile curve:
gp_Pnt end_point_arc_cap(1000.0, 0.0, 0.0); // original: x = 1000.0

// get the first control point of the cap-profile curve and the starting tangent vector:
double start_param_cap = arc_b_spline_curve->LastParameter();
gp_Pnt start_point_arc_cap;
gp_Vec tangent_vector_cap;
arc_b_spline_curve->D1(start_param_cap, start_point_arc_cap, tangent_vector_cap);

// second control point of cap-profile curve:
gp_Pnt cp_cap_2 = geoml::move(start_point_arc_cap, tangent_vector_cap, 0.04);

//third control point of cap-profile curve:
gp_Pnt cp_cap_3 = geoml::move(end_point_arc_cap, gp_Vec(0., 0., 1.), 550.0);

// define the cap-profile curve:
TColgp_Array1OfPnt profile_points_cap(1,4);
profile_points_cap.SetValue(1,start_point_arc_cap);
profile_points_cap.SetValue(2,cp_cap_2);
profile_points_cap.SetValue(3,cp_cap_3);
profile_points_cap.SetValue(4,end_point_arc_cap);

// degree:
Standard_Integer degree_profile_cap = 3;

// weights:
TColStd_Array1OfReal weights_profile_cap(1, 4);
weights_profile_cap.SetValue(1, 1.0);  
weights_profile_cap.SetValue(2, 1.0);
weights_profile_cap.SetValue(3, 1.0);  
weights_profile_cap.SetValue(4, 1.0);  

// knots:
TColStd_Array1OfReal knots_profile_cap(1,2); 
knots_profile_cap.SetValue(1,0.0);           
knots_profile_cap.SetValue(2,1.0);

// multiplicities: 
TColStd_Array1OfInteger mults_profile_cap(1,2); 
mults_profile_cap.SetValue(1,4);                
mults_profile_cap.SetValue(2,4);

// create the profile curve:
Handle(Geom_BSplineCurve) profile_curve_cap 
	= geoml::b_spline_curve(profile_points_cap, weights_profile_cap, 
							knots_profile_cap, mults_profile_cap, degree_profile_cap);

writeToStp(profile_curve_cap, "profile_curve_cap.stp"); 

// now, create a surface of revolution around the profile curve profile_curve_cap:
Handle(Geom_BSplineSurface) nose_ogive_cap_srf
	= geoml::revolving_surface(profile_curve_cap,
                  rot_axis);

nose_ogive_cap_srf -> ExchangeUV();

// write the surface nose_ogive_cap_srf to file:
writeGeomEntityToStepFile(nose_ogive_cap_srf, "config_fuselage_nose_ogive_capp.stp");



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////  
//////////   														
//////////	Wings
//////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////  


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Main wings
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


// start with the upper part of the wing:

// define the inner-upper profile curve:
gp_Pnt i_u_pc_1(12743.744, -1875.046, -1242.3);
//gp_Pnt i_u_pc_2(12743.761, -1875.046, -1241.294);
gp_Pnt i_u_pc_3(12744.907, -1875.046, -1192.336);
gp_Pnt i_u_pc_4(12780.961, -1875.046, -1085.249);
gp_Pnt i_u_pc_5(12922.121, -1875.046, -981.973);
gp_Pnt i_u_pc_6(13134.59, -1875.046, -914.366);
gp_Pnt i_u_pc_7(13412.355, -1875.046, -882.808);
gp_Pnt i_u_pc_8(13751.543, -1875.046, -872.338);
gp_Pnt i_u_pc_9(14145.044, -1875.046, -881.43);
gp_Pnt i_u_pc_10(14582.641, -1875.046, -908.641);
gp_Pnt i_u_pc_11(15052.873, -1875.046, -946.113);
gp_Pnt i_u_pc_12(15543.057, -1875.046, -990.982);
gp_Pnt i_u_pc_13(16039.909, -1875.046, -1040.433);
gp_Pnt i_u_pc_14(16529.885, -1875.046, -1093.018);
gp_Pnt i_u_pc_15(16999.603, -1875.046, -1148.158);
gp_Pnt i_u_pc_16(17436.043, -1875.046, -1206.562);
gp_Pnt i_u_pc_17(17827.393, -1875.046, -1266.624);
gp_Pnt i_u_pc_18(18162.459, -1875.046, -1324.558);
gp_Pnt i_u_pc_19(18433.527, -1875.046, -1376.16);
gp_Pnt i_u_pc_20(18666.664, -1875.046, -1423.234);
//gp_Pnt i_u_pc_21(18791.177, -1875.046, -1444.845);
gp_Pnt i_u_pc_22(18815.763, -1875.046, -1454.34);

Handle(TColgp_HArray1OfPnt) i_u_profile_curve_points_initial = new TColgp_HArray1OfPnt(1,20); // two removed

i_u_profile_curve_points_initial->SetValue(1, i_u_pc_1);
//i_u_profile_curve_points->SetValue(2, i_u_pc_2);
i_u_profile_curve_points_initial->SetValue(2, i_u_pc_3);
i_u_profile_curve_points_initial->SetValue(3, i_u_pc_4);
i_u_profile_curve_points_initial->SetValue(4, i_u_pc_5);
i_u_profile_curve_points_initial->SetValue(5, i_u_pc_6);
i_u_profile_curve_points_initial->SetValue(6, i_u_pc_7);
i_u_profile_curve_points_initial->SetValue(7, i_u_pc_8);
i_u_profile_curve_points_initial->SetValue(8, i_u_pc_9);
i_u_profile_curve_points_initial->SetValue(9, i_u_pc_10);
i_u_profile_curve_points_initial->SetValue(10, i_u_pc_11);
i_u_profile_curve_points_initial->SetValue(11, i_u_pc_12);
i_u_profile_curve_points_initial->SetValue(12, i_u_pc_13);
i_u_profile_curve_points_initial->SetValue(13, i_u_pc_14);
i_u_profile_curve_points_initial->SetValue(14, i_u_pc_15);
i_u_profile_curve_points_initial->SetValue(15, i_u_pc_16);
i_u_profile_curve_points_initial->SetValue(16, i_u_pc_17);
i_u_profile_curve_points_initial->SetValue(17, i_u_pc_18);
i_u_profile_curve_points_initial->SetValue(18, i_u_pc_19);
i_u_profile_curve_points_initial->SetValue(19, i_u_pc_20);
//i_u_profile_curve_points_initial->SetValue(21, i_u_pc_21);
i_u_profile_curve_points_initial->SetValue(20, i_u_pc_22);


// introduce a move in y direction parameter
// define the direction vector:
gp_Vec move_vec_i_wing (0.0, 1.0, 0.0);

// define the factor multiplied to this vector for the first move:
double move_factor_i_wing (1500.0);

// move the control points cp_column (for the periodic case):
Handle(TColgp_HArray1OfPnt) i_u_profile_curve_points
	= geoml::move(i_u_profile_curve_points_initial, move_vec_i_wing, move_factor_i_wing);

Handle(Geom_BSplineCurve) i_u_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(i_u_profile_curve_points); 

writeToStp(i_u_profile_curve, "i_u_profile_curve.stp"); 

// define the mid upper profile curve:
gp_Pnt m_u_pc_1(15081.173, -6340.33, -933.025);
//gp_Pnt m_u_pc_2(15081.183, -6340.33, -932.402);
gp_Pnt m_u_pc_3(15081.892, -6340.33, -902.118);
gp_Pnt m_u_pc_4(15104.195, -6340.33, -835.873);
gp_Pnt m_u_pc_5(15191.517, -6340.33, -771.987);
gp_Pnt m_u_pc_6(15322.95, -6340.33, -730.165);
gp_Pnt m_u_pc_7(15494.776, -6340.33, -710.643);
gp_Pnt m_u_pc_8(15704.597, -6340.33, -704.166);
gp_Pnt m_u_pc_9(15948.017, -6340.33, -709.791);
gp_Pnt m_u_pc_10(16218.715, -6340.33, -726.624);
gp_Pnt m_u_pc_11(16509.6, -6340.33, -749.804);
gp_Pnt m_u_pc_12(16812.828, -6340.33, -777.56);
gp_Pnt m_u_pc_13(17120.18, -6340.33, -808.15);
gp_Pnt m_u_pc_14(17423.28, -6340.33, -840.679);
gp_Pnt m_u_pc_15(17713.847, -6340.33, -874.789);
gp_Pnt m_u_pc_16(17983.829, -6340.33, -910.917);
gp_Pnt m_u_pc_17(18225.918, -6340.33, -948.072);
gp_Pnt m_u_pc_18(18433.19, -6340.33, -983.91);
gp_Pnt m_u_pc_19(18600.873, -6340.33, -1015.831);
gp_Pnt m_u_pc_20(18745.091, -6340.33, -1044.951);
//gp_Pnt m_u_pc_21(18822.115, -6340.33, -1058.319);
gp_Pnt m_u_pc_22(18837.323, -6340.33, -1064.193);

Handle(TColgp_HArray1OfPnt) m_u_profile_curve_points = new TColgp_HArray1OfPnt(1,20); 

m_u_profile_curve_points->SetValue(1, m_u_pc_1);
//m_u_profile_curve_points->SetValue(2, m_u_pc_2);
m_u_profile_curve_points->SetValue(2, m_u_pc_3);
m_u_profile_curve_points->SetValue(3, m_u_pc_4);
m_u_profile_curve_points->SetValue(4, m_u_pc_5);
m_u_profile_curve_points->SetValue(5, m_u_pc_6);
m_u_profile_curve_points->SetValue(6, m_u_pc_7);
m_u_profile_curve_points->SetValue(7, m_u_pc_8);
m_u_profile_curve_points->SetValue(8, m_u_pc_9);
m_u_profile_curve_points->SetValue(9, m_u_pc_10);
m_u_profile_curve_points->SetValue(10, m_u_pc_11);
m_u_profile_curve_points->SetValue(11, m_u_pc_12);
m_u_profile_curve_points->SetValue(12, m_u_pc_13);
m_u_profile_curve_points->SetValue(13, m_u_pc_14);
m_u_profile_curve_points->SetValue(14, m_u_pc_15);
m_u_profile_curve_points->SetValue(15, m_u_pc_16);
m_u_profile_curve_points->SetValue(16, m_u_pc_17);
m_u_profile_curve_points->SetValue(17, m_u_pc_18);
m_u_profile_curve_points->SetValue(18, m_u_pc_19);
m_u_profile_curve_points->SetValue(19, m_u_pc_20);
//m_u_profile_curve_points->SetValue(21, m_u_pc_21);
m_u_profile_curve_points->SetValue(20, m_u_pc_22);

Handle(Geom_BSplineCurve) m_u_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(m_u_profile_curve_points);

writeToStp(m_u_profile_curve, "m_u_profile_curve.stp");

// define the outer upper profile curve:
gp_Pnt o_u_pc_22(22136.972, -16963.479, -249.447);
//gp_Pnt o_u_pc_21(22130.919, -16963.479, -247.11);
gp_Pnt o_u_pc_20(22100.264, -16963.479, -241.789);
gp_Pnt o_u_pc_19(22042.866, -16963.479, -230.2);
gp_Pnt o_u_pc_18(21976.129, -16963.479, -217.495);
gp_Pnt o_u_pc_17(21893.636, -16963.479, -203.232);
gp_Pnt o_u_pc_16(21797.285, -16963.479, -188.444);
gp_Pnt o_u_pc_15(21689.834, -16963.479, -174.065);
gp_Pnt o_u_pc_14(21574.189, -16963.479, -160.49);
gp_Pnt o_u_pc_13(21453.557, -16963.479, -147.543);
gp_Pnt o_u_pc_12(21331.232, -16963.479, -135.369);
gp_Pnt o_u_pc_11(21210.549, -16963.479, -124.322);
gp_Pnt o_u_pc_10(21094.778, -16963.479, -115.096);
gp_Pnt o_u_pc_9(20987.041, -16963.479, -108.397);
gp_Pnt o_u_pc_8(20890.161, -16963.479, -106.158);
gp_Pnt o_u_pc_7(20806.653, -16963.479, -108.736);
gp_Pnt o_u_pc_6(20738.268, -16963.479, -116.506);
gp_Pnt o_u_pc_5(20685.958, -16963.479, -133.151);
gp_Pnt o_u_pc_4(20651.204, -16963.479, -158.577);
gp_Pnt o_u_pc_3(20642.328, -16963.479, -184.942);
//gp_Pnt o_u_pc_2(20642.045, -16963.479, -196.995);
gp_Pnt o_u_pc_1(20642.041, -16963.479, -197.243);


Handle(TColgp_HArray1OfPnt) o_u_profile_curve_points_initial = new TColgp_HArray1OfPnt(1,20); 

o_u_profile_curve_points_initial->SetValue(1, o_u_pc_1);
//o_u_profile_curve_points_initial->SetValue(2, o_u_pc_2);
o_u_profile_curve_points_initial->SetValue(2, o_u_pc_3);
o_u_profile_curve_points_initial->SetValue(3, o_u_pc_4);
o_u_profile_curve_points_initial->SetValue(4, o_u_pc_5);
o_u_profile_curve_points_initial->SetValue(5, o_u_pc_6);
o_u_profile_curve_points_initial->SetValue(6, o_u_pc_7);
o_u_profile_curve_points_initial->SetValue(7, o_u_pc_8);
o_u_profile_curve_points_initial->SetValue(8, o_u_pc_9);
o_u_profile_curve_points_initial->SetValue(9, o_u_pc_10);
o_u_profile_curve_points_initial->SetValue(10, o_u_pc_11);
o_u_profile_curve_points_initial->SetValue(11, o_u_pc_12);
o_u_profile_curve_points_initial->SetValue(12, o_u_pc_13);
o_u_profile_curve_points_initial->SetValue(13, o_u_pc_14);
o_u_profile_curve_points_initial->SetValue(14, o_u_pc_15);
o_u_profile_curve_points_initial->SetValue(15, o_u_pc_16);
o_u_profile_curve_points_initial->SetValue(16, o_u_pc_17);
o_u_profile_curve_points_initial->SetValue(17, o_u_pc_18);
o_u_profile_curve_points_initial->SetValue(18, o_u_pc_19);
o_u_profile_curve_points_initial->SetValue(19, o_u_pc_20);
//o_u_profile_curve_points_initial->SetValue(21, o_u_pc_21);
o_u_profile_curve_points_initial->SetValue(20, o_u_pc_22);

// introduce a move in y direction parameter
// define the direction vector:
gp_Vec move_vec_o_wing (0.0, 1.0, 0.0);

// define the factor multiplied to this vector for the first move:
double move_factor_o_wing (-1500.0);

// move the control points 
Handle(TColgp_HArray1OfPnt) o_u_profile_curve_points
	= geoml::move(o_u_profile_curve_points_initial, move_vec_o_wing, move_factor_o_wing);

Handle(Geom_BSplineCurve) o_u_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(o_u_profile_curve_points);

writeToStp(o_u_profile_curve, "o_u_profile_curve.stp");

// now, define the guiding curves of the wing:
// leading guide curve:
Handle(TColgp_HArray1OfPnt) leading_curve_points = new TColgp_HArray1OfPnt(1,3);
leading_curve_points->SetValue(1, i_u_profile_curve_points->Value(1));
leading_curve_points->SetValue(2, m_u_pc_1);
leading_curve_points->SetValue(3, o_u_profile_curve_points->Value(1));

Handle(Geom_BSplineCurve) leading_curve
	= geoml::interpolate_points_to_b_spline_curve(leading_curve_points);

writeToStp(leading_curve, "leading_curve.stp");

// trailing guide curve:
Handle(TColgp_HArray1OfPnt) trailing_curve_points = new TColgp_HArray1OfPnt(1,3);
trailing_curve_points->SetValue(1, i_u_profile_curve_points->Value(20));
trailing_curve_points->SetValue(2, m_u_pc_22);
trailing_curve_points->SetValue(3, o_u_profile_curve_points->Value(20));

Handle(Geom_BSplineCurve) trailing_curve
	= geoml::interpolate_points_to_b_spline_curve(trailing_curve_points);

writeToStp(trailing_curve, "trailing_curve.stp");

// now, define the upper wing's surface:
std::vector<Handle(Geom_Curve)> list_of_profiles_upper_wing;
list_of_profiles_upper_wing.push_back(i_u_profile_curve);
list_of_profiles_upper_wing.push_back(m_u_profile_curve);
list_of_profiles_upper_wing.push_back(o_u_profile_curve);

std::vector<Handle(Geom_Curve)> list_of_guides_upper_wing;
list_of_guides_upper_wing.push_back(leading_curve);
list_of_guides_upper_wing.push_back(trailing_curve);

Handle(Geom_BSplineSurface) upper_wing_surface
	= geoml::interpolate_curve_network(list_of_profiles_upper_wing,
                          list_of_guides_upper_wing,
                          1.0);

upper_wing_surface -> ExchangeUV();

// write the surface upper_wing_surface to file:
writeGeomEntityToStepFile(upper_wing_surface, "upper_wing_surface.stp");

// lower part of the wing:
// define the inner-lower profile curve:					
gp_Pnt i_l_pc_21 (18815.763, -1875.046, -1454.34);
gp_Pnt i_l_pc_20 (18790.359, -1875.046, -1455.64);
gp_Pnt i_l_pc_19 (18665.042, -1875.046, -1461.521);
gp_Pnt i_l_pc_18 (18429.553, -1875.046, -1477.358);
gp_Pnt i_l_pc_17 (18155.329, -1875.046, -1503.77);
gp_Pnt i_l_pc_16 (17816.545, -1875.046, -1547.528);
gp_Pnt i_l_pc_15 (17421.421, -1875.046, -1611.353);
gp_Pnt i_l_pc_14 (16981.161, -1875.046, -1683.479);
gp_Pnt i_l_pc_13 (16507.719, -1875.046, -1754.291);
gp_Pnt i_l_pc_12 (16014.058, -1875.046, -1809.728);
gp_Pnt i_l_pc_11 (15513.978, -1875.046, -1841.509);
gp_Pnt i_l_pc_10 (15021.479, -1875.046, -1849.277);
gp_Pnt i_l_pc_9 (14549.965, -1875.046, -1835.59);
gp_Pnt i_l_pc_8 (14112.211, -1875.046, -1801.944);
gp_Pnt i_l_pc_7 (13719.716, -1875.046, -1750.54);
gp_Pnt i_l_pc_6 (13383.024, -1875.046, -1682.695);
gp_Pnt i_l_pc_5 (13108.738, -1875.046, -1602.313);
gp_Pnt i_l_pc_4 (12909.179, -1875.046, -1489.265);
gp_Pnt i_l_pc_3 (12772.556, -1875.046, -1381.774);
gp_Pnt i_l_pc_2 (12742.968, -1875.046, -1289.064);
gp_Pnt i_l_pc_1 (12743.744, -1875.046, -1242.3); 

Handle(TColgp_HArray1OfPnt) i_l_profile_curve_points_initial = new TColgp_HArray1OfPnt(1,21); 

i_l_profile_curve_points_initial->SetValue(1,  i_l_pc_1);
i_l_profile_curve_points_initial->SetValue(2,  i_l_pc_2);
i_l_profile_curve_points_initial->SetValue(3,  i_l_pc_3);
i_l_profile_curve_points_initial->SetValue(4,  i_l_pc_4);
i_l_profile_curve_points_initial->SetValue(5,  i_l_pc_5);
i_l_profile_curve_points_initial->SetValue(6,  i_l_pc_6);
i_l_profile_curve_points_initial->SetValue(7,  i_l_pc_7);
i_l_profile_curve_points_initial->SetValue(8,  i_l_pc_8);
i_l_profile_curve_points_initial->SetValue(9,  i_l_pc_9);
i_l_profile_curve_points_initial->SetValue(10,  i_l_pc_10);
i_l_profile_curve_points_initial->SetValue(11, i_l_pc_11);
i_l_profile_curve_points_initial->SetValue(12, i_l_pc_12);
i_l_profile_curve_points_initial->SetValue(13, i_l_pc_13);
i_l_profile_curve_points_initial->SetValue(14, i_l_pc_14);
i_l_profile_curve_points_initial->SetValue(15, i_l_pc_15);
i_l_profile_curve_points_initial->SetValue(16, i_l_pc_16);
i_l_profile_curve_points_initial->SetValue(17, i_l_pc_17);
i_l_profile_curve_points_initial->SetValue(18, i_l_pc_18);
i_l_profile_curve_points_initial->SetValue(19, i_l_pc_19);
i_l_profile_curve_points_initial->SetValue(20, i_l_pc_20);
i_l_profile_curve_points_initial->SetValue(21, i_l_pc_21);

// move the control points in y direction:
Handle(TColgp_HArray1OfPnt) i_l_profile_curve_points
	= geoml::move(i_l_profile_curve_points_initial, move_vec_i_wing, move_factor_i_wing);

Handle(Geom_BSplineCurve) i_l_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(i_l_profile_curve_points); 
 
writeToStp(i_l_profile_curve, "i_l_profile_curve.stp"); 

// define the mid-lower profile curve:					
gp_Pnt m_l_pc_1  (15081.173, -6340.33, -933.025);
gp_Pnt m_l_pc_2  (15080.693, -6340.33, -961.953);
gp_Pnt m_l_pc_3  (15098.996, -6340.33, -1019.304);
gp_Pnt m_l_pc_4  (15183.511, -6340.33, -1085.797);
gp_Pnt m_l_pc_5  (15306.958, -6340.33, -1155.729);
gp_Pnt m_l_pc_6  (15476.631, -6340.33, -1205.453);
gp_Pnt m_l_pc_7  (15684.909, -6340.33, -1247.422);
gp_Pnt m_l_pc_8  (15927.706, -6340.33, -1279.221);
gp_Pnt m_l_pc_9  (16198.501, -6340.33, -1300.034);
gp_Pnt m_l_pc_10 (16490.18,  -6340.33, -1308.501);
gp_Pnt m_l_pc_11 (16794.84,  -6340.33, -1303.695);
gp_Pnt m_l_pc_12 (17104.189, -6340.33, -1284.036);
gp_Pnt m_l_pc_13 (17409.568, -6340.33, -1249.742);
gp_Pnt m_l_pc_14 (17702.439, -6340.33, -1205.938);
gp_Pnt m_l_pc_15 (17974.784, -6340.33, -1161.321);
gp_Pnt m_l_pc_16 (18219.207, -6340.33, -1121.839);
gp_Pnt m_l_pc_17 (18428.779, -6340.33, -1094.77);
gp_Pnt m_l_pc_18 (18598.415, -6340.33, -1078.432);
gp_Pnt m_l_pc_19 (18744.088, -6340.33, -1068.635);
gp_Pnt m_l_pc_20 (18821.609, -6340.33, -1064.997);
gp_Pnt m_l_pc_21 (18837.323, -6340.33, -1064.193);

Handle(TColgp_HArray1OfPnt) m_l_profile_curve_points = new TColgp_HArray1OfPnt(1,21);

m_l_profile_curve_points->SetValue(1,  m_l_pc_1);
m_l_profile_curve_points->SetValue(2,  m_l_pc_2);
m_l_profile_curve_points->SetValue(3,  m_l_pc_3);
m_l_profile_curve_points->SetValue(4,  m_l_pc_4);
m_l_profile_curve_points->SetValue(5,  m_l_pc_5);
m_l_profile_curve_points->SetValue(6,  m_l_pc_6);
m_l_profile_curve_points->SetValue(7,  m_l_pc_7);
m_l_profile_curve_points->SetValue(8,  m_l_pc_8);
m_l_profile_curve_points->SetValue(9,  m_l_pc_9);
m_l_profile_curve_points->SetValue(10, m_l_pc_10);
m_l_profile_curve_points->SetValue(11, m_l_pc_11);
m_l_profile_curve_points->SetValue(12, m_l_pc_12);
m_l_profile_curve_points->SetValue(13, m_l_pc_13);
m_l_profile_curve_points->SetValue(14, m_l_pc_14);
m_l_profile_curve_points->SetValue(15, m_l_pc_15);
m_l_profile_curve_points->SetValue(16, m_l_pc_16);
m_l_profile_curve_points->SetValue(17, m_l_pc_17);
m_l_profile_curve_points->SetValue(18, m_l_pc_18);
m_l_profile_curve_points->SetValue(19, m_l_pc_19);
m_l_profile_curve_points->SetValue(20, m_l_pc_20);
m_l_profile_curve_points->SetValue(21, m_l_pc_21);

Handle(Geom_BSplineCurve) m_l_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(m_l_profile_curve_points);

writeToStp(m_l_profile_curve, "m_l_profile_curve.stp"); 

// define the outer-lower profile curve:					
gp_Pnt o_l_pc_1  (20642.041, -16963.479, -197.243);
gp_Pnt o_l_pc_2  (20641.85,  -16963.479, -208.756);
gp_Pnt o_l_pc_3  (20649.135, -16963.479, -231.582);
gp_Pnt o_l_pc_4  (20682.771, -16963.479, -258.046);
gp_Pnt o_l_pc_5  (20731.903, -16963.479, -285.878);
gp_Pnt o_l_pc_6  (20799.432, -16963.479, -305.668);
gp_Pnt o_l_pc_7  (20882.326, -16963.479, -322.372);
gp_Pnt o_l_pc_8  (20978.958, -16963.479, -335.028);
gp_Pnt o_l_pc_9  (21086.733, -16963.479, -343.311);
gp_Pnt o_l_pc_10 (21202.82,  -16963.479, -346.681);
gp_Pnt o_l_pc_11 (21324.073, -16963.479, -344.768);
gp_Pnt o_l_pc_12 (21447.193, -16963.479, -336.944);
gp_Pnt o_l_pc_13 (21568.732, -16963.479, -323.295);
gp_Pnt o_l_pc_14 (21685.293, -16963.479, -305.861);
gp_Pnt o_l_pc_15 (21793.686, -16963.479, -288.104);
gp_Pnt o_l_pc_16 (21890.965, -16963.479, -272.39);
gp_Pnt o_l_pc_17 (21974.373, -16963.479, -261.617);
gp_Pnt o_l_pc_18 (22041.888, -16963.479, -255.114);
gp_Pnt o_l_pc_19 (22099.865, -16963.479, -251.215);
gp_Pnt o_l_pc_20 (22130.718, -16963.479, -249.767);
gp_Pnt o_l_pc_21 (22136.972, -16963.479, -249.447);


Handle(TColgp_HArray1OfPnt) o_l_profile_curve_points_initial = new TColgp_HArray1OfPnt(1,21);

o_l_profile_curve_points_initial->SetValue(1,  o_l_pc_1);
o_l_profile_curve_points_initial->SetValue(2,  o_l_pc_2);
o_l_profile_curve_points_initial->SetValue(3,  o_l_pc_3);
o_l_profile_curve_points_initial->SetValue(4,  o_l_pc_4);
o_l_profile_curve_points_initial->SetValue(5,  o_l_pc_5);
o_l_profile_curve_points_initial->SetValue(6,  o_l_pc_6);
o_l_profile_curve_points_initial->SetValue(7,  o_l_pc_7);
o_l_profile_curve_points_initial->SetValue(8,  o_l_pc_8);
o_l_profile_curve_points_initial->SetValue(9,  o_l_pc_9);
o_l_profile_curve_points_initial->SetValue(10, o_l_pc_10);
o_l_profile_curve_points_initial->SetValue(11, o_l_pc_11);
o_l_profile_curve_points_initial->SetValue(12, o_l_pc_12);
o_l_profile_curve_points_initial->SetValue(13, o_l_pc_13);
o_l_profile_curve_points_initial->SetValue(14, o_l_pc_14);
o_l_profile_curve_points_initial->SetValue(15, o_l_pc_15);
o_l_profile_curve_points_initial->SetValue(16, o_l_pc_16);
o_l_profile_curve_points_initial->SetValue(17, o_l_pc_17);
o_l_profile_curve_points_initial->SetValue(18, o_l_pc_18);
o_l_profile_curve_points_initial->SetValue(19, o_l_pc_19);
o_l_profile_curve_points_initial->SetValue(20, o_l_pc_20);
o_l_profile_curve_points_initial->SetValue(21, o_l_pc_21);

// introduce a move in y direction parameter
// define the direction vector:

// move the control points 
Handle(TColgp_HArray1OfPnt) o_l_profile_curve_points
	= geoml::move(o_l_profile_curve_points_initial, move_vec_o_wing, move_factor_o_wing);

Handle(Geom_BSplineCurve) o_l_profile_curve
	= geoml::interpolate_points_to_b_spline_curve(o_l_profile_curve_points);

writeToStp(o_l_profile_curve, "o_l_profile_curve.stp"); 

// now, define the lower wing's surface:
std::vector<Handle(Geom_Curve)> list_of_profiles_lower_wing;
list_of_profiles_lower_wing.push_back(i_l_profile_curve);
list_of_profiles_lower_wing.push_back(m_l_profile_curve);
list_of_profiles_lower_wing.push_back(o_l_profile_curve);

Handle(Geom_BSplineSurface) lower_wing_surface
	= geoml::interpolate_curve_network(list_of_profiles_lower_wing,
                          list_of_guides_upper_wing,
                          1.0);

// write the surface lower_wing_surface to file:
writeGeomEntityToStepFile(lower_wing_surface, "lower_wing_surface.stp");

// now, define the inner cap of the wing:
// the edges are given by
std::vector<Handle(Geom_Curve)> edges_of_inner_wing_cap;
edges_of_inner_wing_cap.push_back(i_u_profile_curve);
edges_of_inner_wing_cap.push_back(i_l_profile_curve);

// define the surface:
Handle(Geom_BSplineSurface) inner_wing_cap_surface
	= geoml::interpolate_curves(edges_of_inner_wing_cap);

// write the surface inner_wing_cap_surface to file:
writeGeomEntityToStepFile(inner_wing_cap_surface, "inner_wing_cap_surface.stp");

// now, define the outer cap of the wing:
// the edges are given by
std::vector<Handle(Geom_Curve)> edges_of_outer_wing_cap;
edges_of_outer_wing_cap.push_back(o_u_profile_curve);
edges_of_outer_wing_cap.push_back(o_l_profile_curve);

// define the surface:
Handle(Geom_BSplineSurface) outer_wing_cap_surface
	= geoml::interpolate_curves(edges_of_outer_wing_cap);

outer_wing_cap_surface -> ExchangeUV();

// write the surface outer_wing_cap_surface to file:
writeGeomEntityToStepFile(outer_wing_cap_surface, "outer_wing_cap_surface.stp");


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Hoehenleitwerk (wings)
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

//upper surface hl:
// define the outer-upper profile curve:					
gp_Pnt o_u_hl_19   {36653.367, -6224.95, 1241.75};
gp_Pnt o_u_hl_18   {36602.209, -6224.95, 1249.317};
gp_Pnt o_u_hl_17   {36557.289, -6224.95, 1253.952};
gp_Pnt o_u_hl_16   {36398.928, -6224.95, 1269.749};
gp_Pnt o_u_hl_15   {36283.59,  -6224.95, 1279.586};
gp_Pnt o_u_hl_14   {36159.185, -6224.95, 1289.057};
gp_Pnt o_u_hl_13   {36034.652, -6224.95, 1296.61};
gp_Pnt o_u_hl_12  {35919.658, -6224.95, 1301.905};
gp_Pnt o_u_hl_11   {35792.998, -6224.95, 1303.924};
gp_Pnt o_u_hl_10  {35731.114, -6224.95, 1303.27};
gp_Pnt o_u_hl_9  {35663.957, -6224.95, 1301.084};
gp_Pnt o_u_hl_8  {35606.834, -6224.95, 1297.62};
gp_Pnt o_u_hl_7  {35543.619, -6224.95, 1290.705};
gp_Pnt o_u_hl_6  {35512.122, -6224.95, 1285.431};
gp_Pnt o_u_hl_5  {35481.042, -6224.95, 1279.475};
gp_Pnt o_u_hl_4  {35448.39,  -6224.95, 1268.709};
gp_Pnt o_u_hl_3  {35434.163, -6224.95, 1263.497};
gp_Pnt o_u_hl_2  {35417.737, -6224.95, 1248.881};
gp_Pnt o_u_hl_1  {35417.737, -6224.95, 1241.75}; 

Handle(TColgp_HArray1OfPnt) o_u_profile_hl_curve_points = new TColgp_HArray1OfPnt(1,19);

o_u_profile_hl_curve_points->SetValue(1,  o_u_hl_1);
o_u_profile_hl_curve_points->SetValue(2,  o_u_hl_2);
o_u_profile_hl_curve_points->SetValue(3,  o_u_hl_3);
o_u_profile_hl_curve_points->SetValue(4,  o_u_hl_4);
o_u_profile_hl_curve_points->SetValue(5,  o_u_hl_5);
o_u_profile_hl_curve_points->SetValue(6,  o_u_hl_6);
o_u_profile_hl_curve_points->SetValue(7,  o_u_hl_7);
o_u_profile_hl_curve_points->SetValue(8,  o_u_hl_8);
o_u_profile_hl_curve_points->SetValue(9,  o_u_hl_9);
o_u_profile_hl_curve_points->SetValue(10, o_u_hl_10);
o_u_profile_hl_curve_points->SetValue(11, o_u_hl_11);
o_u_profile_hl_curve_points->SetValue(12, o_u_hl_12);
o_u_profile_hl_curve_points->SetValue(13, o_u_hl_13);
o_u_profile_hl_curve_points->SetValue(14, o_u_hl_14);
o_u_profile_hl_curve_points->SetValue(15, o_u_hl_15);
o_u_profile_hl_curve_points->SetValue(16, o_u_hl_16);
o_u_profile_hl_curve_points->SetValue(17, o_u_hl_17);
o_u_profile_hl_curve_points->SetValue(18, o_u_hl_18);
o_u_profile_hl_curve_points->SetValue(19, o_u_hl_19);

Handle(Geom_BSplineCurve) o_u_profile_hl_curve
	= geoml::interpolate_points_to_b_spline_curve(o_u_profile_hl_curve_points);

writeToStp(o_u_profile_hl_curve, "o_u_profile_hl_curve.stp"); 

// define the inner-upper profile curve:					
gp_Pnt i_u_hl_1 {31465.669, 0, 697.137};
gp_Pnt i_u_hl_2	{31465.669, 0, 718.746};
gp_Pnt i_u_hl_3	{31515.445, 0, 763.038};
gp_Pnt i_u_hl_4	{31558.556, 0, 778.833};
gp_Pnt i_u_hl_5	{31657.503, 0, 811.456};
gp_Pnt i_u_hl_6	{31751.683, 0, 829.505};
gp_Pnt i_u_hl_7	{31847.13,  0, 845.485};
gp_Pnt i_u_hl_8	{32038.689, 0, 866.442};
gp_Pnt i_u_hl_9	{32211.789, 0, 876.937};
gp_Pnt i_u_hl_10 {32415.295, 0, 883.563};
gp_Pnt i_u_hl_11 {32602.823, 0, 885.543};
gp_Pnt i_u_hl_12 {32986.641, 0, 879.426};
gp_Pnt i_u_hl_13 {33335.107, 0, 863.38};
gp_Pnt i_u_hl_14 {33712.482, 0, 840.493};
gp_Pnt i_u_hl_15 {34089.465, 0, 811.792};
gp_Pnt i_u_hl_16 {34438.975, 0, 781.983};
gp_Pnt i_u_hl_17 {34918.855, 0, 734.114};
gp_Pnt i_u_hl_18 {35054.976, 0, 720.067};
gp_Pnt i_u_hl_19 {35210.0,   0, 697.137};

Handle(TColgp_HArray1OfPnt) i_u_profile_hl_curve_points = new TColgp_HArray1OfPnt(1,19);

i_u_profile_hl_curve_points->SetValue(1,  i_u_hl_1);
i_u_profile_hl_curve_points->SetValue(2,  i_u_hl_2);
i_u_profile_hl_curve_points->SetValue(3,  i_u_hl_3);
i_u_profile_hl_curve_points->SetValue(4,  i_u_hl_4);
i_u_profile_hl_curve_points->SetValue(5,  i_u_hl_5);
i_u_profile_hl_curve_points->SetValue(6,  i_u_hl_6);
i_u_profile_hl_curve_points->SetValue(7,  i_u_hl_7);
i_u_profile_hl_curve_points->SetValue(8,  i_u_hl_8);
i_u_profile_hl_curve_points->SetValue(9,  i_u_hl_9);
i_u_profile_hl_curve_points->SetValue(10, i_u_hl_10);
i_u_profile_hl_curve_points->SetValue(11, i_u_hl_11);
i_u_profile_hl_curve_points->SetValue(12, i_u_hl_12);
i_u_profile_hl_curve_points->SetValue(13, i_u_hl_13);
i_u_profile_hl_curve_points->SetValue(14, i_u_hl_14);
i_u_profile_hl_curve_points->SetValue(15, i_u_hl_15);
i_u_profile_hl_curve_points->SetValue(16, i_u_hl_16);
i_u_profile_hl_curve_points->SetValue(17, i_u_hl_17);
i_u_profile_hl_curve_points->SetValue(18, i_u_hl_18);
i_u_profile_hl_curve_points->SetValue(19, i_u_hl_19);

Handle(Geom_BSplineCurve) i_u_profile_hl_curve
	= geoml::interpolate_points_to_b_spline_curve(i_u_profile_hl_curve_points);

writeToStp(i_u_profile_hl_curve, "i_u_profile_hl_curve.stp"); 

// create the upper surface of the hoehenleitwerk:
std::vector<Handle(Geom_Curve)> upper_profiles_hl_curves;
upper_profiles_hl_curves.push_back(o_u_profile_hl_curve);
upper_profiles_hl_curves.push_back(i_u_profile_hl_curve);

Handle(Geom_BSplineSurface) upper_hl_surface
	= geoml::interpolate_curves(upper_profiles_hl_curves);

// write upper_hl_surface to file:
writeGeomEntityToStepFile(upper_hl_surface, "upper_hl_surface.stp");

// lower surface hl:
// define the outer-lower profile curve:					
gp_Pnt o_l_hl_1  {35417.737, -6224.95, 1241.75};
gp_Pnt o_l_hl_2	 {35417.737, -6224.95, 1234.619};
gp_Pnt o_l_hl_3	 {35434.163, -6224.95, 1220.003};
gp_Pnt o_l_hl_4	 {35448.39,  -6224.95, 1214.79};
gp_Pnt o_l_hl_5	 {35481.042, -6224.95, 1204.025};
gp_Pnt o_l_hl_6	 {35512.122, -6224.95, 1198.069};
gp_Pnt o_l_hl_7	 {35543.619, -6224.95, 1192.795};
gp_Pnt o_l_hl_8	 {35606.834, -6224.95, 1185.879};
gp_Pnt o_l_hl_9	 {35663.957, -6224.95, 1182.416};
gp_Pnt o_l_hl_10 {35731.114, -6224.95, 1180.229};
gp_Pnt o_l_hl_11 {35792.998, -6224.95, 1179.576};
gp_Pnt o_l_hl_12 {35919.658, -6224.95, 1181.595};
gp_Pnt o_l_hl_13 {36034.652, -6224.95, 1186.89};
gp_Pnt o_l_hl_14 {36159.185, -6224.95, 1194.442};
gp_Pnt o_l_hl_15 {36283.59,  -6224.95, 1203.914};
gp_Pnt o_l_hl_16 {36398.928, -6224.95, 1213.751};
gp_Pnt o_l_hl_17 {36557.289, -6224.95, 1229.548};
gp_Pnt o_l_hl_18 {36602.209, -6224.95, 1234.183};
gp_Pnt o_l_hl_19 {36653.367, -6224.95, 1241.75};
   
Handle(TColgp_HArray1OfPnt) o_l_profile_hl_lower_curve_points = new TColgp_HArray1OfPnt(1,19);

o_l_profile_hl_lower_curve_points->SetValue(1,  o_l_hl_1);
o_l_profile_hl_lower_curve_points->SetValue(2,  o_l_hl_2);
o_l_profile_hl_lower_curve_points->SetValue(3,  o_l_hl_3);
o_l_profile_hl_lower_curve_points->SetValue(4,  o_l_hl_4);
o_l_profile_hl_lower_curve_points->SetValue(5,  o_l_hl_5);
o_l_profile_hl_lower_curve_points->SetValue(6,  o_l_hl_6);
o_l_profile_hl_lower_curve_points->SetValue(7,  o_l_hl_7);
o_l_profile_hl_lower_curve_points->SetValue(8,  o_l_hl_8);
o_l_profile_hl_lower_curve_points->SetValue(9,  o_l_hl_9);
o_l_profile_hl_lower_curve_points->SetValue(10, o_l_hl_10);
o_l_profile_hl_lower_curve_points->SetValue(11, o_l_hl_11);
o_l_profile_hl_lower_curve_points->SetValue(12, o_l_hl_12);
o_l_profile_hl_lower_curve_points->SetValue(13, o_l_hl_13);
o_l_profile_hl_lower_curve_points->SetValue(14, o_l_hl_14);
o_l_profile_hl_lower_curve_points->SetValue(15, o_l_hl_15);
o_l_profile_hl_lower_curve_points->SetValue(16, o_l_hl_16);
o_l_profile_hl_lower_curve_points->SetValue(17, o_l_hl_17);
o_l_profile_hl_lower_curve_points->SetValue(18, o_l_hl_18);
o_l_profile_hl_lower_curve_points->SetValue(19, o_l_hl_19);

Handle(Geom_BSplineCurve) o_l_profile_hl_curve
	= geoml::interpolate_points_to_b_spline_curve(o_l_profile_hl_lower_curve_points);

writeToStp(o_l_profile_hl_curve, "o_l_profile_hl_curve.stp"); 

// define the inner-lower profile curve:					
gp_Pnt i_l_hl_19  {35210.0,   0, 697.137};
gp_Pnt i_l_hl_18  {35054.976, 0, 674.208};
gp_Pnt i_l_hl_17  {34918.855, 0, 660.161};
gp_Pnt i_l_hl_16  {34438.975, 0, 612.292};
gp_Pnt i_l_hl_15  {34089.465, 0, 582.483};
gp_Pnt i_l_hl_14  {33712.482, 0, 553.782};
gp_Pnt i_l_hl_13  {33335.107, 0, 530.895};
gp_Pnt i_l_hl_12  {32986.641, 0, 514.848};
gp_Pnt i_l_hl_11  {32602.823, 0, 508.732};
gp_Pnt i_l_hl_10  {32415.295, 0, 510.711};
gp_Pnt i_l_hl_9   {32211.789, 0, 517.338};
gp_Pnt i_l_hl_8   {32038.689, 0, 527.833};
gp_Pnt i_l_hl_7   {31847.13,  0, 548.789};
gp_Pnt i_l_hl_6   {31751.683, 0, 564.77};
gp_Pnt i_l_hl_5   {31657.503, 0, 582.819};
gp_Pnt i_l_hl_4   {31558.556, 0, 615.442};
gp_Pnt i_l_hl_3   {31515.445, 0, 631.237};
gp_Pnt i_l_hl_2   {31465.669, 0, 675.529};
gp_Pnt i_l_hl_1   {31465.669, 0, 697.137};
  
Handle(TColgp_HArray1OfPnt) i_l_profile_hl_lower_curve_points = new TColgp_HArray1OfPnt(1,19);

i_l_profile_hl_lower_curve_points->SetValue(1,  i_l_hl_1);
i_l_profile_hl_lower_curve_points->SetValue(2,  i_l_hl_2);
i_l_profile_hl_lower_curve_points->SetValue(3,  i_l_hl_3);
i_l_profile_hl_lower_curve_points->SetValue(4,  i_l_hl_4);
i_l_profile_hl_lower_curve_points->SetValue(5,  i_l_hl_5);
i_l_profile_hl_lower_curve_points->SetValue(6,  i_l_hl_6);
i_l_profile_hl_lower_curve_points->SetValue(7,  i_l_hl_7);
i_l_profile_hl_lower_curve_points->SetValue(8,  i_l_hl_8);
i_l_profile_hl_lower_curve_points->SetValue(9,  i_l_hl_9);
i_l_profile_hl_lower_curve_points->SetValue(10, i_l_hl_10);
i_l_profile_hl_lower_curve_points->SetValue(11, i_l_hl_11);
i_l_profile_hl_lower_curve_points->SetValue(12, i_l_hl_12);
i_l_profile_hl_lower_curve_points->SetValue(13, i_l_hl_13);
i_l_profile_hl_lower_curve_points->SetValue(14, i_l_hl_14);
i_l_profile_hl_lower_curve_points->SetValue(15, i_l_hl_15);
i_l_profile_hl_lower_curve_points->SetValue(16, i_l_hl_16);
i_l_profile_hl_lower_curve_points->SetValue(17, i_l_hl_17);
i_l_profile_hl_lower_curve_points->SetValue(18, i_l_hl_18);
i_l_profile_hl_lower_curve_points->SetValue(19, i_l_hl_19);

Handle(Geom_BSplineCurve) i_l_profile_hl_curve
	= geoml::interpolate_points_to_b_spline_curve(i_l_profile_hl_lower_curve_points);

writeToStp(i_l_profile_hl_curve, "i_l_profile_hl_curve.stp"); 

// create the lower surface of the hoehenleitwerk:
std::vector<Handle(Geom_Curve)> lower_profiles_hl_curves;
lower_profiles_hl_curves.push_back(o_l_profile_hl_curve);
lower_profiles_hl_curves.push_back(i_l_profile_hl_curve);

Handle(Geom_BSplineSurface) lower_hl_surface
	= geoml::interpolate_curves(lower_profiles_hl_curves);

// write upper_hl_surface to file:
writeGeomEntityToStepFile(lower_hl_surface, "lower_hl_surface.stp");


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////	Seitenleitwerk (wings)
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


// half surface sl
// define the lower profile curve:				
gp_Pnt l_sl_1 	{29942.279 ,  0.0     , 1957.613};
gp_Pnt l_sl_2	{29942.279 , -37.007  , 1957.613};
gp_Pnt l_sl_3	{30005.646 , -108.014 , 1957.613};
gp_Pnt l_sl_4	{30073.869 , -140.75  , 1957.613};
gp_Pnt l_sl_5	{30145.449 , -171.466 , 1957.613};
gp_Pnt l_sl_6	{30213.493 , -193.562 , 1957.613};
gp_Pnt l_sl_7	{30281.679 , -212.502 , 1957.613};
gp_Pnt l_sl_8	{30349.685 , -228.558 , 1957.613};
gp_Pnt l_sl_9	{30417.68  , -242.518 , 1957.613};
gp_Pnt l_sl_10	{30485.648 , -254.723 , 1957.613};
gp_Pnt l_sl_11	{30553.607 , -265.458 , 1957.613};
gp_Pnt l_sl_12	{30621.558 , -274.925 , 1957.613};
gp_Pnt l_sl_13	{30689.505 , -283.278 , 1957.613};
gp_Pnt l_sl_14	{30757.449 , -290.639 , 1957.613};
gp_Pnt l_sl_15	{30825.391 , -297.109 , 1957.613};
gp_Pnt l_sl_16	{30893.331 , -302.769 , 1957.613};
gp_Pnt l_sl_17	{30961.27  , -307.688 , 1957.613};
gp_Pnt l_sl_18	{31029.208 , -311.924 , 1957.613};
gp_Pnt l_sl_19	{31097.145 , -315.528 , 1957.613};
gp_Pnt l_sl_20	{31165.082 , -318.546 , 1957.613};
gp_Pnt l_sl_21	{31233.018 , -321.015 , 1957.613};
gp_Pnt l_sl_22	{31300.954 , -322.97  , 1957.613};
gp_Pnt l_sl_23	{31368.889 , -324.444 , 1957.613};
gp_Pnt l_sl_24	{31436.825 , -325.463 , 1957.613};
gp_Pnt l_sl_25	{31504.76  , -326.055 , 1957.613};
gp_Pnt l_sl_26	{31572.695 , -326.241 , 1957.613};
gp_Pnt l_sl_27	{31640.63  , -326.044 , 1957.613};
gp_Pnt l_sl_28	{31708.565 , -325.483 , 1957.613};
gp_Pnt l_sl_29	{31776.499 , -324.577 , 1957.613};
gp_Pnt l_sl_30	{31844.434 , -323.343 , 1957.613};
gp_Pnt l_sl_31	{31912.368 , -321.796 , 1957.613};
gp_Pnt l_sl_32	{31980.303 , -319.951 , 1957.613};
gp_Pnt l_sl_33	{32048.237 , -317.821 , 1957.613};
gp_Pnt l_sl_34	{32116.171 , -315.421 , 1957.613};
gp_Pnt l_sl_35	{32184.105 , -312.76  , 1957.613};
gp_Pnt l_sl_36	{32252.04  , -309.852 , 1957.613};
gp_Pnt l_sl_37	{32319.974 , -306.707 , 1957.613};
gp_Pnt l_sl_38	{32387.908 , -303.334 , 1957.613};
gp_Pnt l_sl_39	{32455.842 , -299.742 , 1957.613};
gp_Pnt l_sl_40	{32523.776 , -295.942 , 1957.613};
gp_Pnt l_sl_41	{32591.71  , -291.941 , 1957.613};
gp_Pnt l_sl_42	{32659.644 , -287.746 , 1957.613};
gp_Pnt l_sl_43	{32727.578 , -283.366 , 1957.613};
gp_Pnt l_sl_44	{32795.512 , -278.807 , 1957.613};
gp_Pnt l_sl_45	{32863.446 , -274.075 , 1957.613};
gp_Pnt l_sl_46	{32931.38  , -269.177 , 1957.613};
gp_Pnt l_sl_47	{32999.314 , -264.118 , 1957.613};
gp_Pnt l_sl_48	{33067.248 , -258.904 , 1957.613};
gp_Pnt l_sl_49	{33135.182 , -253.54  , 1957.613};
gp_Pnt l_sl_50	{33203.116 , -248.03  , 1957.613};
gp_Pnt l_sl_51	{33271.05  , -242.378 , 1957.613};
gp_Pnt l_sl_52	{33338.984 , -236.588 , 1957.613};
gp_Pnt l_sl_53	{33406.918 , -230.665 , 1957.613};
gp_Pnt l_sl_54	{33474.852 , -224.611 , 1957.613};
gp_Pnt l_sl_55	{33542.786 , -218.429 , 1957.613};
gp_Pnt l_sl_56	{33610.72  , -212.122 , 1957.613};
gp_Pnt l_sl_57	{33678.654 , -205.692 , 1957.613};
gp_Pnt l_sl_58	{33746.588 , -199.142 , 1957.613};
gp_Pnt l_sl_59	{33814.522 , -192.474 , 1957.613};
gp_Pnt l_sl_60	{33882.456 , -185.689 , 1957.613};
gp_Pnt l_sl_61	{33950.39  , -178.788 , 1957.613};
gp_Pnt l_sl_62	{34018.325 , -171.772 , 1957.613};
gp_Pnt l_sl_63	{34086.259 , -164.643 , 1957.613};
gp_Pnt l_sl_64	{34154.193 , -157.401 , 1957.613};
gp_Pnt l_sl_65	{34222.127 , -150.045 , 1957.613};
gp_Pnt l_sl_66	{34290.061 , -142.577 , 1957.613};
gp_Pnt l_sl_67	{34357.995 , -134.996 , 1957.613};
gp_Pnt l_sl_68	{34425.929 , -127.302 , 1957.613};
gp_Pnt l_sl_69	{34493.863 , -119.493 , 1957.613};
gp_Pnt l_sl_70	{34561.797 , -111.57  , 1957.613};
gp_Pnt l_sl_71	{34629.732 , -103.53  , 1957.613};
gp_Pnt l_sl_72	{34697.666 , -95.373  , 1957.613};
gp_Pnt l_sl_73	{34765.6   , -87.096  , 1957.613};
gp_Pnt l_sl_74	{34833.534 , -78.699  , 1957.613};
gp_Pnt l_sl_75	{34901.468 , -70.179  , 1957.613};
gp_Pnt l_sl_76	{34969.403 , -61.533  , 1957.613};
gp_Pnt l_sl_77	{35037.337 , -52.76   , 1957.613};
gp_Pnt l_sl_78	{35105.271 , -43.856  , 1957.613};
gp_Pnt l_sl_79	{35173.205 , -34.818  , 1957.613};
gp_Pnt l_sl_80	{35263.79  , -22.585  , 1957.613};
gp_Pnt l_sl_81	{35331.718 , -13.201  , 1957.613};
gp_Pnt l_sl_82	{35377.0   , -6.848  , 1957.613};

Handle(TColgp_HArray1OfPnt) l_profile_sl_curve_points = new TColgp_HArray1OfPnt(1,82);

l_profile_sl_curve_points->SetValue(1,  l_sl_1);
l_profile_sl_curve_points->SetValue(2,  l_sl_2);
l_profile_sl_curve_points->SetValue(3,  l_sl_3);
l_profile_sl_curve_points->SetValue(4,  l_sl_4);
l_profile_sl_curve_points->SetValue(5,  l_sl_5);
l_profile_sl_curve_points->SetValue(6,  l_sl_6);
l_profile_sl_curve_points->SetValue(7,  l_sl_7);
l_profile_sl_curve_points->SetValue(8,  l_sl_8);
l_profile_sl_curve_points->SetValue(9,  l_sl_9);
l_profile_sl_curve_points->SetValue(10, l_sl_10);
l_profile_sl_curve_points->SetValue(11, l_sl_11);
l_profile_sl_curve_points->SetValue(12, l_sl_12);
l_profile_sl_curve_points->SetValue(13, l_sl_13);
l_profile_sl_curve_points->SetValue(14, l_sl_14);
l_profile_sl_curve_points->SetValue(15, l_sl_15);
l_profile_sl_curve_points->SetValue(16, l_sl_16);
l_profile_sl_curve_points->SetValue(17, l_sl_17);
l_profile_sl_curve_points->SetValue(18, l_sl_18);
l_profile_sl_curve_points->SetValue(19, l_sl_19);
l_profile_sl_curve_points->SetValue(20, l_sl_20);
l_profile_sl_curve_points->SetValue(21, l_sl_21);
l_profile_sl_curve_points->SetValue(22, l_sl_22);
l_profile_sl_curve_points->SetValue(23, l_sl_23);
l_profile_sl_curve_points->SetValue(24, l_sl_24);
l_profile_sl_curve_points->SetValue(25, l_sl_25);
l_profile_sl_curve_points->SetValue(26, l_sl_26);
l_profile_sl_curve_points->SetValue(27, l_sl_27);
l_profile_sl_curve_points->SetValue(28, l_sl_28);
l_profile_sl_curve_points->SetValue(29, l_sl_29);
l_profile_sl_curve_points->SetValue(30, l_sl_30);
l_profile_sl_curve_points->SetValue(31, l_sl_31);
l_profile_sl_curve_points->SetValue(32, l_sl_32);
l_profile_sl_curve_points->SetValue(33, l_sl_33);
l_profile_sl_curve_points->SetValue(34, l_sl_34);
l_profile_sl_curve_points->SetValue(35, l_sl_35);
l_profile_sl_curve_points->SetValue(36, l_sl_36);
l_profile_sl_curve_points->SetValue(37, l_sl_37);
l_profile_sl_curve_points->SetValue(38, l_sl_38);
l_profile_sl_curve_points->SetValue(39, l_sl_39);
l_profile_sl_curve_points->SetValue(40, l_sl_40);
l_profile_sl_curve_points->SetValue(41, l_sl_41);
l_profile_sl_curve_points->SetValue(42, l_sl_42);
l_profile_sl_curve_points->SetValue(43, l_sl_43);
l_profile_sl_curve_points->SetValue(44, l_sl_44);
l_profile_sl_curve_points->SetValue(45, l_sl_45);
l_profile_sl_curve_points->SetValue(46, l_sl_46);
l_profile_sl_curve_points->SetValue(47, l_sl_47);
l_profile_sl_curve_points->SetValue(48, l_sl_48);
l_profile_sl_curve_points->SetValue(49, l_sl_49);
l_profile_sl_curve_points->SetValue(50, l_sl_50);
l_profile_sl_curve_points->SetValue(51, l_sl_51);
l_profile_sl_curve_points->SetValue(52, l_sl_52);
l_profile_sl_curve_points->SetValue(53, l_sl_53);
l_profile_sl_curve_points->SetValue(54, l_sl_54);
l_profile_sl_curve_points->SetValue(55, l_sl_55);
l_profile_sl_curve_points->SetValue(56, l_sl_56);
l_profile_sl_curve_points->SetValue(57, l_sl_57);
l_profile_sl_curve_points->SetValue(58, l_sl_58);
l_profile_sl_curve_points->SetValue(59, l_sl_59);
l_profile_sl_curve_points->SetValue(60, l_sl_60);
l_profile_sl_curve_points->SetValue(61, l_sl_61);
l_profile_sl_curve_points->SetValue(62, l_sl_62);
l_profile_sl_curve_points->SetValue(63, l_sl_63);
l_profile_sl_curve_points->SetValue(64, l_sl_64);
l_profile_sl_curve_points->SetValue(65, l_sl_65);
l_profile_sl_curve_points->SetValue(66, l_sl_66);
l_profile_sl_curve_points->SetValue(67, l_sl_67);
l_profile_sl_curve_points->SetValue(68, l_sl_68);
l_profile_sl_curve_points->SetValue(69, l_sl_69);
l_profile_sl_curve_points->SetValue(70, l_sl_70);
l_profile_sl_curve_points->SetValue(71, l_sl_71);
l_profile_sl_curve_points->SetValue(72, l_sl_72);
l_profile_sl_curve_points->SetValue(73, l_sl_73);
l_profile_sl_curve_points->SetValue(74, l_sl_74);
l_profile_sl_curve_points->SetValue(75, l_sl_75);
l_profile_sl_curve_points->SetValue(76, l_sl_76);
l_profile_sl_curve_points->SetValue(77, l_sl_77);
l_profile_sl_curve_points->SetValue(78, l_sl_78);
l_profile_sl_curve_points->SetValue(79, l_sl_79);
l_profile_sl_curve_points->SetValue(80, l_sl_80);
l_profile_sl_curve_points->SetValue(81, l_sl_81);
l_profile_sl_curve_points->SetValue(82, l_sl_82);

Handle(Geom_BSplineCurve) l_profile_sl_curve
	= geoml::interpolate_points_to_b_spline_curve(l_profile_sl_curve_points);

writeToStp(l_profile_sl_curve, "l_profile_sl_curve.stp"); 

// define the upper profile curve:				
gp_Pnt u_sl_82 	{36830.314, -2.39    , 7822.764};
gp_Pnt u_sl_81	{36814.51,  -4.607   , 7822.764};
gp_Pnt u_sl_80	{36790.803, -7.882   , 7822.764};
gp_Pnt u_sl_79	{36759.189, -12.151  , 7822.764};
gp_Pnt u_sl_78	{36735.48,  -15.306  , 7822.764};
gp_Pnt u_sl_77	{36711.771, -18.413  , 7822.764};
gp_Pnt u_sl_76	{36688.062, -21.475  , 7822.764};
gp_Pnt u_sl_75	{36664.353, -24.492  , 7822.764};
gp_Pnt u_sl_74	{36640.644, -27.466  , 7822.764};
gp_Pnt u_sl_73	{36616.935, -30.397  , 7822.764};
gp_Pnt u_sl_72	{36593.226, -33.285  , 7822.764};
gp_Pnt u_sl_71	{36569.517, -36.132  , 7822.764};
gp_Pnt u_sl_70	{36545.808, -38.938  , 7822.764};
gp_Pnt u_sl_69	{36522.099, -41.703  , 7822.764};
gp_Pnt u_sl_68	{36498.39,  -44.428  , 7822.764};
gp_Pnt u_sl_67	{36474.681, -47.114  , 7822.764};
gp_Pnt u_sl_66	{36450.972, -49.76   , 7822.764};
gp_Pnt u_sl_65	{36427.263, -52.366  , 7822.764};
gp_Pnt u_sl_64	{36403.554, -54.933  , 7822.764};
gp_Pnt u_sl_63	{36379.845, -57.46   , 7822.764};
gp_Pnt u_sl_62	{36356.136, -59.949  , 7822.764};
gp_Pnt u_sl_61	{36332.427, -62.397  , 7822.764};
gp_Pnt u_sl_60	{36308.718, -64.805  , 7822.764};
gp_Pnt u_sl_59	{36285.009, -67.173  , 7822.764};
gp_Pnt u_sl_58	{36261.3,   -69.501  , 7822.764};
gp_Pnt u_sl_57	{36237.591, -71.787  , 7822.764};
gp_Pnt u_sl_56	{36213.882, -74.03   , 7822.764};
gp_Pnt u_sl_55	{36190.173, -76.232  , 7822.764};
gp_Pnt u_sl_54	{36166.464, -78.389  , 7822.764};
gp_Pnt u_sl_53	{36142.755, -80.502  , 7822.764};
gp_Pnt u_sl_52	{36119.046, -82.569  , 7822.764};
gp_Pnt u_sl_51	{36095.337, -84.59   , 7822.764};
gp_Pnt u_sl_50	{36071.628, -86.562  , 7822.764};
gp_Pnt u_sl_49	{36047.919, -88.485  , 7822.764};
gp_Pnt u_sl_48	{36024.21,  -90.358  , 7822.764};
gp_Pnt u_sl_47	{36000.501, -92.177  , 7822.764};
gp_Pnt u_sl_46	{35976.792, -93.943  , 7822.764};
gp_Pnt u_sl_45	{35953.083, -95.652  , 7822.764};
gp_Pnt u_sl_44	{35929.374, -97.304  , 7822.764};
gp_Pnt u_sl_43	{35905.665, -98.895  , 7822.764};
gp_Pnt u_sl_42	{35881.957, -100.423 , 7822.764};
gp_Pnt u_sl_41	{35858.248, -101.887 , 7822.764};
gp_Pnt u_sl_40	{35834.539, -103.284 , 7822.764};
gp_Pnt u_sl_39	{35810.83,  -104.61  , 7822.764};
gp_Pnt u_sl_38	{35787.121, -105.863 , 7822.764};
gp_Pnt u_sl_37	{35763.412, -107.041 , 7822.764};
gp_Pnt u_sl_36	{35739.703, -108.138 , 7822.764};
gp_Pnt u_sl_35	{35715.993, -109.153 , 7822.764};
gp_Pnt u_sl_34	{35692.284, -110.082 , 7822.764};
gp_Pnt u_sl_33	{35668.575, -110.92  , 7822.764};
gp_Pnt u_sl_32	{35644.866, -111.663 , 7822.764};
gp_Pnt u_sl_31	{35621.157, -112.307 , 7822.764};
gp_Pnt u_sl_30	{35597.448, -112.847 , 7822.764};
gp_Pnt u_sl_29	{35573.739, -113.277 , 7822.764};
gp_Pnt u_sl_28	{35550.03,  -113.594 , 7822.764};
gp_Pnt u_sl_27	{35526.321, -113.789 , 7822.764};
gp_Pnt u_sl_26	{35502.611, -113.858 , 7822.764};
gp_Pnt u_sl_25	{35478.902, -113.793 , 7822.764};
gp_Pnt u_sl_24	{35455.193, -113.587 , 7822.764};
gp_Pnt u_sl_23	{35431.483, -113.231 , 7822.764};
gp_Pnt u_sl_22	{35407.774, -112.717 , 7822.764};
gp_Pnt u_sl_21	{35384.064, -112.034 , 7822.764};
gp_Pnt u_sl_20	{35360.354, -111.172 , 7822.764};
gp_Pnt u_sl_19	{35336.644, -110.119 , 7822.764};
gp_Pnt u_sl_18	{35312.934, -108.861 , 7822.764};
gp_Pnt u_sl_17	{35289.224, -107.383 , 7822.764};
gp_Pnt u_sl_16	{35265.513, -105.666 , 7822.764};
gp_Pnt u_sl_15	{35241.802, -103.691 , 7822.764};
gp_Pnt u_sl_14	{35218.09,  -101.433 , 7822.764};
gp_Pnt u_sl_13	{35194.378, -98.864  , 7822.764};
gp_Pnt u_sl_12	{35170.664, -95.949  , 7822.764};
gp_Pnt u_sl_11	{35146.949, -92.645  , 7822.764};
gp_Pnt u_sl_10	{35123.232, -88.898  , 7822.764};
gp_Pnt u_sl_9	{35099.511, -84.639  , 7822.764};
gp_Pnt u_sl_8	{35075.781, -79.767  , 7822.764};
gp_Pnt u_sl_7	{35052.047, -74.163  , 7822.764};
gp_Pnt u_sl_6	{35028.25,  -67.553  , 7822.764};
gp_Pnt u_sl_5	{35004.502, -59.842  , 7822.764};
gp_Pnt u_sl_4	{34979.521, -49.122  , 7822.764};
gp_Pnt u_sl_3	{34955.711, -37.697  , 7822.764};
gp_Pnt u_sl_2	{34933.596, -12.915  , 7822.764};
gp_Pnt u_sl_1	{34933.596,     0.0  , 7822.764};

Handle(TColgp_HArray1OfPnt) u_profile_sl_curve_points = new TColgp_HArray1OfPnt(1,82);

u_profile_sl_curve_points->SetValue(1,  u_sl_1);
u_profile_sl_curve_points->SetValue(2,  u_sl_2);
u_profile_sl_curve_points->SetValue(3,  u_sl_3);
u_profile_sl_curve_points->SetValue(4,  u_sl_4);
u_profile_sl_curve_points->SetValue(5,  u_sl_5);
u_profile_sl_curve_points->SetValue(6,  u_sl_6);
u_profile_sl_curve_points->SetValue(7,  u_sl_7);
u_profile_sl_curve_points->SetValue(8,  u_sl_8);
u_profile_sl_curve_points->SetValue(9,  u_sl_9);
u_profile_sl_curve_points->SetValue(10, u_sl_10);
u_profile_sl_curve_points->SetValue(11, u_sl_11);
u_profile_sl_curve_points->SetValue(12, u_sl_12);
u_profile_sl_curve_points->SetValue(13, u_sl_13);
u_profile_sl_curve_points->SetValue(14, u_sl_14);
u_profile_sl_curve_points->SetValue(15, u_sl_15);
u_profile_sl_curve_points->SetValue(16, u_sl_16);
u_profile_sl_curve_points->SetValue(17, u_sl_17);
u_profile_sl_curve_points->SetValue(18, u_sl_18);
u_profile_sl_curve_points->SetValue(19, u_sl_19);
u_profile_sl_curve_points->SetValue(20, u_sl_20);
u_profile_sl_curve_points->SetValue(21, u_sl_21);
u_profile_sl_curve_points->SetValue(22, u_sl_22);
u_profile_sl_curve_points->SetValue(23, u_sl_23);
u_profile_sl_curve_points->SetValue(24, u_sl_24);
u_profile_sl_curve_points->SetValue(25, u_sl_25);
u_profile_sl_curve_points->SetValue(26, u_sl_26);
u_profile_sl_curve_points->SetValue(27, u_sl_27);
u_profile_sl_curve_points->SetValue(28, u_sl_28);
u_profile_sl_curve_points->SetValue(29, u_sl_29);
u_profile_sl_curve_points->SetValue(30, u_sl_30);
u_profile_sl_curve_points->SetValue(31, u_sl_31);
u_profile_sl_curve_points->SetValue(32, u_sl_32);
u_profile_sl_curve_points->SetValue(33, u_sl_33);
u_profile_sl_curve_points->SetValue(34, u_sl_34);
u_profile_sl_curve_points->SetValue(35, u_sl_35);
u_profile_sl_curve_points->SetValue(36, u_sl_36);
u_profile_sl_curve_points->SetValue(37, u_sl_37);
u_profile_sl_curve_points->SetValue(38, u_sl_38);
u_profile_sl_curve_points->SetValue(39, u_sl_39);
u_profile_sl_curve_points->SetValue(40, u_sl_40);
u_profile_sl_curve_points->SetValue(41, u_sl_41);
u_profile_sl_curve_points->SetValue(42, u_sl_42);
u_profile_sl_curve_points->SetValue(43, u_sl_43);
u_profile_sl_curve_points->SetValue(44, u_sl_44);
u_profile_sl_curve_points->SetValue(45, u_sl_45);
u_profile_sl_curve_points->SetValue(46, u_sl_46);
u_profile_sl_curve_points->SetValue(47, u_sl_47);
u_profile_sl_curve_points->SetValue(48, u_sl_48);
u_profile_sl_curve_points->SetValue(49, u_sl_49);
u_profile_sl_curve_points->SetValue(50, u_sl_50);
u_profile_sl_curve_points->SetValue(51, u_sl_51);
u_profile_sl_curve_points->SetValue(52, u_sl_52);
u_profile_sl_curve_points->SetValue(53, u_sl_53);
u_profile_sl_curve_points->SetValue(54, u_sl_54);
u_profile_sl_curve_points->SetValue(55, u_sl_55);
u_profile_sl_curve_points->SetValue(56, u_sl_56);
u_profile_sl_curve_points->SetValue(57, u_sl_57);
u_profile_sl_curve_points->SetValue(58, u_sl_58);
u_profile_sl_curve_points->SetValue(59, u_sl_59);
u_profile_sl_curve_points->SetValue(60, u_sl_60);
u_profile_sl_curve_points->SetValue(61, u_sl_61);
u_profile_sl_curve_points->SetValue(62, u_sl_62);
u_profile_sl_curve_points->SetValue(63, u_sl_63);
u_profile_sl_curve_points->SetValue(64, u_sl_64);
u_profile_sl_curve_points->SetValue(65, u_sl_65);
u_profile_sl_curve_points->SetValue(66, u_sl_66);
u_profile_sl_curve_points->SetValue(67, u_sl_67);
u_profile_sl_curve_points->SetValue(68, u_sl_68);
u_profile_sl_curve_points->SetValue(69, u_sl_69);
u_profile_sl_curve_points->SetValue(70, u_sl_70);
u_profile_sl_curve_points->SetValue(71, u_sl_71);
u_profile_sl_curve_points->SetValue(72, u_sl_72);
u_profile_sl_curve_points->SetValue(73, u_sl_73);
u_profile_sl_curve_points->SetValue(74, u_sl_74);
u_profile_sl_curve_points->SetValue(75, u_sl_75);
u_profile_sl_curve_points->SetValue(76, u_sl_76);
u_profile_sl_curve_points->SetValue(77, u_sl_77);
u_profile_sl_curve_points->SetValue(78, u_sl_78);
u_profile_sl_curve_points->SetValue(79, u_sl_79);
u_profile_sl_curve_points->SetValue(80, u_sl_80);
u_profile_sl_curve_points->SetValue(81, u_sl_81);
u_profile_sl_curve_points->SetValue(82, u_sl_82);

Handle(Geom_BSplineCurve) u_profile_sl_curve
	= geoml::interpolate_points_to_b_spline_curve(u_profile_sl_curve_points);

writeToStp(u_profile_sl_curve, "u_profile_sl_curve.stp"); 

// create the half surface of Seitenleitwerk:
std::vector<Handle(Geom_Curve)> profiles_sl_curves;
profiles_sl_curves.push_back(l_profile_sl_curve);
profiles_sl_curves.push_back(u_profile_sl_curve);

Handle(Geom_BSplineSurface) half_sl_surface
	= geoml::interpolate_curves(profiles_sl_curves);

// write upper_hl_surface to file:
writeGeomEntityToStepFile(half_sl_surface, "half_sl_surface.stp");

// create a solid of the wing:
// create faces of the four wing surfaces:
TopoDS_Face face_upper_wing = BRepBuilderAPI_MakeFace(upper_wing_surface, 1e1);
TopoDS_Face face_lower_wing = BRepBuilderAPI_MakeFace(lower_wing_surface, 1e1);
TopoDS_Face face_inner_cap  = BRepBuilderAPI_MakeFace(inner_wing_cap_surface, 1e1);
TopoDS_Face face_outer_cap  = BRepBuilderAPI_MakeFace(outer_wing_cap_surface, 1e1);

// Combine the faces into a shell
BRep_Builder builder_wing;

TopoDS_Shell shell_wing;
builder_wing.MakeShell(shell_wing);
builder_wing.Add(shell_wing, face_upper_wing);
builder_wing.Add(shell_wing, face_lower_wing);
builder_wing.Add(shell_wing, face_inner_cap);
builder_wing.Add(shell_wing, face_outer_cap);

writeGeomEntityToStepFile(shell_wing, "shell_wing.stp");

/*
BRepBuilderAPI_Sewing sewing;
sewing.Add(face_upper_wing);
sewing.Add(face_lower_wing);
sewing.Add(face_inner_cap);
sewing.Add(face_outer_cap);

TopoDS_Shape combined_wing = sewing.SewedShape();
writeGeomEntityToStepFile(combined_wing, "combined_wing.stp");
*/

// Create a solid from the shell
TopoDS_Solid solid_wing;
builder_wing.MakeSolid(solid_wing);
builder_wing.Add(solid_wing, shell_wing);

writeGeomEntityToStepFile(solid_wing, "solid_wing.stp");

// create a solid of the fuselage:
// create faces of the five fuselage surfaces:
TopoDS_Face face_cylinder_fuselage = BRepBuilderAPI_MakeFace(fuselage_cylinder, 1e1);
TopoDS_Face face_rear_fuselage = BRepBuilderAPI_MakeFace(rear_surface, 1e1);
TopoDS_Face face_nose_cap_fuselage  = BRepBuilderAPI_MakeFace(nose_ogive_cap_srf, 1e1);
TopoDS_Face face_nose_fuselage  = BRepBuilderAPI_MakeFace(nose_ogive_srf, 1e1);
TopoDS_Face face_rear_cap_fuselage  = BRepBuilderAPI_MakeFace(rear_surface_cap, 1e1);

// Combine the faces into a shell
BRep_Builder builder_fuselage;

TopoDS_Shell shell_fuselage;
builder_fuselage.MakeShell(shell_fuselage);
builder_fuselage.Add(shell_fuselage, face_cylinder_fuselage);
builder_fuselage.Add(shell_fuselage, face_rear_fuselage);
builder_fuselage.Add(shell_fuselage, face_nose_cap_fuselage);
builder_fuselage.Add(shell_fuselage, face_nose_fuselage);
builder_fuselage.Add(shell_fuselage, face_rear_cap_fuselage);

writeGeomEntityToStepFile(shell_fuselage, "shell_fuselage.stp");

// Create a solid from the shell
TopoDS_Solid solid_fuselage;
builder_fuselage.MakeSolid(solid_fuselage);
builder_fuselage.Add(solid_fuselage, shell_fuselage);

writeGeomEntityToStepFile(solid_fuselage, "solid_fuselage.stp");

// fuse wing with fuselage:
BRepAlgoAPI_Fuse fuser (solid_wing, solid_fuselage);
fuser.Build();

TopoDS_Shape fused_wing_fuselage = fuser.Shape();

writeGeomEntityToStepFile(fused_wing_fuselage, "fused_wing_fuselage.stp");


std::cout << "end of wing fuselage configuration modelling" << std::endl;



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////																		   /////////
/////////					        // Box with Holes //						   /////////
/////////							////////////////////						   /////////
/////////																		   /////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

 
std::cout << "start to model box with sphere cutouts" << std::endl;

// create points
gp_Pnt o_corner1(0, 0, 0);
gp_Pnt o_corner2(0, 1, 0);
gp_Pnt o_corner3(0, 1, 1);
gp_Pnt o_corner4(0, 0, 1);

// define a surface defined by 4 points:
std::vector <gp_Pnt> o_corner_points{o_corner1, o_corner2, o_corner3, o_corner4};

Handle(Geom_BSplineSurface) o_four_point_srf 
	= geoml::surface_from_4_points (o_corner_points);

// write four_point_srf to file:
writeGeomEntityToStepFile(o_four_point_srf, "o_four_point_srf.stp");

// now, extrude this surface:

// define the extrusion direction:
gp_Vec extr_dir(1.0, 0.0, 0.0);
gp_Vec extr_vec = geoml::scale_vector(extr_dir, 10.0);

// create the extrusion:
TopoDS_Face o_four_point_srf_shape = BRepBuilderAPI_MakeFace(o_four_point_srf, 1e-1);

BRepPrimAPI_MakePrism o_extrusion (o_four_point_srf_shape, extr_vec);

TopoDS_Shape o_extruded_shape = o_extrusion.Shape();

// write extruded_shape to file:
writeGeomEntityToStepFile(o_extruded_shape, "o_extruded_shape.stp");

// create the inner surface of the box:

double thickness (0.1);

// create points
gp_Pnt i_corner1(0, o_corner1.Y() + thickness, o_corner1.Z() + thickness);
gp_Pnt i_corner2(0, o_corner2.Y() - thickness, o_corner2.Z() + thickness);
gp_Pnt i_corner3(0, o_corner3.Y() - thickness, o_corner3.Z() - thickness);
gp_Pnt i_corner4(0, o_corner4.Y() + thickness, o_corner4.Z() - thickness);

// define a surface by 4 points:
std::vector <gp_Pnt> i_corner_points{i_corner1, i_corner2, i_corner3, i_corner4};

Handle(Geom_BSplineSurface) i_four_point_srf 
	= geoml::surface_from_4_points (i_corner_points);

// write four_point_srf to file:
writeGeomEntityToStepFile(i_four_point_srf, "i_four_point_srf.stp");

// now, extrude this surface:

// create the extrusion:
TopoDS_Face i_four_point_srf_shape = BRepBuilderAPI_MakeFace(i_four_point_srf, 1e-1);

BRepPrimAPI_MakePrism i_extrusion (i_four_point_srf_shape, extr_vec);

TopoDS_Shape i_extruded_shape = i_extrusion.Shape();

// write extruded_shape to file:
writeGeomEntityToStepFile(i_extruded_shape, "i_extruded_shape.stp");

// curout:
BRepAlgoAPI_Cut cutter(o_extruded_shape, i_extruded_shape);
cutter.Build();
TopoDS_Shape result_cut_box = cutter.Shape();

// write result_cut_box to disk:
writeGeomEntityToStepFile(result_cut_box, "result_cut_box.stp");

// now, position spheres to create cutouts later:

double sphere_position (3.0);

gp_Pnt center_1 = geoml::move(o_corner1, extr_dir, sphere_position);
gp_Pnt center_2 = geoml::move(o_corner2, extr_dir, sphere_position);
gp_Pnt center_3 = geoml::move(o_corner3, extr_dir, sphere_position);
gp_Pnt center_4 = geoml::move(o_corner4, extr_dir, sphere_position);

// now, create the spheres:
double sphere_radius (0.2);

BRepPrimAPI_MakeSphere sphere_1(center_1, sphere_radius);
BRepPrimAPI_MakeSphere sphere_2(center_2, sphere_radius);
BRepPrimAPI_MakeSphere sphere_3(center_3, sphere_radius);
BRepPrimAPI_MakeSphere sphere_4(center_4, sphere_radius);

TopoDS_Shape sphere_shape_1 = sphere_1.Shape();
TopoDS_Shape sphere_shape_2 = sphere_2.Shape();
TopoDS_Shape sphere_shape_3 = sphere_3.Shape();
TopoDS_Shape sphere_shape_4 = sphere_4.Shape();

// write result_cut_box to disk:
writeGeomEntityToStepFile(sphere_shape_1, "sphere_shape_1.stp");
writeGeomEntityToStepFile(sphere_shape_2, "sphere_shape_2.stp");
writeGeomEntityToStepFile(sphere_shape_3, "sphere_shape_3.stp");
writeGeomEntityToStepFile(sphere_shape_4, "sphere_shape_4.stp");

// now, make fillets:
std::initializer_list<int> index_list {1,2,5,8};


TopoDS_Shape rounded_box = geoml::make_fillet(result_cut_box, index_list, 0.05);

// write rounded_box to disk:
writeGeomEntityToStepFile(rounded_box, "rounded_box.stp");


std::cout << "end of main function" << std::endl;

return 0;
}


