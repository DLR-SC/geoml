# Selection cases: L = (case_1 & selection_1) | (case_2 & selection_2)
from geoml import pygeoml
from geoml.pygeoml import is_edge, is_face, is_vertex
from OCC.Core.BRepGProp import brepgprop_SurfaceProperties
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.Core.GProp import GProp_GProps
from OCC.Core.gp import gp_Pnt

# Constant predicates: a case is ALWAYS or NEVER, depending on the parameters
ALWAYS = is_vertex | ~is_vertex
NEVER = ~ALWAYS


def case(condition):
    return ALWAYS if condition else NEVER


def select_faces(shape, condition):
    """Select the faces of `shape` for which condition(TopoDS_Face) is true."""
    faces = shape.select_subshapes(is_face)
    predicate = None
    for i in range(faces.size()):
        if condition(faces[i].shape()):
            same = pygeoml.is_same(faces[i])
            predicate = same if predicate is None else predicate | same
    return shape.select_subshapes(predicate)


def at(axis, value):
    """True for faces whose centre of mass has the coordinate `value` in direction `axis`."""
    def condition(face):
        props = GProp_GProps()
        brepgprop_SurfaceProperties(face, props)
        return abs(props.CentreOfMass().Coord(axis + 1) - value) < 1e-6
    return condition


def make_part(d):
    """A box with a notch of width 2 at position d in its frontal upper edge X.

    Modelling intention:
      case 1 (d >= 3): fillet both pieces of X
      case 2 (d <  3): fillet only the piece of X right of the notch
    """
    box = pygeoml.create_box(10.0, 6.0, 4.0)
    front = select_faces(box, at(1, 0.0)).unique_element()
    top = select_faces(box, at(2, 4.0)).unique_element()
    right = select_faces(box, at(0, 10.0)).unique_element()
    X = box.select_subshapes(is_edge & pygeoml.is_subshape_of(front) & pygeoml.is_subshape_of(top)).unique_element()

    tool = pygeoml.Shape(BRepPrimAPI_MakeBox(gp_Pnt(d, -1.0, 3.0), 2.0, 2.0, 2.0).Shape())
    notched = box - tool

    # Selections: the pieces of X, and the piece of X that touches the right face
    right_now = notched.select_subshapes(is_face & pygeoml.is_descendent_of(right)).unique_element()
    pieces_of_X = is_edge & pygeoml.is_descendent_of(X)
    right_piece_of_X = pieces_of_X & pygeoml.has_subshape_that(is_vertex & pygeoml.is_subshape_of(right_now))

    # Cases: conditions on the parameter only, mutually exclusive and exhaustive
    case_1 = case(d >= 3.0)
    case_2 = case(d < 3.0)

    # The selection criterion L
    L = (case_1 & pieces_of_X) | (case_2 & right_piece_of_X)

    edges = notched.select_subshapes(L)
    return notched, edges, pygeoml.make_fillet(notched, edges, 0.5)


notched, edges, part = make_part(2.0)
assert edges.size() == 1  # case 2: only the piece right of the notch

notched, edges, part = make_part(5.0)
assert edges.size() == 2  # case 1: both pieces

print("OK")
