"""Render the result geometry of the C++ examples to docs/_static/examples/.

The colours follow the wing image on the overview page: blue for the main
geometry, orange for highlighted edges, curves and points, light grey for
secondary parts.

Run with the Python of the pixi environment (needs geoml and pythonocc):

    python docs/render_example_images.py
"""
import os
import runpy

from OCC.Core.AIS import AIS_Shape
from OCC.Core.Aspect import Aspect_POM_Fill, Aspect_TOL_SOLID
from OCC.Core.BRep import BRep_Builder
from OCC.Core.BRepBuilderAPI import BRepBuilderAPI_MakeEdge, BRepBuilderAPI_MakeFace
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeSphere
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

    def _show(self, ais):
        drawer = ais.Attributes()
        drawer.SetDeviationCoefficient(1e-4)
        drawer.SetDeviationAngle(0.02)
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
        self._show(ais)

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


def interpolate(points, degree=None):
    vec = containers.point_vector(points)
    if degree is None:
        return pygeoml.interpolate_points_to_b_spline_curve(vec)
    return pygeoml.interpolate_points_to_b_spline_curve(vec, degree)


# --- Curves and surfaces: the geometry of the C++ examples -----------------

def render_curve():
    points = [gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(2.0, 0.0, 1.0)]
    scene = Scene()
    scene.edges([edge(interpolate(points))], BLUE_CURVE)
    scene.points(points, 0.03)
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
    scene = Scene()
    scene.edges([start_edge, end_edge], BLUE_CURVE)
    scene.edges([blend.blend_curve()], ORANGE)
    scene.points(start_points + end_points, 0.08, BLUE_CURVE)
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
    surface = Geom_BSplineSurface(poles, knots, knots, u_mults, v_mults, 2, 1)

    control_polygon = [edge_between(grid[i][0], grid[i][1]) for i in range(3)]
    control_polygon += [edge_between(grid[i][j], grid[i + 1][j]) for i in range(2) for j in range(2)]

    scene = Scene()
    scene.faces([BRepBuilderAPI_MakeFace(surface, 1e-6).Face()])
    scene.edges(control_polygon, ORANGE, 1.5)
    scene.points([p for row in grid for p in row], 0.03)
    scene.save("surface_example.png", (1.0, -0.6, 0.8))


def edge_between(p, q):
    return BRepBuilderAPI_MakeEdge(p, q).Edge()


def render_curve_network():
    p = [[gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 1.0, 0.0), gp_Pnt(1.0, 2.0, 0.0)],
         [gp_Pnt(0.0, 0.0, 3.0), gp_Pnt(0.0, 1.0, 3.0), gp_Pnt(1.0, 2.0, 3.0)],
         [gp_Pnt(0.0, 4.0, 3.0), gp_Pnt(0.0, 5.0, 3.0), gp_Pnt(1.0, 6.0, 3.0)]]
    u_curves = [interpolate(p[i], 2) for i in range(3)]
    v_curves = [interpolate([p[0][j], p[1][j], p[2][j]], 2) for j in range(3)]
    surface = pygeoml.interpolate_curve_network(
        containers.geomcurve_vector(u_curves), containers.geomcurve_vector(v_curves), 1e-1)

    scene = Scene()
    scene.faces([BRepBuilderAPI_MakeFace(surface, 1e-6).Face()], edge_colour=None)
    scene.edges([edge(c) for c in u_curves + v_curves], ORANGE, 3.0)
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
