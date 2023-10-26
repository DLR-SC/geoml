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

#include <iostream>

namespace geoml {

TColgp_Array2OfPnt
extract_control_points_surface(

    const Handle(Geom_BSplineSurface)& b_spline_surface)
{
    TColgp_Array2OfPnt control_points = b_spline_surface->Poles();

    return control_points; 
}
 

//TColgp_Array1OfPnt 
std::vector<gp_Pnt> 
extract_control_point_column_row (const Handle(Geom_BSplineSurface)& b_spline_surface, 

                                  int UV_direction, int index)
{
    //if ( UV_direction != 1 || UV_direction != 2)
        //throw error message
    // if( index < 1 || index > ...)
        //throw error message

    TColgp_Array2OfPnt cp_net = b_spline_surface->Poles();
    std::vector<gp_Pnt> point_vector;

    if (UV_direction == 1)
    {            
        for(int i = 1; i <= cp_net.ColLength(); ++i)
        {
            point_vector.push_back(cp_net.Value(i, index));
        }

        return point_vector;

    } else if (UV_direction == 2)
    {   
        for(int i = 1; i <= cp_net.RowLength(); ++i)
        {
            point_vector.push_back(cp_net.Value(index, i));
        }

        return point_vector; 
    }   
}                            

TColgp_Array1OfPnt 
move (const TColgp_Array1OfPnt& points, gp_Vec direction, double factor)
{
    TColgp_Array1OfPnt my_array(1,points.Length());

    for (int i = 1; i <= points.Length(); ++i)
    {
        gp_Pnt moved_point( (points.Value(i)).X() + factor * direction.X(),
                            (points.Value(i)).Y() + factor * direction.Y(),
                            (points.Value(i)).Z() + factor * direction.Z() );
        my_array.SetValue(i, moved_point);
    }   

    return my_array;
}

gp_Pnt 
move (const gp_Pnt& point, gp_Vec direction, double factor)

{
        gp_Pnt moved_point( point.X() + factor * direction.X(),
                            point.Y() + factor * direction.Y(),
                            point.Z() + factor * direction.Z() );   

    return moved_point;
}

Handle(TColgp_HArray1OfPnt) 
move (const Handle(TColgp_HArray1OfPnt)& points, gp_Vec direction, double factor)
{
    Handle(TColgp_HArray1OfPnt) my_array = new TColgp_HArray1OfPnt(1,points->Length());

    for (int i = 1; i <= points->Length(); ++i)
    {
        gp_Pnt moved_point( (points->Value(i)).X() + factor * direction.X(),
                            (points->Value(i)).Y() + factor * direction.Y(),
                            (points->Value(i)).Z() + factor * direction.Z() );
        my_array->SetValue(i, moved_point);
    }   

    return my_array;
}

TopoDS_Shape moved(TopoDS_Shape const& origin, gp_Vec const& direction, double factor)
{
    auto t = Transformation(direction*factor);
    return t.Transform(origin);
}

TopoDS_Compound repeat_shape(TopoDS_Shape const& origin, Transformation const& trans, int n_repeats)
{
    TopoDS_Compound result;
    BRep_Builder a_builder;

    TopoDS_Shape shape = origin;
    a_builder.MakeCompound(result);
    a_builder.Add(result, shape);
    for (int i=1; i < n_repeats; ++i) {
        shape = trans.Transform(shape);
        a_builder.Add(result, shape);
    }
    return result;
}

TColgp_Array2OfPnt 
create_point_net_from_arrays (const std::vector<TColgp_Array1OfPnt>& point_lists,
                                     int rows_columns)
{
    // TODO: check if all arrays in the vector have the same lenght, throw error otherwise
    // TODO: check if rows_columns argument is eitehr 1 or 2, throw error otherwise

    if (rows_columns == 1)
    {   
        int num_rows = point_lists.size();
        int num_columns = point_lists.at(0).Length();

        TColgp_Array2OfPnt cp_net(1, num_rows, 1, num_columns);

        for (int i = 1; i <= num_columns; ++i) 
        {
            for (int j = 1; j <= num_rows; ++j)
            {
                cp_net.SetValue(j,i,point_lists.at(j - 1).Value(i));
            }
        }

        return cp_net;

    } else if (rows_columns == 2)
    {
        int num_columns = point_lists.size();
        int num_rows = point_lists.at(0).Length();

        TColgp_Array2OfPnt cp_net(1, num_rows, 1, num_columns);
        
        for (int i = 1; i <= num_columns; ++i) 
        {
            for (int j = 1; j <= num_rows; ++j)
            {
                cp_net.SetValue(j,i,point_lists.at(i - 1).Value(j));
            }
        }

        return cp_net;
    }

}

TopoDS_Shape
make_fillet (const TopoDS_Shape &solid_shape , int edge_index, double radius)
{
    BRepFilletAPI_MakeFillet MF (solid_shape);

    TopTools_IndexedMapOfShape edgeMap;
    TopExp::MapShapes(solid_shape, TopAbs_EDGE, edgeMap);

    MF.Add(radius, TopoDS::Edge(edgeMap(edge_index)));

    return MF.Shape();
}

TopoDS_Shape
make_fillet (const TopoDS_Shape &solid_shape , const std::vector<int> &edge_indices, double radius)
{
    BRepFilletAPI_MakeFillet MF (solid_shape);

    TopTools_IndexedMapOfShape edgeMap;
    TopExp::MapShapes(solid_shape, TopAbs_EDGE, edgeMap);

    for (int i: edge_indices)
    {
         MF.Add(radius, TopoDS::Edge(edgeMap(i)));  
    }

    return MF.Shape();      
}


gp_Vec
scale_vector (const gp_Vec &vector , double factor)
{
    return vector.Multiplied(factor);
}

///////////////////////////// temp

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

void writeGeomEntityToStepFile(Handle_Geom_BSplineSurface surface, std::string fileName)
{
    Handle_Geom_Surface surface_tmp = Handle_Geom_Surface::DownCast(surface);

	BRepBuilderAPI_MakeFace faceMaker;
	faceMaker.Init(surface_tmp,true,1E-6);	
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

TopoDS_Face make_face (const Handle_Geom_BSplineSurface & surface)
{
   TopoDS_Face face_tmp = BRepBuilderAPI_MakeFace(surface, 1e1); 
   return face_tmp;
}



} // namespace geoml
