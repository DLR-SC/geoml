from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt


p00 = gp_Pnt(0.0, 0.0, 0.0)
p01 = gp_Pnt(0.0, 1.0, 0.0)
p02 = gp_Pnt(1.0, 2.0, 0.0)

p10 = gp_Pnt(0.0, 0.0, 3.0)
p11 = gp_Pnt(0.0, 1.0, 3.0)
p12 = gp_Pnt(1.0, 2.0, 3.0)

p20 = gp_Pnt(0.0, 4.0, 3.0)
p21 = gp_Pnt(0.0, 5.0, 3.0)
p22 = gp_Pnt(1.0, 6.0, 3.0)

# B-spline curves of degree 2 through the points
u_curves = containers.geomcurve_vector([
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p00, p01, p02]), 2),
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p10, p11, p12]), 2),
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p20, p21, p22]), 2),
])

v_curves = containers.geomcurve_vector([
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p00, p10, p20]), 2),
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p01, p11, p21]), 2),
    pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector([p02, p12, p22]), 2),
])

surface = pygeoml.interpolate_curve_network(u_curves, v_curves, 1e-1)
