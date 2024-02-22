#include "utility/modeling.h"
#include "geometry/Transformation.h"
#include <BRep_Builder.hxx>
#include <BRepFilletAPI_MakeFillet.hxx> 
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Shape.hxx>
#include "STEPControl_Writer.hxx"
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <algorithm>


#include <iostream>

namespace geoml {

} // namespace geoml
