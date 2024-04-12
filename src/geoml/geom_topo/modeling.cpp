#include "geom_topo/modeling.h"
#include <Geom_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>


namespace geoml{

TopoDS_Face
Geom_BSplineSurfaceToTopoDS_Face(const Handle(Geom_BSplineSurface) &surface)
{
    Standard_Real tolerance (1e-5);
    BRepBuilderAPI_MakeFace faceMaker(surface, tolerance);
    TopoDS_Face face = faceMaker.Face();

    return face;
}

} // namespace geoml

