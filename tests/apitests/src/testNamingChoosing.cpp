#include <geoml/naming_choosing/Shape.h>
#include <geoml/modeling/ModelingContext.h>
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

TEST(Shape, add_persistent_meta_tag_to_subshapes)
{
    using namespace geoml;
    auto box = create_box(1., 1., 1.);
    add_persistent_meta_tag_to_subshapes(box, is_vertex, "vertex");
    auto not_vertices = box.select_subshapes(!is_vertex);
    for (auto const& f : not_vertices) {
        ASSERT_FALSE(f.has_tag("vertex"));
    }
    auto vertices = box.select_subshapes(is_vertex);
    for (auto const& f : vertices) {
        ASSERT_TRUE(f.has_tag("vertex"));
    }
}

// Currently, an edge is picked via an index, which is dependant of a hash function, which is used in the context of ShapeContainers, which is a typedef 
// of std::unordered_set. For hash is calculated with ShapeHasher, which uses, amongs other inputs, the address of the TShape instance of the 
// underlying TopoDS_Shape instance. Each execution of the tests may lead to different addresses allocated by the operational system (depending
// on which recources are currently available, also due to potentially other open applications), and hence to different hashes which again might
// lead to different index distributions, which explains the flakyness of this edge selection
TEST(SelectSubShapes, DISABLED_BooleanCutCheckNumberOfSubShapes)
{   
    using namespace geoml;
    auto box = create_box(1., 1., 1.);
    auto cylinder_big = create_cylinder(0.5, 0.5);
    auto cylinder_small = create_cylinder(0.25, 0.75);

    auto result = box - cylinder_big - cylinder_small;

    // Get an edge from the box. Selecting first one here.
    auto box_edges = box.select_subshapes(is_edge);
    
    EXPECT_EQ(box_edges.size(), 12);

    auto const& edge_in = box_edges[0];

    // Get edges in result that originate from the edge selected in box
    auto result_edges = result.select_subshapes(
        is_edge &&                 // any edge,
        is_descendent_of(edge_in)  // created from edge_in thru any number of operations
    );

    EXPECT_EQ(result_edges.size(), 1);

    // Get a face from the cylinder. Selecting first one here.
    auto cyl_faces = cylinder_big.select_subshapes(is_face);

    EXPECT_EQ(cyl_faces.size(), 3);

    auto face_in = cyl_faces[1];

    // Get face in result that originate from the face selected in cylinder
    auto result_faces = result.select_subshapes(
        is_face && is_descendent_of(face_in)
    );

    EXPECT_EQ(result_faces.size(), 1);

    // Get all faces in result that originate from any subshape contained in the two cylinders
    auto result_faces_from_cylinders = result.select_subshapes(
        is_face && 
        (is_descendent_of_subshape_in(cylinder_small) || is_descendent_of_subshape_in(cylinder_big) )
    );

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
        alpha = face.select_subshapes(is_vertex && is_same(alpha_v)).unique_element();
        beta = face.select_subshapes(is_vertex && is_same(beta_v)).unique_element();    
        a = face.select_subshapes(is_edge && is_same(a_e)).unique_element();

        b = face.select_subshapes(is_edge && !is_same(a) && has_subshape(beta)).unique_element();
        d = face.select_subshapes(is_edge && !is_same(a) && has_subshape(alpha)).unique_element();
    
        gamma = b.select_subshapes(is_vertex && !is_same(beta)).unique_element();
        delta = d.select_subshapes(is_vertex && !is_same(alpha)).unique_element();

        c = face.select_subshapes(is_edge && has_subshape(gamma) && has_subshape(delta)).unique_element();
        
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

    // relevant edges
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

// Currently, an edge is picked via an index, which is dependant of a hash function, which is used in the context of ShapeContainers, which is a typedef 
// of std::unordered_set. For hash is calculated with ShapeHasher, which uses, amongs other inputs, the address of the TShape instance of the 
// underlying TopoDS_Shape instance. Each execution of the tests may lead to different addresses allocated by the operational system (depending
// on which recources are currently available, also due to potentially other open applications), and hence to different hashes which again might
// lead to different index distributions, which explains the flakyness of this edge selection 
TEST_F(RectangularFace, DISABLED_example_rectangle_triangles)
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

    auto const&  C = triangular_srf_edges[0]; // "pick" edge
    auto const&  B = triangular_srf_edges[1]; // "pick" edge
    auto const&  A = triangular_srf_edges[2]; // "pick" edge

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

    // get edges in cut_result carrying tag_1
    auto cut_result_edges_tag_1 = cut_result.select_subshapes(has_tag(tag_1));

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

    auto const&  C = triangular_srf_edges[0]; // "pick" edge
    auto const&  B = triangular_srf_edges[1]; // "pick" edge
    auto const&  A = triangular_srf_edges[2]; // "pick" edge

    // add a tag track to face
    auto criterion = 
        is_edge &&
        is_descendent_of(a) &&
        has_subshape_that(is_descendent_of(alpha));

    std::string tag = "vertically persistent name";
    int remainingSteps = 2;

    geoml::ModelingContext context;
    context.add_tag_track(face, criterion, tag, geoml::TrackLifetime::steps(remainingSteps));

    // get edges in rectuangular_srf carrying tag "vertically persisten name"
    auto rectangular_srf_tagged_edges = face.select_subshapes(geoml::has_tag(tag));

    EXPECT_EQ(face.select_subshapes(criterion).size(), 1);
    EXPECT_EQ(rectangular_srf_tagged_edges.size(), 1);

    // perform first modelling operation
    geoml::Shape cut_result = boolean_subtract(face, triangular_srf, context);

    // get edges in cut_result carrying tag "vertically persisten name"
    auto cut_result_tagged_edges = cut_result.select_subshapes(geoml::has_tag(tag));

    EXPECT_EQ(cut_result_tagged_edges .size(), 1);

    // perform second modelling operation
    geoml::Shape cut_result_2 = boolean_subtract(cut_result, triangular_srf_2, context);

    // get edges in cut_result_2 carrying tag "vertically persisten name"
    auto cut_result_2_tagged_edges = cut_result_2.select_subshapes(geoml::has_tag(tag));

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


struct PredicateSelection
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



TEST_P(PredicateSelection, split_edge_modeling_intent)
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

    auto m = face_cut.select_subshapes(is_edge && is_descendent_of(a));
    auto n = m.filter(has_subshape(alpha));
    auto edge1 = (n.is_empty()? m : n).unique_element();
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

    auto p = face_cut.select_subshapes(
        is_edge && 
        has_common_vertex_with(edge1) &&
        !is_descendent_of(d)
    );
    auto q = p.filter(is_descendent_of(b));
    auto edge2 = (q.is_empty()? p : q).unique_element();
    EXPECT_EQ(edge2.size(), 1);
    EXPECT_EQ(TopoDS_Shape(edge2).ShapeType(), TopAbs_EDGE);

    /************************************************************************
     * Assert that the edge selection really corresponds to modeling intent *
     ************************************************************************/

    // In every case, edge1 and edge2 should share a vertex.
    // Let v1 and v2 be the vertices of edge1 and v2 and v3 be the vertices of edge2
    auto v1 = face_cut.select_subshapes(is_vertex && is_subshape_of(edge1) && !is_subshape_of(edge2)).unique_element();
    EXPECT_EQ(v1.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v1).ShapeType(), TopAbs_VERTEX);
    auto v1_p = BRep_Tool::Pnt(TopoDS::Vertex(v1));

    auto v2 = face_cut.select_subshapes(is_vertex && is_subshape_of(edge1) && is_subshape_of(edge2)).unique_element();
    EXPECT_EQ(v2.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v2).ShapeType(), TopAbs_VERTEX);
    auto v2_p = BRep_Tool::Pnt(TopoDS::Vertex(v2));

    auto v3 = face_cut.select_subshapes(is_vertex && !is_subshape_of(edge1) && is_subshape_of(edge2)).unique_element();
    EXPECT_EQ(v3.size(), 1);
    EXPECT_EQ(TopoDS_Shape(v2).ShapeType(), TopAbs_VERTEX);
    auto v3_p = BRep_Tool::Pnt(TopoDS::Vertex(v3));

    auto alpha_p = BRep_Tool::Pnt(TopoDS::Vertex(alpha));
    auto beta_p = BRep_Tool::Pnt(TopoDS::Vertex(beta));
    auto gamma_p = BRep_Tool::Pnt(TopoDS::Vertex(gamma));

    if (design_parameter == "alpha") {
        EXPECT_TRUE(v1_p.SquareDistance(gp_Pnt(alpha_p.X() + 0.5, 0., 0.)) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gamma_p) < 1e-8);
    };

    if (design_parameter == "beta") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(gp_Pnt(beta_p.X() - 0.5, 0., 0.)) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(beta_p.X() - 0.5, beta_p.Y() + 0.5, 0.)) < 1e-8);
    };

    if (design_parameter == "gamma") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(gamma_p.X(),gamma_p.Y() - 0.5, 0.)) < 1e-8);
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
        EXPECT_TRUE(v2_p.SquareDistance(gp_Pnt(alpha_p.X() + 1.5, 0., 0.)) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(alpha_p.X() + 1.5, alpha_p.Y() + 0.5, 0.)) < 1e-8);
    }

    if (design_parameter == "b") {
        EXPECT_TRUE(v1_p.SquareDistance(alpha_p) < 1e-8);
        EXPECT_TRUE(v2_p.SquareDistance(beta_p) < 1e-8);
        EXPECT_TRUE(v3_p.SquareDistance(gp_Pnt(beta_p.X(), beta_p.Y() + 1.5, 0.)) < 1e-8);
    }

}


INSTANTIATE_TEST_SUITE_P(
    TopologyChangingParameters,
    PredicateSelection,
    testing::Values(
        "alpha", "beta", "gamma", "delta", "a", "b", "c", "d", "inside", "outside"
    )
);

/**
 * Verifies that a modified edge shared by two adjacent faces reports
 * consistent history regardless of which parent face is used to select it.
 *
 * Subtracts a corner box [0,1]^3 from [0,2]^3 so that the three box edges
 * meeting at the origin are shortened (modified). Selects one such resulting
 * edge via both of its adjacent faces and checks that both handles satisfy
 * is_same(), has_origin(), is_descendent_of(), and is_modified_descendent_of().
 */
TEST(ShapeHistory, shared_edge_history)
{
    using namespace geoml;

    Shape box = create_box(2.0, 2.0, 2.0);
    Shape cutter = create_box(1.0, 1.0, 1.0);
    Shape result = boolean_subtract(box, cutter);

    // Find an original box edge that was modified into a single resulting edge.
    Shape orig_edge;
    Shape expected_edge;
    for (auto const& candidate : box.select_subshapes(is_edge)) {
        auto descendents = result.select_subshapes(is_edge && is_modified_descendent_of(candidate));
        if (descendents.size() == 1) {
            orig_edge = candidate;
            expected_edge = descendents.unique_element();
            break;
        }
    }

    ASSERT_FALSE(orig_edge.is_null());
    ASSERT_FALSE(expected_edge.is_null());
    ASSERT_FALSE(expected_edge.is_same(orig_edge));
    ASSERT_TRUE(expected_edge.has_origin());
    ASSERT_TRUE(expected_edge.is_descendent_of(orig_edge));

    // Select the two adjacent faces sharing this resulting edge.
    Shape adjacent_faces = result.select_subshapes(is_face && has_subshape(expected_edge));
    ASSERT_EQ(adjacent_faces.size(), 2u);

    Shape edge_from_face1 = adjacent_faces[0].select_subshapes(is_edge && is_same(expected_edge)).unique_element();
    Shape edge_from_face2 = adjacent_faces[1].select_subshapes(is_edge && is_same(expected_edge)).unique_element();

    ASSERT_TRUE(edge_from_face1.is_same(edge_from_face2));

    // Both topological handles to the same edge must report the same history.
    EXPECT_TRUE(edge_from_face1.has_origin());
    EXPECT_TRUE(edge_from_face2.has_origin());
    EXPECT_TRUE(edge_from_face1.is_descendent_of(orig_edge));
    EXPECT_TRUE(edge_from_face2.is_descendent_of(orig_edge));
    EXPECT_TRUE(edge_from_face1.is_modified_descendent_of(orig_edge));
    EXPECT_TRUE(edge_from_face2.is_modified_descendent_of(orig_edge));
}

/**
 * Verifies that multi-step history remains consistent across adjacent
 * faces when a shared edge is modified in step 1 and left unmodified in step 2.
 *
 * Step 1 subtracts [0,1]^3 from [0,2]^3 (modifying the edges at the origin);
 * step 2 subtracts [1.5,2.5]^3 at the opposite corner, leaving those edges
 * unmodified. Checks that both face-selected handles retain their unmodified
 * origin link into step 1 and trace back to the original box edge.
 */
TEST(ShapeHistory, shared_edge_multistep_history)
{
    using namespace geoml;

    // Step 1: subtract [0,1]^3 from [0,2]^3 -> shortens the 3 edges meeting at (0,0,0).
    Shape box = create_box(2.0, 2.0, 2.0);
    Shape cutter1 = create_box(1.0, 1.0, 1.0);
    Shape step1 = boolean_subtract(box, cutter1);

    // Step 2: subtract [1.5,2.5]^3 at the opposite corner (2,2,2) -> leaves the
    // edges modified in step 1 unmodified in step 2.
    Shape cutter2(BRepPrimAPI_MakeBox(gp_Pnt(1.5, 1.5, 1.5), 1.0, 1.0, 1.0).Shape());
    Shape step2 = boolean_subtract(step1, cutter2);

    Shape orig_edge;
    Shape step1_edge;
    for (auto const& candidate : box.select_subshapes(is_edge)) {
        auto descendents = step1.select_subshapes(is_edge && is_modified_descendent_of(candidate));
        if (descendents.size() == 1) {
            orig_edge = candidate;
            step1_edge = descendents.unique_element();
            break;
        }
    }

    ASSERT_FALSE(orig_edge.is_null());
    ASSERT_FALSE(step1_edge.is_null());

    // Confirm this edge survived step 2 unmodified (still same OCCT edge as step1_edge).
    Shape step2_edge = step2.select_subshapes(is_edge && is_same(step1_edge)).unique_element();
    ASSERT_FALSE(step2_edge.is_same(orig_edge));

    Shape adjacent_faces = step2.select_subshapes(is_face && has_subshape(step2_edge));
    ASSERT_EQ(adjacent_faces.size(), 2u);

    Shape edge_from_face1 = adjacent_faces[0].select_subshapes(is_edge && is_same(step2_edge)).unique_element();
    Shape edge_from_face2 = adjacent_faces[1].select_subshapes(is_edge && is_same(step2_edge)).unique_element();

    ASSERT_TRUE(edge_from_face1.is_same(edge_from_face2));

    // Even though the edge was unmodified in step 2, both occurrences must keep
    // their origin link into step 1 and therefore trace back to orig_edge.
    EXPECT_TRUE(edge_from_face1.has_origin());
    EXPECT_TRUE(edge_from_face2.has_origin());
    EXPECT_TRUE(edge_from_face1.is_unmodified_descendent_of(step1_edge));
    EXPECT_TRUE(edge_from_face2.is_unmodified_descendent_of(step1_edge));
    EXPECT_TRUE(edge_from_face1.is_descendent_of(orig_edge));
    EXPECT_TRUE(edge_from_face2.is_descendent_of(orig_edge));
}

/**
 * Verifies that tags applied via add_meta_tag_to_subshapes(),
 * add_persistent_meta_tag_to_subshapes(), and ModelingContext tag tracks are
 * attached consistently to all occurrences of a shared edge without duplicates.
 *
 * Checks both direct tagging on the original box and ModelingContext track
 * propagation across boolean_subtract(), selecting the shared edge through
 * both adjacent faces in each case.
 */
TEST(ShapeHistory, shared_edge_tags_and_tag_tracks)
{
    using namespace geoml;

    Shape box = create_box(2.0, 2.0, 2.0);
    Shape cutter = create_box(1.0, 1.0, 1.0);

    // Pick a box edge that will be modified by cutting [0,1]^3.
    Shape probe = boolean_subtract(box, cutter);
    Shape orig_edge;
    for (auto const& candidate : box.select_subshapes(is_edge)) {
        if (probe.select_subshapes(is_edge && is_modified_descendent_of(candidate)).size() == 1) {
            orig_edge = candidate;
            break;
        }
    }
    ASSERT_FALSE(orig_edge.is_null());

    // Direct tagging on box via member and free function.
    box.add_meta_tag_to_subshapes(is_edge && is_same(orig_edge), "member_tag");
    add_persistent_meta_tag_to_subshapes(box, is_edge && is_same(orig_edge), "free_tag");

    Shape box_faces = box.select_subshapes(is_face && has_subshape(orig_edge));
    ASSERT_EQ(box_faces.size(), 2u);
    Shape box_e1 = box_faces[0].select_subshapes(is_edge && is_same(orig_edge)).unique_element();
    Shape box_e2 = box_faces[1].select_subshapes(is_edge && is_same(orig_edge)).unique_element();

    EXPECT_TRUE(box_e1.has_tag("member_tag"));
    EXPECT_TRUE(box_e2.has_tag("member_tag"));
    EXPECT_TRUE(box_e1.has_tag("free_tag"));
    EXPECT_TRUE(box_e2.has_tag("free_tag"));

    // ModelingContext tag track applied immediately and during boolean_subtract.
    ModelingContext context;
    context.add_tag_track(box, is_edge && is_descendent_of(orig_edge), "tracked_edge",
                          TrackLifetime::steps(2));
    EXPECT_TRUE(box_e1.has_tag("tracked_edge"));
    EXPECT_TRUE(box_e2.has_tag("tracked_edge"));

    Shape result = boolean_subtract(box, cutter, context);

    Shape expected_edge = result.select_subshapes(is_edge && is_modified_descendent_of(orig_edge)).unique_element();
    Shape result_faces = result.select_subshapes(is_face && has_subshape(expected_edge));
    ASSERT_EQ(result_faces.size(), 2u);

    Shape res_e1 = result_faces[0].select_subshapes(is_edge && is_same(expected_edge)).unique_element();
    Shape res_e2 = result_faces[1].select_subshapes(is_edge && is_same(expected_edge)).unique_element();

    EXPECT_TRUE(res_e1.has_tag("tracked_edge"));
    EXPECT_TRUE(res_e2.has_tag("tracked_edge"));
    EXPECT_TRUE(res_e1.is_modified_descendent_of(orig_edge));
    EXPECT_TRUE(res_e2.is_modified_descendent_of(orig_edge));
}

/**
 * Verifies that tagging subshapes evaluates the predicate across the entire
 * topology tree before mutating any tags, so state-dependent predicates such
 * as checking !root.has_tag(...) do not interfere with ongoing selection,
 * while still tagging all occurrences of shared subshapes.
 */
TEST(ShapeHistory, tagging_evaluates_predicate_before_applying_tags)
{
    using namespace geoml;

    // Member function: Shape::add_meta_tag_to_subshapes
    {
        Shape box = create_box(2.0, 2.0, 2.0);
        box.add_meta_tag_to_subshapes([&](Shape const&) { return !box.has_tag("review"); }, "review");

        EXPECT_TRUE(box.has_tag("review"));
        Shape tagged_edges = box.select_subshapes(is_edge && has_tag("review"));
        ASSERT_EQ(tagged_edges.size(), 12u);

        Shape sample_edge = tagged_edges[0];
        Shape adjacent_faces = box.select_subshapes(is_face && has_subshape(sample_edge));
        ASSERT_EQ(adjacent_faces.size(), 2u);
        Shape e1 = adjacent_faces[0].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
        Shape e2 = adjacent_faces[1].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
        EXPECT_TRUE(e1.has_tag("review"));
        EXPECT_TRUE(e2.has_tag("review"));
    }

    // Free function: add_persistent_meta_tag_to_subshapes
    {
        Shape box = create_box(2.0, 2.0, 2.0);
        add_persistent_meta_tag_to_subshapes(box, [&](Shape const&) { return !box.has_tag("review"); }, "review");

        EXPECT_TRUE(box.has_tag("review"));
        Shape tagged_edges = box.select_subshapes(is_edge && has_tag("review"));
        ASSERT_EQ(tagged_edges.size(), 12u);

        Shape sample_edge = tagged_edges[0];
        Shape adjacent_faces = box.select_subshapes(is_face && has_subshape(sample_edge));
        ASSERT_EQ(adjacent_faces.size(), 2u);
        Shape e1 = adjacent_faces[0].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
        Shape e2 = adjacent_faces[1].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
        EXPECT_TRUE(e1.has_tag("review"));
        EXPECT_TRUE(e2.has_tag("review"));
    }
}

/**
 * Verifies that ModelingContext evaluates tag track predicates across all
 * unique subshapes before mutating metadata (both on immediate registration
 * and during modeling propagation), while attaching matching tags consistently
 * to all topological occurrences of shared edges.
 */
TEST(ShapeHistory, modeling_context_evaluates_tracks_before_applying_to_shared_subshapes)
{
    using namespace geoml;

    Shape box = create_box(2.0, 2.0, 2.0);
    Shape cutter = create_box(1.0, 1.0, 1.0);

    ModelingContext context;

    // Immediate evaluation on registration must see pre-mutation state for the whole tree.
    context.add_tag_track(box, [&](Shape const&) { return !box.has_tag("initial"); }, "initial",
                          TrackLifetime::steps(1));
    EXPECT_TRUE(box.has_tag("initial"));
    Shape initial_edges = box.select_subshapes(is_edge && has_tag("initial"));
    ASSERT_EQ(initial_edges.size(), 12u);

    // During propagation, all incoming tracks evaluate against the post-history-mapping
    // result before any metadata updates are committed, so the second track observes
    // !shape.has_tag("ready") on every edge of the result.
    context.add_tag_track(box, is_edge, "ready", TrackLifetime::steps(1));
    context.add_tag_track(box, [](Shape const& shape) {
        return shape.is_type(TopAbs_EDGE) && !shape.has_tag("ready");
    }, "second", TrackLifetime::steps(1));

    // On immediate registration on box, "ready" was already applied before "second" was registered.
    EXPECT_TRUE(box.select_subshapes(is_edge && has_tag("second")).is_empty());

    Shape result = boolean_subtract(box, cutter, context);

    Shape result_edges = result.select_subshapes(is_edge);
    ASSERT_FALSE(result_edges.is_empty());
    EXPECT_EQ(result.select_subshapes(is_edge && has_tag("ready")).size(), result_edges.size());
    EXPECT_EQ(result.select_subshapes(is_edge && has_tag("second")).size(), result_edges.size());

    for (auto const& edge : result_edges) {
        Shape adjacent_faces = result.select_subshapes(is_face && has_subshape(edge));
        ASSERT_EQ(adjacent_faces.size(), 2u);
        Shape e1 = adjacent_faces[0].select_subshapes(is_edge && is_same(edge)).unique_element();
        Shape e2 = adjacent_faces[1].select_subshapes(is_edge && is_same(edge)).unique_element();
        EXPECT_TRUE(e1.has_tag("ready"));
        EXPECT_TRUE(e2.has_tag("ready"));
        EXPECT_TRUE(e1.has_tag("second"));
        EXPECT_TRUE(e2.has_tag("second"));
    }
}

/**
 * Verifies that tagging entry points invoke predicates with a Shape const&,
 * selecting the const-reference overload when a functor overloads on both
 * Shape const& and Shape&.
 */
TEST(ShapeHistory, tagging_invokes_predicate_with_const_shape_ref)
{
    using namespace geoml;

    struct ConstEdgePredicate
    {
        bool operator()(Shape const& s) const { return s.is_type(TopAbs_EDGE); }
        bool operator()(Shape&) const { return false; }
    };

    Shape box = create_box(2.0, 2.0, 2.0);
    ConstEdgePredicate const pred{};

    box.add_meta_tag_to_subshapes(pred, "const_member");
    add_persistent_meta_tag_to_subshapes(box, pred, "const_free");

    Shape member_tagged = box.select_subshapes(has_tag("const_member"));
    Shape free_tagged = box.select_subshapes(has_tag("const_free"));
    ASSERT_EQ(member_tagged.size(), 12u);
    ASSERT_EQ(free_tagged.size(), 12u);
    EXPECT_EQ(box.select_subshapes(is_edge && has_tag("const_member")).size(), 12u);
    EXPECT_EQ(box.select_subshapes(is_edge && has_tag("const_free")).size(), 12u);

    Shape sample_edge = member_tagged[0];
    Shape adjacent_faces = box.select_subshapes(is_face && has_subshape(sample_edge));
    ASSERT_EQ(adjacent_faces.size(), 2u);
    Shape e1 = adjacent_faces[0].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
    Shape e2 = adjacent_faces[1].select_subshapes(is_edge && is_same(sample_edge)).unique_element();
    EXPECT_TRUE(e1.has_tag("const_member"));
    EXPECT_TRUE(e2.has_tag("const_member"));
    EXPECT_TRUE(e1.has_tag("const_free"));
    EXPECT_TRUE(e2.has_tag("const_free"));
}
