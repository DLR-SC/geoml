"""Render the result geometry of the C++ examples to docs/_static/examples/.

The colours follow the wing image on the overview page: blue for the main
geometry, orange for highlighted edges and for the control points of curves
and surfaces, grey for secondary parts.

Run with the Python of the pixi environment (needs geoml and pythonocc):

    python docs/render_example_images.py
"""
import os
import runpy

from OCC.Core.AIS import AIS_Shape
from OCC.Core.Aspect import Aspect_POM_Fill, Aspect_TOL_SOLID
from OCC.Core.BRep import BRep_Builder
from OCC.Core.BRepAdaptor import BRepAdaptor_Curve, BRepAdaptor_Surface
from OCC.Core.BRepBuilderAPI import BRepBuilderAPI_MakeEdge, BRepBuilderAPI_MakeFace
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeSphere
from OCC.Core.GeomAbs import GeomAbs_BezierCurve
from OCC.Core.Graphic3d import Graphic3d_MaterialAspect, Graphic3d_NOM_PLASTIC
from OCC.Core.Prs3d import Prs3d_LineAspect
from OCC.Core.Quantity import Quantity_Color, Quantity_TOC_sRGB
from OCC.Core.TopoDS import TopoDS_Compound
from OCC.Core.gp import gp_Pnt
from OCC.Display.OCCViewer import Viewer3d

from geoml import pygeoml
from geoml.occ_helpers import containers

HERE = os.path.dirname(os.path.abspath(__file__))
EXAMPLES = os.path.join(HERE, "examples")
OUT = os.path.join(HERE, "_static", "examples")

BLUE = "#0a6fa8"        # surfaces (rendered with shading)
BLUE_EDGE = "#3375a3"   # edges of blue surfaces
BLUE_CURVE = "#2c5979"  # curves
ORANGE = "#ff7f00"      # highlighted edges, curves and points
GREY = "#e6e6e6"        # secondary parts
GREY_CURVE = "#a0a0a0"  # secondary curves
BLACK = "#000000"       # edges of grey parts


def colour(hex_code):
    r, g, b = (int(hex_code[i:i + 2], 16) / 255.0 for i in (1, 3, 5))
    return Quantity_Color(r, g, b, Quantity_TOC_sRGB)


def compound(shapes):
    result = TopoDS_Compound()
    builder = BRep_Builder()
    builder.MakeCompound(result)
    for s in shapes:
        builder.Add(result, s)
    return result


def occ_shapes(selection):
    """The OpenCASCADE shapes of a geoml selection."""
    return [selection[i].shape() for i in range(selection.size())]


class Scene:
    def __init__(self, width=1200, height=800):
        self.viewer = Viewer3d()
        self.viewer.Create()
        self.viewer.SetSize(width, height)
        self.viewer.View.SetBackgroundColor(colour("#ffffff"))
        self.viewer.View.ChangeRenderingParams().NbMsaaSamples = 8
        self.viewer.default_drawer.SetFaceBoundaryDraw(True)
        self.context = self.viewer.Context

    def _show(self, ais, fine=True):
        drawer = ais.Attributes()
        drawer.SetDeviationCoefficient(1e-4 if fine else 1e-3)
        drawer.SetDeviationAngle(0.02 if fine else 0.2)
        self.context.Display(ais, False)
        self.context.Deactivate(ais)

    def faces(self, shapes, face_colour=BLUE, edge_colour=BLUE_EDGE, edge_width=1.5):
        ais = AIS_Shape(compound(shapes))
        ais.SetDisplayMode(1)
        material = Graphic3d_MaterialAspect(Graphic3d_NOM_PLASTIC)
        material.SetSpecularColor(colour("#404040"))
        ais.SetMaterial(material)
        ais.SetColor(colour(face_colour))
        drawer = ais.Attributes()
        drawer.SetFaceBoundaryDraw(edge_colour is not None)
        drawer.SetFaceBoundaryAspect(Prs3d_LineAspect(colour(edge_colour or face_colour), Aspect_TOL_SOLID, edge_width))
        # push the faces back so that curves lying on them stay visible
        drawer.ShadingAspect().Aspect().SetPolygonOffsets(Aspect_POM_Fill, 4.0, 4.0)
        self._show(ais)

    def edges(self, shapes, edge_colour=ORANGE, width=4.0):
        ais = AIS_Shape(compound(shapes))
        ais.SetDisplayMode(0)
        drawer = ais.Attributes()
        aspect = Prs3d_LineAspect(colour(edge_colour), Aspect_TOL_SOLID, width)
        drawer.SetWireAspect(aspect)
        drawer.SetFreeBoundaryAspect(aspect)
        drawer.SetUnFreeBoundaryAspect(aspect)
        drawer.SetSeenLineAspect(aspect)
        self._show(ais)

    def points(self, pnts, radius, point_colour=ORANGE):
        spheres = [BRepPrimAPI_MakeSphere(p, radius).Shape() for p in pnts]
        ais = AIS_Shape(compound(spheres))
        ais.SetDisplayMode(1)
        ais.SetColor(colour(point_colour))
        ais.Attributes().SetFaceBoundaryDraw(False)
        self._show(ais, fine=False)  # a fine mesh of many spheres takes very long

    def save(self, name, direction=(1.0, -1.0, 1.0), zoom=0.85):
        """Save the scene, seen from `direction` with z pointing up."""
        self.viewer.View.SetProj(*direction)
        self.viewer.View.SetUp(0.0, 0.0, 1.0)
        self.viewer.FitAll()
        self.viewer.View.SetZoom(zoom)
        self.viewer.View.Redraw()
        os.makedirs(OUT, exist_ok=True)
        path = os.path.join(OUT, name)
        self.viewer.View.Dump(path)
        print("wrote", path)


def edge(curve):
    return BRepBuilderAPI_MakeEdge(curve).Edge()


def face(surface):
    return BRepBuilderAPI_MakeFace(surface, 1e-6).Face()


def edge_between(p, q):
    return BRepBuilderAPI_MakeEdge(p, q).Edge()


def interpolate(points, degree=None):
    vec = containers.point_vector(points)
    if degree is None:
        return pygeoml.interpolate_points_to_b_spline_curve(vec)
    return pygeoml.interpolate_points_to_b_spline_curve(vec, degree)


def curve_poles(curve_edge):
    """The control points of the B-spline or Bezier curve of an edge."""
    adaptor = BRepAdaptor_Curve(curve_edge)
    if adaptor.GetType() == GeomAbs_BezierCurve:
        curve = adaptor.Bezier()
    else:
        curve = adaptor.BSpline()
    return [curve.Pole(i) for i in range(1, curve.NbPoles() + 1)]


def surface_poles(surface_face):
    """The control points of the B-spline surface of a face as a grid."""
    bspline = BRepAdaptor_Surface(surface_face).BSpline()
    return [[bspline.Pole(i, j) for j in range(1, bspline.NbVPoles() + 1)]
            for i in range(1, bspline.NbUPoles() + 1)]


def control_polygon(scene, poles, radius):
    """Show the control points and the control polygon of a curve."""
    scene.edges([edge_between(p, q) for p, q in zip(poles, poles[1:])], ORANGE, 1.5)
    scene.points(poles, radius)


def control_net(scene, grid, radius):
    """Show the control points and the control net of a surface."""
    lines = [edge_between(p, q) for row in grid for p, q in zip(row, row[1:])]
    lines += [edge_between(p, q) for r0, r1 in zip(grid, grid[1:]) for p, q in zip(r0, r1)]
    scene.edges(lines, ORANGE, 1.5)
    scene.points([p for row in grid for p in row], radius)


# --- Curves and surfaces: the geometry of the C++ examples -----------------
# Curves and surfaces are shown with their control points (orange).

def render_curve():
    points = [gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(2.0, 0.0, 1.0)]
    curve = edge(interpolate(points))
    scene = Scene()
    scene.edges([curve], BLUE_CURVE)
    control_polygon(scene, curve_poles(curve), 0.03)
    scene.save("curve_example.png", (0.0, -1.0, 0.0))


def render_blend_curve():
    start_points = [gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(2.0, 1.0, 0.0)]
    end_points = [gp_Pnt(4.0, 3.0, 0.0), gp_Pnt(5.0, 4.0, 0.0), gp_Pnt(6.0, 4.0, 0.0)]
    start_edge = pygeoml.CurveToEdge(interpolate(start_points, 2))
    end_edge = pygeoml.CurveToEdge(interpolate(end_points, 2))
    blend = pygeoml.BlendCurve(
        pygeoml.BlendCurveConnection(start_edge, start_points[-1], pygeoml.GContinuity_G1),
        pygeoml.BlendCurveConnection(end_edge, end_points[0], pygeoml.GContinuity_G1),
    )
    blend_edge = blend.blend_curve()
    scene = Scene()
    scene.edges([start_edge, end_edge], GREY_CURVE)
    scene.edges([blend_edge], BLUE_CURVE)
    for e in (start_edge, end_edge, blend_edge):
        control_polygon(scene, curve_poles(e), 0.07)
    scene.save("blend_curve_example.png", (0.0, 0.0, 1.0))


def render_surface():
    from OCC.Core.Geom import Geom_BSplineSurface
    from OCC.Core.TColgp import TColgp_Array2OfPnt
    from OCC.Core.TColStd import TColStd_Array1OfReal, TColStd_Array1OfInteger

    grid = [[gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0)],
            [gp_Pnt(0.0, 1.0, 0.0), gp_Pnt(1.0, 1.0, 0.0)],
            [gp_Pnt(0.0, 2.0, 0.0), gp_Pnt(1.0, 2.0, 1.0)]]
    poles = TColgp_Array2OfPnt(1, 3, 1, 2)
    for i in range(3):
        for j in range(2):
            poles.SetValue(i + 1, j + 1, grid[i][j])
    knots = TColStd_Array1OfReal(1, 2)
    knots.SetValue(1, 0.0)
    knots.SetValue(2, 1.0)
    u_mults = TColStd_Array1OfInteger(1, 2)
    u_mults.SetValue(1, 3)
    u_mults.SetValue(2, 3)
    v_mults = TColStd_Array1OfInteger(1, 2)
    v_mults.SetValue(1, 2)
    v_mults.SetValue(2, 2)
    # the same surface as geoml::nurbs_surface in the example (all weights are 1)
    surface = face(Geom_BSplineSurface(poles, knots, knots, u_mults, v_mults, 2, 1))

    scene = Scene()
    scene.faces([surface])
    control_net(scene, surface_poles(surface), 0.03)
    scene.save("surface_example.png", (1.0, -0.6, 0.8))


def render_curve_network():
    p = [[gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 1.0, 0.0), gp_Pnt(1.0, 2.0, 0.0)],
         [gp_Pnt(0.0, 0.0, 3.0), gp_Pnt(0.0, 1.0, 3.0), gp_Pnt(1.0, 2.0, 3.0)],
         [gp_Pnt(0.0, 4.0, 3.0), gp_Pnt(0.0, 5.0, 3.0), gp_Pnt(1.0, 6.0, 3.0)]]
    u_curves = [interpolate(p[i], 2) for i in range(3)]
    v_curves = [interpolate([p[0][j], p[1][j], p[2][j]], 2) for j in range(3)]
    surface = face(pygeoml.interpolate_curve_network(
        containers.geomcurve_vector(u_curves), containers.geomcurve_vector(v_curves), 1e-1))

    scene = Scene()
    scene.faces([surface], edge_colour=None)
    scene.edges([edge(c) for c in u_curves + v_curves], GREY, 3.0)
    control_net(scene, surface_poles(surface), 0.05)
    scene.save("curve_network_example.png", (1.0, -0.4, 0.3))


# --- Subshapes: the Python versions build the same geometry as C++ ---------

def run_example(name):
    return runpy.run_path(os.path.join(EXAMPLES, name))


def render_selection():
    ex = run_example("python_shape_selection_example.py")
    box, top_edges = ex["box"], ex["top_edges"]
    scene = Scene()
    scene.faces([box.shape()])
    scene.edges(occ_shapes(top_edges))
    scene.points([pygeoml_vertex_point(v) for v in occ_shapes(ex["bottom_vertices"])], 0.35)
    scene.save("shape_selection_example.png")


def pygeoml_vertex_point(vertex):
    from OCC.Core.BRep import BRep_Tool
    from OCC.Core.TopoDS import topods
    return BRep_Tool.Pnt(topods.Vertex(vertex))


def render_history():
    ex = run_example("python_shape_history_example.py")
    result, top = ex["result"], ex["top"]
    is_face = pygeoml.is_face
    from_tools = result.select_subshapes(
        is_face & (pygeoml.is_descendent_of_subshape_in(ex["drill"])
                   | pygeoml.is_descendent_of_subshape_in(ex["corner"])))
    from_box = result.select_subshapes(is_face & ~pygeoml.is_descendent_of_subshape_in(ex["drill"])
                                       & ~pygeoml.is_descendent_of_subshape_in(ex["corner"]))
    top_now = result.select_subshapes(is_face & pygeoml.is_descendent_of(top)).unique_element()
    top_edges = result.select_subshapes(pygeoml.is_edge & pygeoml.is_subshape_of(top_now))

    scene = Scene()
    scene.faces(occ_shapes(from_box))
    scene.faces(occ_shapes(from_tools), GREY, BLACK, 1.0)
    scene.edges(occ_shapes(top_edges))
    scene.save("shape_history_example.png", (1.0, 0.7, 0.9))


def render_tagging():
    ex = run_example("python_shape_tagging_example.py")
    step2 = ex["step2"]
    has_tag = pygeoml.has_tag
    bore = step2.select_subshapes(has_tag("bore"))
    others = step2.select_subshapes(pygeoml.is_face & ~has_tag("bore"))
    top_face = step2.select_subshapes(has_tag("top")).unique_element()
    top_edges = step2.select_subshapes(pygeoml.is_edge & pygeoml.is_subshape_of(top_face))

    scene = Scene()
    scene.faces(occ_shapes(others))
    scene.faces(occ_shapes(bore), GREY, BLACK, 1.0)
    scene.edges(occ_shapes(top_edges))
    scene.save("shape_tagging_example.png", (1.0, 0.7, 0.9))


if __name__ == "__main__":
    render_curve()
    render_blend_curve()
    render_surface()
    render_curve_network()
    render_selection()
    render_history()
    render_tagging()
