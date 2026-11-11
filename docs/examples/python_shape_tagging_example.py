from geoml import pygeoml
from geoml.pygeoml import is_edge, is_face, has_tag
from OCC.Core.BRepAdaptor import BRepAdaptor_Surface
from OCC.Core.BRepGProp import brepgprop_SurfaceProperties
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox, BRepPrimAPI_MakeCylinder
from OCC.Core.GeomAbs import GeomAbs_Cylinder
from OCC.Core.GProp import GProp_GProps
from OCC.Core.gp import gp_Ax2, gp_Dir, gp_Pnt


def select_faces(shape, condition):
    """Select the faces of `shape` for which condition(TopoDS_Face) is true."""
    faces = shape.select_subshapes(is_face)
    predicate = None
    for i in range(faces.size()):
        if condition(faces[i].shape()):
            same = pygeoml.is_same(faces[i])
            predicate = same if predicate is None else predicate | same
    return shape.select_subshapes(predicate)


def at_height(z):
    def condition(face):
        props = GProp_GProps()
        brepgprop_SurfaceProperties(face, props)
        return abs(props.CentreOfMass().Z() - z) < 1e-6
    return condition


def is_cylindrical(face):
    return BRepAdaptor_Surface(face).GetType() == GeomAbs_Cylinder


def make_drill():
    axis = gp_Ax2(gp_Pnt(5.0, 5.0, -1.0), gp_Dir(0.0, 0.0, 1.0))
    return pygeoml.Shape(BRepPrimAPI_MakeCylinder(axis, 2.0, 12.0).Shape())


def make_corner_tool(x, y):
    return pygeoml.Shape(BRepPrimAPI_MakeBox(gp_Pnt(x, y, 8.0), 3.0, 3.0, 3.0).Shape())


# ----------------------------------------------------------------------
# 1. Meta tags: attach names to subshapes and select them later
# ----------------------------------------------------------------------
box = pygeoml.create_box(10.0, 10.0, 10.0)

top = select_faces(box, at_height(10.0)).unique_element()
bottom = select_faces(box, at_height(0.0)).unique_element()

top.add_meta_tag("top")                                            # tag a single shape
box.add_meta_tag_to_subshapes(pygeoml.is_same(bottom), "bottom")   # tag all matching subshapes
pygeoml.add_persistent_meta_tag_to_subshapes(box, is_edge, "edge")  # same as a free function

assert box.select_subshapes(has_tag("top")).size() == 1
assert box.select_subshapes(has_tag("top") | has_tag("bottom")).size() == 2
assert box.select_subshapes(has_tag("edge")).size() == 12

# Meta tags belong to the subshapes of this shape. A modeling operation
# creates new subshapes, so the tags are not part of the result.
drilled = box - make_drill()
assert drilled.select_subshapes(has_tag("top")).size() == 0

# ----------------------------------------------------------------------
# 2. Tag tracks: re-apply a tag after each of the next N operations
# ----------------------------------------------------------------------
part = pygeoml.create_box(10.0, 10.0, 10.0)
drill = make_drill()

part_top = select_faces(part, at_height(10.0)).unique_element()
drill_wall = select_faces(drill, is_cylindrical).unique_element()

# A tag track stores a tag, a criterion and the number of modeling steps it
# stays active. Its criterion is evaluated on the result of every operation
# that has the shape as input, so it can use history predicates to find the
# subshapes that originate from the tagged input subshapes.
part.add_tag_track(pygeoml.TagTrack("top", is_face & pygeoml.is_descendent_of(part_top), 2))
drill.add_tag_track(pygeoml.TagTrack("bore", is_face & pygeoml.is_descendent_of(drill_wall), 2))

# apply_tag_tracks() also tags the input shape itself
part.apply_tag_tracks()
assert part.select_subshapes(has_tag("top")).size() == 1

# Step 1: the tag tracks of both inputs are passed on to the result
step1 = part - drill
assert step1.select_subshapes(has_tag("top")).size() == 1
assert step1.select_subshapes(has_tag("bore")).size() == 1

# Step 2: the tags are still applied, although the top face is modified again
step2 = step1 - make_corner_tool(8.0, 8.0)
assert step2.select_subshapes(has_tag("top")).size() == 1
assert step2.select_subshapes(has_tag("bore")).size() == 1

# Step 3: the tag tracks were limited to two steps and have expired
step3 = step2 - make_corner_tool(-1.0, -1.0)
assert step3.select_subshapes(has_tag("top")).size() == 0

# Tags can be combined with all other predicates, e.g. the circular edge
# where the bore meets the top face
bore = step2.select_subshapes(has_tag("bore")).unique_element()
top_face = step2.select_subshapes(has_tag("top")).unique_element()
rim = step2.select_subshapes(
    is_edge & pygeoml.is_subshape_of(bore) & pygeoml.is_subshape_of(top_face)
)
assert rim.size() == 1

print("OK")
