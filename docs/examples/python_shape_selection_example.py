from geoml import pygeoml
from geoml.pygeoml import is_vertex, is_edge, is_face
from OCC.Core.BRepGProp import brepgprop_SurfaceProperties
from OCC.Core.GProp import GProp_GProps


def is_face_at_height(shape, z):
    """Predicate for the faces of `shape` whose centre of mass lies at height z.

    Python functions cannot be used as geoml predicates directly. Instead, look
    up the faces with pythonocc and build a predicate from them with is_same.
    """
    faces = shape.select_subshapes(is_face)
    predicate = None
    # Index the selection with range(): iterating over a Shape directly
    # does not stop at the last subshape.
    for i in range(faces.size()):
        props = GProp_GProps()
        brepgprop_SurfaceProperties(faces[i].shape(), props)
        if abs(props.CentreOfMass().Z() - z) < 1e-6:
            same = pygeoml.is_same(faces[i])
            predicate = same if predicate is None else predicate | same
    return predicate


box = pygeoml.create_box(10.0, 10.0, 10.0)

# Select subshapes by type. Each subshape is returned once, wrapped in a
# compound Shape.
assert box.select_subshapes(is_vertex).size() == 8
assert box.select_subshapes(is_edge).size() == 12
assert box.select_subshapes(is_face).size() == 6

# unique_element() unwraps a selection with exactly one subshape
top = box.select_subshapes(is_face_at_height(box, 10.0)).unique_element()
bottom = box.select_subshapes(is_face_at_height(box, 0.0)).unique_element()

# Combine predicates with & (and), | (or) and ~ (not)
top_edges = box.select_subshapes(is_edge & pygeoml.is_subshape_of(top))
assert top_edges.size() == 4

vertical_edges = box.select_subshapes(
    is_edge & ~pygeoml.is_subshape_of(top) & ~pygeoml.is_subshape_of(bottom)
)
assert vertical_edges.size() == 4

# Select by topological relations: faces that share an edge with the top face
side_faces = box.select_subshapes(
    is_face
    & ~pygeoml.is_same(top)
    & pygeoml.has_subshape_that(is_edge & pygeoml.is_subshape_of(top))
)
assert side_faces.size() == 4

# Selections are Shapes themselves and can be searched further
bottom_vertices = side_faces.select_subshapes(is_vertex & pygeoml.is_subshape_of(bottom))
assert bottom_vertices.size() == 4

# Predicates can also be evaluated directly
assert is_face(top) and not is_edge(top)
assert pygeoml.has_subshape_that(is_edge)(top)

print("OK")
