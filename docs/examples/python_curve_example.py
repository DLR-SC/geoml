from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt


points = [
    gp_Pnt(0.0, 0.0, 0.0),
    gp_Pnt(0.5, 0.0, 0.0),
    gp_Pnt(1.0, 1.0, 0.0),
]

curve = pygeoml.interpolate_points_to_b_spline_curve(
    containers.point_vector(points),
    2,
)

edge = pygeoml.CurveToEdge(curve)
shape = pygeoml.Shape(edge)

shape.add_meta_tag("guide-curve")
