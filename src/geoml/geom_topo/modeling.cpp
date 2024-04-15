#include "geom_topo/modeling.h"
#include <Geom_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>


namespace geoml{

TopoDS_Face
SurfaceToFace(const Handle(Geom_Surface) &surface)
{
    Standard_Real tolerance (1e-5);
    BRepBuilderAPI_MakeFace faceMaker(surface, tolerance);
    TopoDS_Face face = faceMaker.Face();

    return face;
}

TopoDS_Edge
CurveToEdge(const Handle(Geom_Curve) &curve)
{
    BRepBuilderAPI_MakeEdge edgeMaker(curve);
    TopoDS_Edge edge = edgeMaker.Edge();

    return edge;
}

} // namespace geoml

