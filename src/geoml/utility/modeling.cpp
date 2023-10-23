#include "utility/modeling.h"
#include <BRepFilletAPI_MakeFillet.hxx> 
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>

#include <iostream>


TColgp_Array2OfPnt
geoml::extract_control_points_surface(
    const Handle(Geom_BSplineSurface)& b_spline_surface)
{
    TColgp_Array2OfPnt control_points = b_spline_surface->Poles();

    return control_points; 
}
 

TColgp_Array1OfPnt 
geoml::extract_control_point_column_row (const Handle(Geom_BSplineSurface)& b_spline_surface, 
                                  int UV_direction, int index)
{
    //if ( UV_direction != 1 || UV_direction != 2)
        //throw error message
    // if( index < 1 || index > ...)
        //throw error message

    TColgp_Array2OfPnt cp_net = b_spline_surface->Poles();

    if (UV_direction == 1)
    {    
        TColgp_Array1OfPnt cp_column(1, cp_net.ColLength());
        
        for(int i = 1; i <= cp_net.ColLength(); ++i)
        {
            cp_column.SetValue(i, cp_net.Value(i, index));
        }

        return cp_column;

    } else if (UV_direction == 2)
    {
        TColgp_Array1OfPnt cp_column(1, cp_net.RowLength());
        
        for(int i = 1; i <= cp_net.RowLength(); ++i)
        {
            cp_column.SetValue(i, cp_net.Value(index, i));
        }

        return cp_column;
        
    }   
}                            

TColgp_Array1OfPnt 
geoml::move (const TColgp_Array1OfPnt& points, gp_Vec direction, double factor)
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
geoml::move (const gp_Pnt& point, gp_Vec direction, double factor)
{
        gp_Pnt moved_point( point.X() + factor * direction.X(),
                            point.Y() + factor * direction.Y(),
                            point.Z() + factor * direction.Z() );   

    return moved_point;
}

Handle(TColgp_HArray1OfPnt) 
geoml::move (const Handle(TColgp_HArray1OfPnt)& points, gp_Vec direction, double factor)
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

TColgp_Array2OfPnt 
geoml::create_point_net_from_arrays (const std::vector<TColgp_Array1OfPnt>& point_lists,
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
geoml::make_fillet (const TopoDS_Shape &solid_shape , int edge_index, double radius)
{
    BRepFilletAPI_MakeFillet MF (solid_shape);

    TopTools_IndexedMapOfShape edgeMap;
    TopExp::MapShapes(solid_shape, TopAbs_EDGE, edgeMap);

    MF.Add(radius, TopoDS::Edge(edgeMap(edge_index)));

    return MF.Shape();
}

TopoDS_Shape
geoml::make_fillet (const TopoDS_Shape &solid_shape , const std::initializer_list<int> &edge_indices, double radius)
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
geoml::scale_vector (const gp_Vec &vector , double factor)
{
    return vector.Multiplied(factor);
}

