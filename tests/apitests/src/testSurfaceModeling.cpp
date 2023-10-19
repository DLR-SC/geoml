#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"

#include <filesystem>
#include <iostream>

#include "STEPControl_Writer.hxx"


namespace apitests
{

std::vector<Handle(Geom_Curve)> read_curves(const std::string& brepFile, bool& ok)
{
    // first read the brep-input file
    TopoDS_Shape shape_u;
    BRep_Builder builder_u;

    ok = BRepTools::Read(shape_u, brepFile.c_str(), builder_u);
    if (!ok) return {};

    TopExp_Explorer Explorer;

    // get the splines in u-direction from the Edges
    std::vector<Handle(Geom_Curve)> curves;
    for (Explorer.Init(shape_u, TopAbs_EDGE); Explorer.More(); Explorer.Next()) {
        const auto& edge = TopoDS::Edge(Explorer.Current());
        double beginning = 0;
        double end = 1;
        curves.push_back(BRep_Tool::Curve(edge, beginning, end));
    }

    return curves;
}

} // apitests

class interpolate_curve_network: public ::testing::TestWithParam<std::string>
{
protected:
    void SetUp() override
    {
        // get the name of the folder with the B-spline network data
        std::string path_profiles = "../unittests/TestData/CurveNetworks/" + GetParam() + "/profiles.brep";
        std::string path_guides = "../unittests/TestData/CurveNetworks/" + GetParam() + "/guides.brep";

        std::filesystem::path output_dir("TestData/CurveNetworks/" + GetParam());
        if (!std::filesystem::is_directory(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }

        path_output = "TestData/CurveNetworks/" + GetParam() + "/result_gordon.brep";

        bool ok = false;
        ucurves = apitests::read_curves(path_profiles, ok);
        ASSERT_TRUE(ok);

        ok = false;
        vcurves = apitests::read_curves(path_guides, ok);
        ASSERT_TRUE(ok);

    }

    // name of the folder with the B-spline network data
    std::vector<Handle(Geom_Curve)> vcurves, ucurves;
    std::string path_output;

};

TEST_P(interpolate_curve_network, testFromBRep)
{

    Handle(Geom_BSplineSurface) gordonSurface = geoml::interpolate_curve_network(ucurves, vcurves, 3e-4);
    BRepTools::Write(BRepBuilderAPI_MakeFace(gordonSurface, Precision::Confusion()), path_output.c_str());
}

INSTANTIATE_TEST_SUITE_P(SurfaceModeling, interpolate_curve_network, ::testing::Values(
   "nacelle",
   "full_nacelle",
   "wing2",
   "spiralwing",
   "test_surface4_sorted",
   "test_surface4",
   "wing3",
   "bellyfairing",
   "helibody",
   "fuselage1",
   "fuselage2",
   "ffd"
));

class interpolate_curves: public ::testing::TestWithParam<std::string>
{
protected:
    void SetUp() override
    {
        // get the name of the folder with the B-spline network data
        std::string path_profiles = "../unittests/TestData/CurveNetworks/" + GetParam() + "/profiles.brep";

        std::filesystem::path output_dir("TestData/InterpolateCurves/" + GetParam());
        if (!std::filesystem::is_directory(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }

        path_output = "TestData/InterpolateCurves/" + GetParam() + "/result.brep";

        bool ok = false;
        curves = apitests::read_curves(path_profiles, ok);
        ASSERT_TRUE(ok);
    }

    // name of the folder with the B-spline network data
    std::vector<Handle(Geom_Curve)> curves;
    std::string path_output;

};

TEST_P(interpolate_curves, testFromBRep)
{

    Handle(Geom_BSplineSurface) surface = geoml::interpolate_curves(curves);
    BRepTools::Write(BRepBuilderAPI_MakeFace(surface, Precision::Confusion()), path_output.c_str());

}

// @TODO: not all are working
INSTANTIATE_TEST_SUITE_P(SurfaceModeling, interpolate_curves, ::testing::Values(
  // "nacelle",
  "full_nacelle",
  "wing2",
  // "spiralwing",
  "test_surface4_sorted",
  "test_surface4",
  "wing3",
  // "bellyfairing",
  "helibody",
  "fuselage1",
  "fuselage2",
  "ffd"
));

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

TEST(SurfaceModeling, revolution_surface)
{
    // create a cylindrical surface:

    // create a profile curve for the cylinder
    gp_Pnt start_point(7., 0., 2.);
    gp_Pnt end_point(23., 0., 2.); // The height of the cylinder

    TColgp_Array1OfPnt profile_points(1,2);
    profile_points.SetValue(1,start_point);
    profile_points.SetValue(2,end_point);

    // degree:
    Standard_Integer degree = 1;

    // weights:
    TColStd_Array1OfReal weights(1, 2);
    weights.SetValue(1, 1.0);  
    weights.SetValue(2, 1.0);  

    // knots:
    TColStd_Array1OfReal knots(1,2); 
    knots.SetValue(1,0.0);           
    knots.SetValue(2,1.0);

    // multiplicities: 
    TColStd_Array1OfInteger mults(1,2); 
    mults.SetValue(1,2);                
    mults.SetValue(2,2);

    Handle(Geom_BSplineCurve) profile_curve 
    	= geoml::b_spline_curve(profile_points, weights, knots, mults, degree);


    Handle(Geom_BSplineSurface) cylinder =
        geoml::revolving_surface(profile_curve, 
                                 gp_Ax1(gp_Pnt(0., 0., 0.), gp_Dir(1., 0., 0.)));

    // Create a TopoDS_Face using BRepBuilderAPI_MakeFace
    BRepBuilderAPI_MakeFace face_maker(cylinder, 1.0e-6); 
    TopoDS_Face face = face_maker.Face();
    TopoDS_Shape shape = face;

    // calculate the area of the surface 
    GProp_GProps properties;
    BRepGProp::SurfaceProperties(shape, properties);

    double area = properties.Mass();

    EXPECT_NEAR(area, 201.601, 1e-3);
    EXPECT_TRUE(cylinder->IsVRational());
    EXPECT_FALSE(cylinder->IsURational());
    EXPECT_EQ(cylinder->NbUPoles(),6);
    EXPECT_EQ(cylinder->NbVPoles(),2);
    EXPECT_EQ(cylinder->UDegree(),2);
    EXPECT_EQ(cylinder->VDegree(),1);
    EXPECT_EQ(cylinder->NbUKnots(),4);
    EXPECT_EQ(cylinder->NbVKnots(),2);
    EXPECT_TRUE(cylinder->IsUPeriodic());
    EXPECT_FALSE(cylinder->IsVPeriodic());

    //writeGeomEntityToStepFile(cylinder, "cylinder_periodic.stp");

    cylinder->SetUNotPeriodic();
    EXPECT_FALSE(cylinder->IsUPeriodic());
    EXPECT_EQ(cylinder->NbUPoles(),7); // this is important to note: it changed from 6 to 7
    EXPECT_EQ(cylinder->NbUKnots(),6); // changed from 4 to 6 (why?)

    //writeGeomEntityToStepFile(cylinder, "cylinder_non_periodic.stp");
        
}

TEST(SurfaceModeling, b_spline_surface)
{
    int num_rows = 3;   //    ** 
    int num_cols = 2;   //    **  
                        //    **          

    TColgp_Array2OfPnt srf_cp_net(1, num_rows, 1, num_cols);

    // set the degree in U direction and V direction
    Standard_Integer degree_U = 2;
    Standard_Integer degree_V = 1;

    EXPECT_EQ(srf_cp_net.ColLength(), 3);
    EXPECT_EQ(srf_cp_net.RowLength(), 2);
/*
    // define the weight net for the surface:
    TColStd_Array2OfReal srf_weight_net(1, srf_cp_net.ColLength(), 1, 
                                                    rear_srf_cp_net.RowLength()); // is this use correct?

    rear_srf_weight_net.SetValue(1, 1, 1.0);
    rear_srf_weight_net.SetValue(2, 1, 0.5);
    rear_srf_weight_net.SetValue(3, 1, 1.0);
    rear_srf_weight_net.SetValue(4, 1, 0.5);
    rear_srf_weight_net.SetValue(5, 1, 1.0);
    rear_srf_weight_net.SetValue(6, 1, 0.5);

    rear_srf_weight_net.SetValue(1, 2, 1.0);
    rear_srf_weight_net.SetValue(2, 2, 0.5);
    rear_srf_weight_net.SetValue(3, 2, 1.0);
    rear_srf_weight_net.SetValue(4, 2, 0.5);
    rear_srf_weight_net.SetValue(5, 2, 1.0);
    rear_srf_weight_net.SetValue(6, 2, 0.5);

    rear_srf_weight_net.SetValue(1, 3, 1.0);
    rear_srf_weight_net.SetValue(2, 3, 0.5);
    rear_srf_weight_net.SetValue(3, 3, 1.0);
    rear_srf_weight_net.SetValue(4, 3, 0.5);
    rear_srf_weight_net.SetValue(5, 3, 1.0);
    rear_srf_weight_net.SetValue(6, 3, 0.5);

    rear_srf_weight_net.SetValue(1, 4, 1.0);
    rear_srf_weight_net.SetValue(2, 4, 0.5);
    rear_srf_weight_net.SetValue(3, 4, 1.0);
    rear_srf_weight_net.SetValue(4, 4, 0.5);
    rear_srf_weight_net.SetValue(5, 4, 1.0);
    rear_srf_weight_net.SetValue(6, 4, 0.5);


    rear_srf_weight_net.SetValue(1, 5, 1.0);
    rear_srf_weight_net.SetValue(2, 5, 0.5);
    rear_srf_weight_net.SetValue(3, 5, 1.0);
    rear_srf_weight_net.SetValue(4, 5, 0.5);
    rear_srf_weight_net.SetValue(5, 5, 1.0);
    rear_srf_weight_net.SetValue(6, 5, 0.5);


    // set the U-knots (columns):
    TColStd_Array1OfReal knots_U(1,4); 

    knots_U.SetValue(1,0.0);           
    knots_U.SetValue(2,1.0);
    knots_U.SetValue(3,2.0);
    knots_U.SetValue(4,3.0);

    // set the V-knots (rows):
    TColStd_Array1OfReal knots_V(1,2); 

    knots_U.SetValue(1,0.0);           
    knots_U.SetValue(2,1.0);

    // multiplicities U-direction: 
    TColStd_Array1OfInteger mults_U(1,4); // as we are in the periodic case, the mults must 	
                                        // maybe be adapted...(!)
    mults_U.SetValue(1,3);                
    mults_U.SetValue(2,2);
    mults_U.SetValue(3,2);
    mults_U.SetValue(4,3);

    // multiplicities V-direction: 
    TColStd_Array1OfInteger mults_V(1,2); 

    mults_V.SetValue(1,5);                
    mults_V.SetValue(2,5);

    std::cout << "does it get till here?" << std::endl;

    // define the rear surface:
    Handle(Geom_BSplineSurface) rear_surface
        = geoml::b_spline_surface( rear_srf_cp_net, 
                            rear_srf_weight_net, 
                            knots_U, 
                            knots_V, 
                            mults_U, 
                            mults_V, 
                            degree_U, 
                            degree_V, 
                            Standard_True ); 
*/					
}


