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

TColgp_Array2OfPnt
extract_control_points_surface(const Handle(Geom_BSplineSurface)& b_spline_surface)
{
    TColgp_Array2OfPnt control_points = b_spline_surface->Poles();

    return control_points; 
}
 

TColgp_Array1OfPnt  
extract_control_point_column_row (const Handle(Geom_BSplineSurface)& b_spline_surface, 
                                                         int UV_direction, int index)
{
    //if ( UV_direction != 1 || UV_direction != 2)
        //throw error message
    // if( index < 1 || index > ...)
        //throw error message

    /*TColgp_Array2OfPnt cp_net = b_spline_surface->Poles();
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
    }*/

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

std::vector<gp_Pnt> 
move (const std::vector<gp_Pnt> & points, gp_Vec direction, double factor)
{
    //Handle(TColgp_HArray1OfPnt) my_array = new TColgp_HArray1OfPnt(1,points->Length());
    std::vector<gp_Pnt> point_vector;

    for (int i = 0; i < points.size(); ++i)
    {
        gp_Pnt moved_point( (points.at(i)).X() + factor * direction.X(),
                            (points.at(i)).Y() + factor * direction.Y(),
                            (points.at(i)).Z() + factor * direction.Z() );

        point_vector.push_back(moved_point);
    }   

    return point_vector;    
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

int writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName)
{
    STEPControl_Writer writer;
    writer.Transfer(my_shape,STEPControl_AsIs);
    writer.Write(fileName.c_str());

    return 0;
}

//////////////////////////////////////

TopoDS_Face make_face (const Handle_Geom_BSplineSurface & surface)
{
   TopoDS_Face face_tmp = BRepBuilderAPI_MakeFace(surface, 1e1); 
   return face_tmp;
}

int test_func (const std::vector< std::vector<gp_Pnt> > & control_points)
{
    int length_row = control_points.at(0).size();
    return length_row;
}

Handle(Geom_Curve) convert_to_geom_curve(const Handle(Geom_TrimmedCurve)& trimmed_curve)
{
    return trimmed_curve->BasisCurve();
}

Handle(Geom_BSplineCurve) geom_convert_from_trimmed_curve(const Handle(Geom_TrimmedCurve)& trimmed_curve)
{
    Handle(Geom_BSplineCurve) b_spline_curve = GeomConvert::CurveToBSplineCurve(trimmed_curve);
    return b_spline_curve;
}

gp_Pnt vector_of_points_at_index (const std::vector<gp_Pnt> & points, int index)
{
    return points.at(index);
}

std::vector<Handle(Geom_Curve)> vector_b_spline_to_geom_curve_converter (const std::vector<Handle(Geom_BSplineCurve)> input_vec)
{
    std::vector<Handle(Geom_Curve)> tmp_vec;

    for(int i = 0; i < input_vec.size(); ++i)
    {
        tmp_vec.push_back(input_vec.at(i));
    } 
    
    return tmp_vec;
}

Handle(Geom_Curve) convert_to_geom_curve(const Handle(Geom_BSplineCurve)& bspline_curve)
{
    Handle(Geom_Curve) tmp_curve;
    tmp_curve = bspline_curve;

    return tmp_curve;
}

Handle(Geom_Surface) convert_to_geom_surface(const Handle(Geom_BSplineSurface)& bspline_surface)
{
    Handle(Geom_Surface) tmp_surface;
    tmp_surface = bspline_surface;

    return tmp_surface;
}

TopoDS_Shape fuse_two_solids (const TopoDS_Solid & solid_1, const TopoDS_Solid & solid_2)
{
    BRepAlgoAPI_Fuse fuser (solid_1, solid_2);
    fuser.Build();

    return fuser.Shape();
}

TopoDS_Shape remove_faces_from_shape (const TopoDS_Shape & shape, const std::vector<int> & indices_of_faces_to_remove)
{
    TopoDS_Shape tmp_shape = shape;
    TopTools_IndexedMapOfShape face_map;
    TopExp::MapShapes(tmp_shape, TopAbs_FACE, face_map);
    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);

    for (int face_index: indices_of_faces_to_remove)
    {
         TopoDS_Face selected_face = TopoDS::Face(face_map(face_index)); 
         builder.Add(compound, selected_face);        
    }


    BRepAlgoAPI_Cut cutter(tmp_shape, compound); 

    return cutter.Shape();
}

TopoDS_Shape extrude_surface (const Handle(Geom_BSplineSurface) & surface, const gp_Vec & extrusion_vector)
{
    double tolerance {0.1};
    Handle(Geom_Surface) surface_geom;
    surface_geom = surface;
    TopoDS_Face surface_shape = BRepBuilderAPI_MakeFace(surface_geom, tolerance);

    BRepPrimAPI_MakePrism extrusion (surface_shape, extrusion_vector);

    TopoDS_Shape extruded_shape = extrusion.Shape();

    return extruded_shape;
}

TopoDS_Shape sphere (const gp_Pnt & center, double radius)
{
    BRepPrimAPI_MakeSphere sphere(center, radius);
    TopoDS_Shape sphere_shape = sphere.Shape();

    return sphere_shape;
}

TopoDS_Shape make_compound (const std::vector<TopoDS_Shape> & shapes)
{
    TopoDS_Compound compound;  
    BRep_Builder builder;
    builder.MakeCompound(compound);

    for (auto shape: shapes)
    {
        builder.Add(compound, shape);
    }

    return compound;
}

TopoDS_Shape cut_away ( const TopoDS_Shape & original_shape, const TopoDS_Shape & cutting_tool_shape)
{
    BRepAlgoAPI_Cut cutter(original_shape, cutting_tool_shape); 
    TopoDS_Shape cut_shape = cutter.Shape();

    return cut_shape;
}

TopoDS_Shell make_shell(TopoDS_Shape const& shape){
    BRep_Builder b;
    TopoDS_Shell s;
    b.MakeShell(s);
    TopExp_Explorer exp;
    for (exp.Init(shape, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face f = TopoDS::Face(exp.Current());
        b.Add(s, f);
    }
    return s;
}

TopoDS_Shell remove_faces(TopoDS_Shell const& shell, std::vector<TopoDS_Face> const& faces)
{
    BRep_Builder b;
    TopoDS_Shell s;
    b.MakeShell(s);
    TopExp_Explorer exp;
    for (exp.Init(shell, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face f = TopoDS::Face(exp.Current());
        auto it = std::find_if(
            faces.begin(), 
            faces.end(), 
            [&f](auto const& excl_face){ return excl_face.IsSame(f); }
        );
        if (it == faces.end()) {
            b.Add(s, f);
        }
    }
    return s;
}


} // namespace geoml
