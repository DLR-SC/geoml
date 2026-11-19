from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt


start_points = [
    gp_Pnt(0.0, 0.0, 0.0),
    gp_Pnt(1.0, 0.0, 0.0),
    gp_Pnt(2.0, 1.0, 0.0),
]
end_points = [
    gp_Pnt(4.0, 3.0, 0.0),
    gp_Pnt(5.0, 4.0, 0.0),
    gp_Pnt(6.0, 4.0, 0.0),
]

start_curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(start_points), 2)
end_curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(end_points), 2)

start_edge = pygeoml.CurveToEdge(start_curve)
end_edge = pygeoml.CurveToEdge(end_curve)

start_connection = pygeoml.BlendCurveConnection(
    start_edge,
    start_points[-1],
    pygeoml.GContinuity_G1,
)
end_connection = pygeoml.BlendCurveConnection(
    end_edge,
    end_points[0],
    pygeoml.GContinuity_G1,
)

blend = pygeoml.BlendCurve(start_connection, end_connection)
blend_edge = blend.blend_curve()
blend_curve = pygeoml.EdgeToCurve(blend_edge)

# wrap the edges in Shapes and name them with meta tags
start_shape = pygeoml.Shape(start_edge)
end_shape = pygeoml.Shape(end_edge)
blend_shape = pygeoml.Shape(blend_edge)
start_shape.add_meta_tag("start-curve")
end_shape.add_meta_tag("end-curve")
blend_shape.add_meta_tag("blend-curve")
