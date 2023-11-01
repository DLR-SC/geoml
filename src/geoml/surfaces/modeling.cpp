#include "surfaces/modeling.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"

// here, I have to clean up and delete the includes that are not used:
#include <iostream>
#include <cmath>
#include <vector>
#include <geoml/curves/modeling.h>
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
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Circ.hxx>
#include <GeomConvert.hxx>
#include <NCollection_Array2.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <GeomConvert.hxx>




Handle(Geom_BSplineSurface)
geoml::interpolate_curve_network(const std::vector<Handle (Geom_Curve)> &ucurves,
                                 const std::vector<Handle (Geom_Curve)> &vcurves,
                                 double tolerance)
{
    geoml::InterpolateCurveNetwork interpolator(ucurves, vcurves, tolerance);

    return interpolator.Surface();
}

Handle(Geom_BSplineSurface)
geoml::interpolate_curves(const std::vector<Handle (Geom_Curve)> &ucurves, unsigned int max_degree,
                          bool join_continously)
{
    geoml::CurvesToSurface c2s(ucurves, join_continously);
    c2s.SetMaxDegree(max_degree);
    return c2s.Surface();
}

Handle(Geom_BSplineSurface)
geoml::revolving_surface(const Handle(Geom_BSplineCurve)& profile_curve,
                         const gp_Ax1& rotation_axis,
                         const Standard_Real angle)
{   
    BRepBuilderAPI_MakeEdge edgeMaker(profile_curve);
    TopoDS_Edge edge = edgeMaker.Edge();
    TopoDS_Shape shape = edge;
 
    BRepPrimAPI_MakeRevol revol_maker(shape, 
    								  rotation_axis,
                                      angle); 

    TopoDS_Shape revol_shape = revol_maker.Shape();

    // Now, extract a B-spline surface out of the TopoDS_Shape 
    // For that, extract the face first
    TopoDS_Face face;
    for (TopExp_Explorer explorer(revol_shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        face = TopoDS::Face(explorer.Current());
        break; // Let us assume, there is only one face in the shape
    }

    Handle(Geom_Surface) revol_geom_surface = BRep_Tool::Surface(face);

    Handle(Geom_BSplineSurface)
        revol_b_spline_surface = GeomConvert::SurfaceToBSplineSurface(revol_geom_surface);

    return revol_b_spline_surface;
}

GEOML_API_EXPORT Handle(Geom_BSplineSurface)
geoml::b_spline_surface(const TColgp_Array2OfPnt &control_points, 
                 const TColStd_Array2OfReal &weights, 
                 const TColStd_Array1OfReal &U_knots, 
                 const TColStd_Array1OfReal &V_knots, 
                 const TColStd_Array1OfInteger &U_mults, 
                 const TColStd_Array1OfInteger &V_mults, 
                 const Standard_Integer U_degree, 
                 const Standard_Integer V_degree, 
                 const Standard_Boolean U_periodic, 
                 const Standard_Boolean V_periodic)
{
    /*
    TColgp_Array2OfPnt control_points_col (1, control_points.at(0).size(), 1, control_points.size());

    for(int i = 0; i < control_points.size(); ++i)
    {
        for(int j = 0; j < control_points.at(0).size(); ++j)
        {
            control_points_col.SetValue(j + 1, i + 1, control_points.at(i).at(j));
        }
    }

    TColStd_Array2OfReal weights_col (1, control_points.at(0).size(), 1, control_points.size());

    for(int i = 0; i < control_points.size(); ++i)
    {
        for(int j = 0; j < control_points.at(0).size(); ++j)
        {
            weights_col.SetValue(j + 1, i + 1, weights.at(i).at(j));
        }
    }

    TColStd_Array1OfReal U_knots_col (1, U_knots.size());

    for(int i = 0; i < U_knots.size(); ++i)
    {
        U_knots_col.SetValue(i + 1, U_knots.at(i));
    }

    TColStd_Array1OfReal V_knots_col (1, V_knots.size());

    for(int i = 0; i < V_knots.size(); ++i)
    {
        V_knots_col.SetValue(i + 1, V_knots.at(i));
    }

    TColStd_Array1OfInteger U_mults_col (1, U_mults.size());

    for(int i = 0; i < U_mults.size(); ++i)
    {
        U_mults_col.SetValue(i + 1, U_mults.at(i));
    }

    TColStd_Array1OfInteger V_mults_col (1, V_mults.size());

    for(int i = 0; i < V_mults.size(); ++i)
    {
        V_mults_col.SetValue(i + 1, V_mults.at(i));
    }
    
    Handle(Geom_BSplineSurface) srf_handle = new Geom_BSplineSurface( control_points_col,
                                                                    weights_col,
                                                                    U_knots_col,
                                                                    V_knots_col,
                                                                    U_mults_col,
                                                                    V_mults_col,
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic );*/
    
    Handle(Geom_BSplineSurface) srf_handle = new Geom_BSplineSurface( control_points,
                                                                    weights,
                                                                    U_knots,
                                                                    V_knots,
                                                                    U_mults,
                                                                    V_mults,
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic );

    return srf_handle;                                        
}                 

Handle(Geom_BSplineSurface)
geoml::surface_from_4_points (const std::vector<gp_Pnt> & points)
{
    // set the control points
    TColgp_Array2OfPnt cp_net(1, 2, 1, 2);
    cp_net.SetValue(1, 1, points.at(0));
    cp_net.SetValue(2, 1, points.at(1));
    cp_net.SetValue(1, 2, points.at(3));
    cp_net.SetValue(2, 2, points.at(2));

    // set the degree in U and Vdirection
    // degree:
    Standard_Integer degree_U = 1;
    Standard_Integer degree_V = 1;

    // define the weight net:
    TColStd_Array2OfReal weight_net(1, 2, 1, 2); 
    weight_net.SetValue(1, 1, 1);
    weight_net.SetValue(2, 1, 1);
    weight_net.SetValue(1, 2, 1);
    weight_net.SetValue(2, 2, 1);    

    // set the U-knots:
    TColStd_Array1OfReal knots_U(1,2); 

    knots_U.SetValue(1, 0.0);           
    knots_U.SetValue(2, 1.0);

    // set the V-knots:
    TColStd_Array1OfReal knots_V(1,2); 

    knots_V.SetValue(1, 0.0);           
    knots_V.SetValue(2, 1.0);

    // multiplicities U-direction: 
    TColStd_Array1OfInteger mults_U(1,2); 
    mults_U.SetValue(1,2);                
    mults_U.SetValue(2,2);

    // multiplicities V-direction: 
    TColStd_Array1OfInteger mults_V(1,2); 
    mults_V.SetValue(1,2);                
    mults_V.SetValue(2,2);

    Handle(Geom_BSplineSurface) srf_handle = new Geom_BSplineSurface( cp_net,
                                                                      weight_net,
                                                                      knots_U,
                                                                      knots_V,
                                                                      mults_U,
                                                                      mults_V,
                                                                      degree_U,
                                                                      degree_V );
                                                                        
    return srf_handle;

}                                                                        