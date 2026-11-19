from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt


# a 3 x 2 grid of control points
control_points = pygeoml.Array2dgp_Pnt(3, 2)
control_points.setValue(0, 0, gp_Pnt(0.0, 0.0, 0.0))
control_points.setValue(1, 0, gp_Pnt(0.0, 1.0, 0.0))
control_points.setValue(2, 0, gp_Pnt(0.0, 2.0, 0.0))
control_points.setValue(0, 1, gp_Pnt(1.0, 0.0, 0.0))
control_points.setValue(1, 1, gp_Pnt(1.0, 1.0, 0.0))
control_points.setValue(2, 1, gp_Pnt(1.0, 2.0, 1.0))

weights = pygeoml.Array2dStandardReal(3, 2)
for i in range(3):
    for j in range(2):
        weights.setValue(i, j, 1.0)

u_knots = containers.standard_real_vector([0.0, 1.0])
v_knots = containers.standard_real_vector([0.0, 1.0])
u_mults = containers.int_vector([3, 3])
v_mults = containers.int_vector([2, 2])

surface = pygeoml.nurbs_surface(control_points, weights, u_knots, v_knots, u_mults, v_mults, 2, 1)

# Use surface.Value(u, v) to evaluate points on the surface
