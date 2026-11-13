// Minimal example: create a simple NURBS surface from a 2x3 control grid
#include <vector>
#include <gp_Pnt.hxx>
#include <Geom_BSplineSurface.hxx>
#include "geoml/surfaces/surfaces.h"
#include "geoml/data_structures/Array2d.h"

int main()
{
    gp_Pnt p00(0.0, 0.0, 0.0);
    gp_Pnt p10(0.0, 1.0, 0.0);
    gp_Pnt p20(0.0, 2.0, 0.0);
    gp_Pnt p01(1.0, 0.0, 0.0);
    gp_Pnt p11(1.0, 1.0, 0.0);
    gp_Pnt p21(1.0, 2.0, 1.0);

    geoml::Array2d<gp_Pnt> control_points(3,2);
    control_points.setValue(0,0,p00);
    control_points.setValue(1,0,p10);
    control_points.setValue(2,0,p20);
    control_points.setValue(0,1,p01);
    control_points.setValue(1,1,p11);
    control_points.setValue(2,1,p21);

    geoml::Array2d<double> weights(3,2);
    for(int i=0;i<3;i++) for(int j=0;j<2;j++) weights.setValue(i,j,1.0);

    std::vector<double> U_knots{0.0, 1.0};
    std::vector<double> V_knots{0.0, 1.0};
    std::vector<int> U_mults{3,3};
    std::vector<int> V_mults{2,2};

    Handle(Geom_BSplineSurface) s = geoml::nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, 2, 1);

    // Use s->D0(u,v,point) to evaluate points on the surface

    return 0;
}
