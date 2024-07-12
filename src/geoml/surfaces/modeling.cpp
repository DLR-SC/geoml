#include "surfaces/modeling.h"
#include "geoml/curves/modeling.h"
#include "geoml/data_structures/conversions.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"
#include "common/CommonFunctions.h"

#include "BRepBuilderAPI_MakeEdge.hxx"
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx> //
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <GeomConvert.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <iostream>

#include <STEPControl_Writer.hxx>
////////////////
void writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName)
{
STEPControl_Writer writer;
writer.Transfer(my_shape,STEPControl_AsIs);
writer.Write(fileName.c_str());
}
////////////////

namespace geoml{

Handle(Geom_BSplineSurface) interpolate_curve_network(const std::vector<Handle (Geom_Curve)> &ucurves,
                                 const std::vector<Handle (Geom_Curve)> &vcurves,
                                 double tolerance)
{
    InterpolateCurveNetwork interpolator(ucurves, vcurves, tolerance);

    return interpolator.Surface();
}

Handle(Geom_BSplineSurface)
interpolate_curves(const std::vector<Handle (Geom_Curve)> &ucurves, unsigned int max_degree,
                          bool join_continuously)
{
    CurvesToSurface c2s(ucurves, join_continuously);
    c2s.SetMaxDegree(max_degree);
    return c2s.Surface();
}

TopoDS_Shape
revolving_shape(const TopoDS_Shape& profile_shape,
                const gp_Pnt& start_point_rotation_axis,
                const gp_Vec& rotation_axis_direction,
                const Standard_Real angle)
{   
    gp_Dir dir (rotation_axis_direction);
    gp_Ax1 axis (start_point_rotation_axis, dir);
    
    BRepPrimAPI_MakeRevol revol_maker(profile_shape, axis, angle); 

    TopoDS_Shape revol_shape = revol_maker.Shape();

    return revol_shape;
}

Handle(Geom_BSplineSurface)
nurbs_surface(const Array2d<gp_Pnt> &control_points, 
                 const Array2d<Standard_Real> &weights, 
                 const std::vector<Standard_Real> &U_knots, 
                 const std::vector<Standard_Real> &V_knots, 
                 const std::vector<int> &U_mults, 
                 const std::vector<int> &V_mults, 
                 const int U_degree, 
                 const int V_degree, 
                 const bool U_periodic,
                 const bool V_periodic)
{
    return new Geom_BSplineSurface(geoml::Array2d_to_TCol(control_points),
                                                                    geoml::Array2d_to_TCol(weights),
                                                                    geoml::StdVector_to_TCol(U_knots),
                                                                    geoml::StdVector_to_TCol(V_knots),
                                                                    geoml::StdVector_to_TCol(U_mults),
                                                                    geoml::StdVector_to_TCol(V_mults),
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic);

}

Handle(Geom_BSplineSurface)
surface_from_4_points(const gp_Pnt &p_1, const gp_Pnt &p_2, const gp_Pnt &p_3, const gp_Pnt &p_4)
{
    // set the control points
    geoml::Array2d<gp_Pnt> cp_net(2,2);

    cp_net.setValue(0, 0, p_1);
    cp_net.setValue(1, 0, p_2);
    cp_net.setValue(1, 1, p_3);
    cp_net.setValue(0, 1, p_4);

    // set the degree in u and v direction
    // degree:
    int degree_U = 1;
    int degree_V = 1;

    // define the weight net:
    geoml::Array2d<Standard_Real> weight_net(2,2); 

    weight_net.setValue(0, 0, 1);
    weight_net.setValue(1, 0, 1);
    weight_net.setValue(1, 1, 1);
    weight_net.setValue(0, 1, 1);    

    // set the U-knots:
    std::vector<Standard_Real> knots_U; 

    knots_U.push_back(0.0);           
    knots_U.push_back(1.0);

    // set the V-knots:
    std::vector<Standard_Real> knots_V; 

    knots_V.push_back(0.0);           
    knots_V.push_back(1.0);

    // multiplicities U-direction: 
    std::vector<int> mults_U; 
    mults_U.push_back(2);                
    mults_U.push_back(2);

    // multiplicities V-direction: 
    std::vector<int> mults_V; 
    mults_V.push_back(2);                
    mults_V.push_back(2);

    Handle(Geom_BSplineSurface) srf_handle = nurbs_surface( cp_net, 
                                                            weight_net, 
                                                            knots_U, 
                                                            knots_V, 
                                                            mults_U, 
                                                            mults_V, 
                                                            degree_U, 
                                                            degree_V);
                                                                        
    return srf_handle;
} 

TopoDS_Face
face_from_4_points(const gp_Pnt &p_1, const gp_Pnt &p_2, const gp_Pnt &p_3, const gp_Pnt &p_4)
{
    // TopoDS_Face face = BRepBuilderAPI_MakeFace(surface_from_4_points(p_1,p_2,p_3,p_4), Precision::Confusion());

    // return face;

    TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(p_1, p_2);
    TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(p_2, p_3);
    TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(p_3, p_4);
    TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(p_4, p_1);

    // Create wire from edges
    BRepBuilderAPI_MakeWire wireMaker;
    wireMaker.Add(edge1);
    wireMaker.Add(edge2);
    wireMaker.Add(edge3);
    wireMaker.Add(edge4);
    TopoDS_Wire wire = wireMaker.Wire();

writeGeomEntityToStepFile(wire, "my_wire.stp");

    // Create face from wire
    TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);

writeGeomEntityToStepFile(face, "my_face.stp");
    return face;

}

} // namespace geoml
