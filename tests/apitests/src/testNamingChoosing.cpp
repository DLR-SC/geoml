#include <geoml/naming_choosing/Shape.h>
#include <geoml/surfaces/surfaces.h>
#include <geoml/data_structures/Array2d.h>
#include <geoml/geom_topo_conversions/geom_topo_conversions.h>
#include <geoml/primitives/modeling.hpp>
#include <geoml/boolean_ops/modeling.hpp>
#include <geoml/predicates/predicate_functions.h>

#include <gtest/gtest.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include "BRepTools.hxx"
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS.hxx>

TEST(SelectSubShapes, BooleanCutCheckNumberOfSubShapes)
{   
    auto box = geoml::create_box(1., 1., 1.);
    auto cylinder_big = geoml::create_cylinder(0.5, 0.5);
    auto cylinder_small = geoml::create_cylinder(0.25, 0.75);

    auto result = box - cylinder_big - cylinder_small;

    // Get an edge from the box. Selecting first one here.
    auto box_edges = box.select_subshapes(geoml::is_edge);
    
    EXPECT_EQ(box_edges.size(), 12);

    auto const& edge_in = box_edges.front();

    // Get edges in result that originate from the edge selected in box
    auto result_edges = result.select_subshapes(
        geoml::is_edge &&                 // any edge,
        geoml::is_descendent_of(edge_in)  // created from edge_in thru any number of operations
    );

    EXPECT_EQ(result_edges.size(), 1);

    // Get a face from the cylinder. Selecting first one here.
    auto cyl_faces = cylinder_big.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_FACE); });

    EXPECT_EQ(cyl_faces.size(), 3);

    auto const& face_in = cyl_faces[1];

    // Get face in result that originate from the face selected in cylinder
    auto result_faces = result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_FACE) &&               // any edge,
               s.is_descendent_of(face_in);            // created from edge_in thru any number of operations
    });

    EXPECT_EQ(result_faces.size(), 1);

    // Get all faces in result that originate from any subshape contained in the two cylinders
    auto result_faces_from_cylinders = result.select_subshapes([&](geoml::Shape const& s){
        return s.is_type(TopAbs_FACE) && (s.is_descendent_of_subshape_in(cylinder_small) || s.is_descendent_of_subshape_in(cylinder_big) );
    });

    EXPECT_EQ(result_faces_from_cylinders.size(), 4);

}

class RectangularFace : public testing::Test {

    /*
        Create a rectangular face

        Vertices: alpha, beta, gamma, delta
        edges: a,b,c,d 

          delta    c   gamma 
                +----+
                |    |
             d  |    |  b
                +----+
          alpha    a   beta 
    */

public:
    void SetUp() override {
        using namespace geoml;

        // construct a swept face from the edge a connecting alpha and beta
        auto alpha_v = BRepBuilderAPI_MakeVertex(gp_Pnt(0., 0., 0.));
        auto beta_v = BRepBuilderAPI_MakeVertex(gp_Pnt(4., 0., 0.));
        auto a_e = BRepBuilderAPI_MakeEdge(alpha_v, beta_v);
        face = BRepPrimAPI_MakePrism(a_e, gp_Vec(0., 4., 0.)).Shape();

        // extract vertices and edges
        alpha = face.filter(is_vertex && is_same(alpha_v));
        beta = face.filter(is_vertex && is_same(beta_v));    
        a = face.filter(is_edge && is_same(a_e));

        b = face.filter(is_edge && !is_same(a) && has_subshape(beta));
        d = face.filter(is_edge && !is_same(a) && has_subshape(alpha));
    
        gamma = b.filter(is_vertex && !is_same(beta));
        delta = d.filter(is_vertex && !is_same(alpha));

        c = face.filter(is_edge && has_subshape(gamma) && has_subshape(delta));
        
        ASSERT_EQ(alpha.size(), 1);
        ASSERT_EQ(TopoDS_Shape(alpha).ShapeType(), TopAbs_VERTEX);
        ASSERT_EQ(beta.size(), 1); 
        ASSERT_EQ(TopoDS_Shape(beta).ShapeType(), TopAbs_VERTEX);
        ASSERT_EQ(gamma.size(), 1); 
        ASSERT_EQ(TopoDS_Shape(gamma).ShapeType(), TopAbs_VERTEX);
        ASSERT_EQ(delta.size(), 1); 
        ASSERT_EQ(TopoDS_Shape(delta).ShapeType(), TopAbs_VERTEX);

        ASSERT_EQ(a.size(), 1); 
        ASSERT_EQ(TopoDS_Shape(a).ShapeType(), TopAbs_EDGE);
        ASSERT_EQ(b.size(), 1);
        ASSERT_EQ(TopoDS_Shape(b).ShapeType(), TopAbs_EDGE);
        ASSERT_EQ(c.size(), 1);
        ASSERT_EQ(TopoDS_Shape(c).ShapeType(), TopAbs_EDGE);
        ASSERT_EQ(d.size(), 1);
        ASSERT_EQ(TopoDS_Shape(d).ShapeType(), TopAbs_EDGE);
    }

protected:
    // vertices
    geoml::Shape alpha;
    geoml::Shape beta;
    geoml::Shape gamma;
    geoml::Shape delta;

    // relevant edges (don't care about c)
    geoml::Shape a;
    geoml::Shape b;
    geoml::Shape c;
    geoml::Shape d;

    // the face
    geoml::Shape face;
};

namespace {

    geoml::Shape make_tool(double design_parameter) {
        // create a triangular surface
        gp_Pnt p1(5.0 - design_parameter, -1.0, 0.0); 
        gp_Pnt p2(7.0 - design_parameter, -1.0, 0.0); 
        gp_Pnt p3(6.0 - design_parameter, 1.0, 0.0); 

        // create a polygon from these points
        BRepBuilderAPI_MakePolygon polygon;
        polygon.Add(p1);
        polygon.Add(p2);
        polygon.Add(p3);
        polygon.Close(); 

        // create a face from the polygon
        TopoDS_Face triangle_face = BRepBuilderAPI_MakeFace(polygon.Wire());

        return triangle_face;
    }

} // anonymous namespace


TEST_F(RectangularFace, example_rectangle_triangles)
{   
    using namespace geoml;

    geoml::Shape triangular_srf = make_tool(3.);
    geoml::Shape triangular_srf_2 = make_tool(5.);

    // perform the modelling steps
    geoml::Shape cut_result = face - triangular_srf;
    geoml::Shape cut_result_2 = cut_result - triangular_srf_2; 

    // Select edges from the triangular_srf. 
    auto triangular_srf_edges = triangular_srf.select_subshapes(is_edge);

    EXPECT_EQ(triangular_srf_edges.size(), 3);

    auto const&  C = triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = triangular_srf_edges.at(2); // "pick" edge

    // Get edges in result that originate from edge a and have a vertex that originates from alpha
    auto cut_result_edges_a_alpha = cut_result.select_subshapes(
        is_edge && 
        is_descendent_of(a) && 
        has_subshape_that(is_descendent_of(alpha))
    );

    EXPECT_EQ(cut_result_edges_a_alpha.size(), 1);

    // Get edges in result that originate from edge a or an edge in triangular_srf
    auto cut_result_edges_a_or_tool = cut_result.select_subshapes(
        is_edge &&               
        ( is_descendent_of(a) || 
          is_descendent_of_subshape_in(triangular_srf) )
    );

    EXPECT_EQ(cut_result_edges_a_or_tool.size(), 4);

    // test choosing after two modelling steps:

    // Get edges in result that originate from edge a and have a vertex originating from alpha or originates from B:
    auto cut_result_2_edges_a_alpha_B = cut_result_2.select_subshapes(
        (is_edge && is_descendent_of(a) && has_subshape_that(is_descendent_of(alpha)) ) || 
        is_descendent_of(B)
    );


    EXPECT_EQ(cut_result_2_edges_a_alpha_B.size(), 2);

    // test horizontal tags:

    // define criterion_2
    auto criterion_2 = 
        is_edge &&
        is_descendent_of(a) &&
        !has_subshape_that(is_descendent_of(alpha));

    // define tag 
    std::string tag_2 ("tag_2");

    // add tag to subshapes that fulfill criterion_2
    cut_result.add_meta_tag_to_subshapes(criterion_2, tag_2);

    // Select edges in cut_result carrying tag_2
    auto cut_result_edges_tag_2 = cut_result.select_subshapes(has_tag(tag_2));

    EXPECT_EQ(cut_result_edges_tag_2.size(), 1);

    // test tag tracks:
    
    // define criterion_1
    auto criterion_1 = is_edge && ( is_descendent_of(A) || is_descendent_of(B));

    std::string tag_1 = "persistent name 1";

    int remainingSteps_1 = 2;

    // define tag_track_1
    TagTrack tag_track_1(tag_1, criterion_1, remainingSteps_1);

    // get edges in cut_result carrying tag_1
    auto cut_result_edges_tag_1 = cut_result.select_subshapes(has_tag(tag_track_1.m_tag));

    EXPECT_EQ(cut_result_edges_tag_1.size(), 0);
}

TEST_F(RectangularFace, test_tag_tracks_with_operations)
{   
    using namespace geoml;

    geoml::Shape triangular_srf = make_tool(3.);
    geoml::Shape triangular_srf_2 = make_tool(5.);

    // Select edges from the triangular_srf. 
    auto triangular_srf_edges = triangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    EXPECT_EQ(triangular_srf_edges .size(), 3);

    auto const&  C = triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = triangular_srf_edges.at(2); // "pick" edge

    // add a tag track to face
    auto criterion = 
        is_edge &&
        is_descendent_of(a) &&
        has_subshape_that(is_descendent_of(alpha));

    std::string tag = "vertically persistent name";
    int remainingSteps = 2;

    geoml::TagTrack tag_track(tag, criterion, remainingSteps);

    face.add_tag_track(tag_track);

    // apply the tag tracks of rectangular_srf to the Shape itself
    face.apply_tag_tracks();

    // get edges in rectuangular_srf carrying tag "vertically persisten name"
    auto rectangular_srf_tagged_edges = face.select_subshapes(geoml::has_tag(tag_track.m_tag));

    EXPECT_EQ(face.select_subshapes(criterion).size(), 1);
    EXPECT_EQ(rectangular_srf_tagged_edges.size(), 1);

    // perform first modelling operation
    geoml::Shape cut_result = face - triangular_srf;

    // get edges in cut_result carrying tag "vertically persisten name"
    auto cut_result_tagged_edges = cut_result.select_subshapes(geoml::has_tag(tag_track.m_tag));

    EXPECT_EQ(cut_result_tagged_edges .size(), 1);

    // perform second modelling operation
    geoml::Shape cut_result_2 = cut_result - triangular_srf_2;

    // get edges in cut_result_2 carrying tag "vertically persisten name"
    auto cut_result_2_tagged_edges = cut_result_2.select_subshapes(geoml::has_tag(tag_track.m_tag));

    EXPECT_EQ(cut_result_2_tagged_edges.size(), 1);

}

TEST_F(RectangularFace, test_shape_predicates)
{   
    // define a predicate
    geoml::ShapePredicate pred = geoml::is_edge;

    // get edges in rectuangular_srf 
    auto rectangular_srf_edges = face.select_subshapes(pred);

    EXPECT_EQ(rectangular_srf_edges.size(), 4);

    face.add_meta_tag_to_subshapes(pred, "edge_tag");

    auto rectangular_srf_edges_with_tag = face.select_subshapes(geoml::has_tag("edge_tag"));    

    EXPECT_EQ(rectangular_srf_edges_with_tag.size(), 4);
}


struct PredicateFilter
 : public RectangularFace
 , public testing::WithParamInterface<std::string> 
{
public:
    void SetUp() override
    {
        RectangularFace::SetUp();

        auto alpha_p = BRep_Tool::Pnt(TopoDS::Vertex(alpha)).Coord() - gp_Pnt(0.5, 0.5, 0.5).Coord();
        auto beta_p = BRep_Tool::Pnt(TopoDS::Vertex(beta)).Coord() - gp_Pnt(0.5, 0.5, 0.5).Coord();
        auto gamma_p = BRep_Tool::Pnt(TopoDS::Vertex(gamma)).Coord() - gp_Pnt(0.5, 0.5, 0.5).Coord();
        auto delta_p = BRep_Tool::Pnt(TopoDS::Vertex(delta)).Coord() - gp_Pnt(0.5, 0.5, 0.5).Coord();

        auto a_p = 0.5*(alpha_p + beta_p);
        auto b_p = 0.5*(beta_p + gamma_p);
        auto c_p = 0.5*(gamma_p + delta_p);
        auto d_p = 0.5*(delta_p + alpha_p);

        tool_origin = {
            {"alpha"  , alpha_p         }, // tool cuts away alpha
            {"beta"   , beta_p          }, // tool cuts away beta
            {"gamma"  , gamma_p         }, // tool cuts away gamma
            {"delta"  , delta_p         }, // tool cuts away delta
            {"a"      , a_p             }, // tool splits a
            {"b"      , b_p             }, // tool splits b
            {"c"      , c_p             }, // tool splits c
            {"d"      , d_p             }, // tool splits d
            {"inside" , 0.5*(a_p + c_p) }, // tool splits no edge but cuts a hole in face
            {"outside", 100*gamma_p     }  // tool doesn't cut face at alll
        };
    }

    geoml::Shape create_tool(std::string const& s) {
        return BRepPrimAPI_MakeBox(tool_origin[s], 1., 1., 1.).Shape();
    }

private:

    std::unordered_map<std::string, gp_Pnt> tool_origin;

};



TEST_P(PredicateFilter, split_edge_modeling_intent)
{
    using namespace geoml;

    auto design_parameter = GetParam();
    auto tool = create_tool(design_parameter);
    auto face_cut = face - tool;

    /*********************************
     * select edge of cutting result *
     *********************************/

    /* modeling intent:

       edge1:
       Always choose the edge that is a descendent of a and shares a vertex with alpha
       If this does not exist, just choose the edge that is a descendent of a
     */

    auto m = face_cut.filter(is_edge && is_descendent_of(a));
    auto n = m.filter(is_edge && has_subshape(alpha));
    auto edge1 = n.is_empty()? m : n;
    EXPECT_EQ(edge1.size(), 1);
    EXPECT_EQ(TopoDS_Shape(edge1).ShapeType(), TopAbs_EDGE);

    /*
       edge2:
       Always choose the edge that is a descendent of b and has a common vertex 
       with edge1. If this does not exist, choose the edge that has a common vertex
       with edge1 and is not a descendent of d
    */ 
    auto has_common_vertex_with = [](Shape const& other) {
        return has_subshape_that(is_vertex && is_subshape_of(other)) &&
               !is_same(other);
    };

    auto p = face_cut.filter(
        is_edge && 
        has_common_vertex_with(edge1) &&
        !is_descendent_of(d)
    );
    auto q = p.filter(is_descendent_of(b));
    auto edge2 = q.is_empty() ? p : q;
    EXPECT_EQ(edge2.size(), 1);
    EXPECT_EQ(TopoDS_Shape(edge2).ShapeType(), TopAbs_EDGE);

    /************************************************************************
     * Assert that the edge selection really corresponds to modeling intent *
     ************************************************************************/

    // In every case, edge1 and edge2 should share a vertex.
    // Let v1 and v2 be the indices of edge1 and v2 and v3 be the indices of edge2
    auto v1 = face_cut.filter(is_vertex && is_subshape_of(edge1) && !is_subshape_of(edge2));
    EXPECT_EQ(v1.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v1).ShapeType(), TopAbs_VERTEX);
    auto v1_p = BRep_Tool::Pnt(TopoDS::Vertex(v1));

    auto v2 = face_cut.filter(is_vertex && is_subshape_of(edge1) && is_subshape_of(edge2));
    EXPECT_EQ(v2.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v2).ShapeType(), TopAbs_VERTEX);
    auto v2_p = BRep_Tool::Pnt(TopoDS::Vertex(v2));

    auto v3 = face_cut.filter(is_vertex && !is_subshape_of(edge1) && is_subshape_of(edge2));
    EXPECT_EQ(v3.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v2).ShapeType(), TopAbs_VERTEX);
    auto v3_p = BRep_Tool::Pnt(TopoDS::Vertex(v3));

    auto alpha_p = BRep_Tool::Pnt(TopoDS::Vertex(alpha));
    auto beta_p = BRep_Tool::Pnt(TopoDS::Vertex(beta));
    auto gamma_p = BRep_Tool::Pnt(TopoDS::Vertex(gamma));

    if (design_parameter == "alpha") {
        // EXPECT_TRUE(v1_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gamma_p) < 1e-8);
    };

    if (design_parameter == "beta") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        // EXPECT_TRUE(v2_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
        // EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
    };

    if (design_parameter == "gamma") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        // EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
    };

    if (
        design_parameter == "delta" || 
        design_parameter == "c" || 
        design_parameter == "d" || 
        design_parameter == "inside" || 
        design_parameter == "outside"
    ) {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gamma_p) < 1e-8);
    };

    if (design_parameter == "a") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        // EXPECT_TRUE(v2_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
        // EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
    }

    if (design_parameter == "b") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        // EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(X,X,X)) < 1e-8);
    }

}


INSTANTIATE_TEST_SUITE_P(
    TopologyChangingParameters,
    PredicateFilter,
    testing::Values(
        "alpha", "beta", "gamma", "delta", "a", "b", "c", "d", "inside", "outside"
    )
);
