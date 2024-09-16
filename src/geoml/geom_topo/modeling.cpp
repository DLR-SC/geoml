#include "geom_topo/modeling.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <BRep_Tool.hxx>

namespace geoml{

TopoDS_Face SurfaceToFace(const Handle(Geom_Surface) &surface, Standard_Real tolDegEd)
{
    BRepBuilderAPI_MakeFace faceMaker(surface, tolDegEd);
    TopoDS_Face face = faceMaker.Face();

    return face;
}

TopoDS_Edge CurveToEdge(const Handle(Geom_Curve) &curve)
{
    BRepBuilderAPI_MakeEdge edgeMaker(curve);
    TopoDS_Edge edge = edgeMaker.Edge();

    return edge;
}

Handle(Geom_Curve) EdgeToCurve(TopoDS_Edge const& edge)
{
    Standard_Real first_param, last_param;
    Handle(Geom_Curve) geom_curve = BRep_Tool::Curve(edge, first_param, last_param);  

    return geom_curve;
}


} // namespace geoml

