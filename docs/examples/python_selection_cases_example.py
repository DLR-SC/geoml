# Selection cases: L = (Case_1 and Selection_1) or (Case_2 and Selection_2)
from geoml import pygeoml
from geoml.pygeoml import is_edge, is_face
from OCC.Core.BRepGProp import brepgprop_SurfaceProperties
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.Core.GProp import GProp_GProps
from OCC.Core.gp import gp_Pnt


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


def select_case(cases):
    """Return the selection of the one case that applies."""
    assert sum(case for case, _ in cases) == 1, "the cases are not mutually exclusive and exhaustive"
    return next(selection for case, selection in cases if case)


def make_part(d):
    """A box B, moved along the frontal upper edge X of a box by d, is subtracted from the box.

    Modelling intention:
      case 1, B cuts X:         fillet the edges of the notch on the front face
      case 2, B does not cut X: fillet X
    """
    box = pygeoml.create_box(10.0, 6.0, 4.0)
    front = select_faces(box, at(1, 0.0)).unique_element()
    top = select_faces(box, at(2, 4.0)).unique_element()
    X = box.select_subshapes(is_edge & pygeoml.is_subshape_of(front) & pygeoml.is_subshape_of(top)).unique_element()

    B = pygeoml.Shape(BRepPrimAPI_MakeBox(gp_Pnt(d, -1.0, 3.0), 2.0, 2.0, 2.0).Shape())
    result = box - B

    # Cases, formulated with a history predicate: B cuts X if the result
    # contains a modified descendent of X
    case_1 = not result.select_subshapes(pygeoml.is_modified_descendent_of(X)).is_empty()
    case_2 = not case_1

    # Selections: the edges shared by the front face and the faces from B, and the edge X
    front_now = result.select_subshapes(is_face & pygeoml.is_descendent_of(front)).unique_element()
    faces_from_B = result.select_subshapes(is_face & pygeoml.is_descendent_of_subshape_in(B))
    selection_1 = is_edge & pygeoml.is_subshape_of(front_now) & pygeoml.is_subshape_of(faces_from_B)
    selection_2 = is_edge & pygeoml.is_descendent_of(X)

    # The selection criterion L
    L = select_case([(case_1, selection_1), (case_2, selection_2)])

    edges = result.select_subshapes(L)
    return result, edges, pygeoml.make_fillet(result, edges, 0.3)


result, edges, part = make_part(4.0)
assert edges.size() == 3  # case 1: B cuts X, the three edges of the notch

result, edges, part = make_part(12.0)
assert edges.size() == 1  # case 2: B misses the box, the edge X

print("OK")
