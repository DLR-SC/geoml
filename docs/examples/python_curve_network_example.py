from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt


def curve_from_points(points):
    return pygeoml.interpolate_points_to_b_spline_curve(
        containers.point_vector(points),
        2,
    )


p00 = gp_Pnt(0.0, 0.0, 0.0)
p01 = gp_Pnt(0.0, 1.0, 0.0)
p02 = gp_Pnt(1.0, 2.0, 0.0)

p10 = gp_Pnt(0.0, 0.0, 3.0)
p11 = gp_Pnt(0.0, 1.0, 3.0)
p12 = gp_Pnt(1.0, 2.0, 3.0)

p20 = gp_Pnt(0.0, 4.0, 3.0)
p21 = gp_Pnt(0.0, 5.0, 3.0)
p22 = gp_Pnt(1.0, 6.0, 3.0)

u_curves = containers.geomcurve_vector([
    curve_from_points([p00, p01, p02]),
    curve_from_points([p10, p11, p12]),
    curve_from_points([p20, p21, p22]),
])

v_curves = containers.geomcurve_vector([
    curve_from_points([p00, p10, p20]),
    curve_from_points([p01, p11, p21]),
    curve_from_points([p02, p12, p22]),
])

surface = pygeoml.interpolate_curve_network(u_curves, v_curves, 1e-1)
