from geoml import pygeoml
from geoml.pygeoml import is_edge, is_face
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


# Input shapes: a box and a cylinder that drills a hole through it.
# OpenCASCADE shapes from pythonocc can be wrapped in a geoml Shape.
box = pygeoml.create_box(10.0, 10.0, 10.0)
drill = pygeoml.Shape(
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(5.0, 5.0, -1.0), gp_Dir(0.0, 0.0, 1.0)), 2.0, 12.0).Shape()
)

# Pick the subshapes we want to follow through the modelling operations
top = select_faces(box, at_height(10.0)).unique_element()
drill_wall = select_faces(drill, is_cylindrical).unique_element()

# Modelling step 1: drill the hole
drilled = box - drill

# The top face got a hole: it is a *modified* descendent of the original top face
drilled_top = drilled.select_subshapes(is_face & pygeoml.is_descendent_of(top)).unique_element()
assert drilled_top.is_modified_descendent_of(top)

# Faces of the box that were not touched by the drill
unmodified = drilled.select_subshapes(is_face & pygeoml.is_unmodified_descendent_of_subshape_in(box))
modified = drilled.select_subshapes(is_face & pygeoml.is_modified_descendent_of_subshape_in(box))
assert unmodified.size() == 4
assert modified.size() == 2

# The wall of the hole originates from the cylinder's lateral face
hole_wall = drilled.select_subshapes(is_face & pygeoml.is_descendent_of(drill_wall)).unique_element()

# The edge where the hole meets the top face
rim = drilled.select_subshapes(
    is_edge & pygeoml.is_subshape_of(hole_wall) & pygeoml.is_subshape_of(drilled_top)
)
assert rim.size() == 1

# Modelling step 2: cut away a corner of the box
corner = pygeoml.Shape(BRepPrimAPI_MakeBox(gp_Pnt(8.0, 8.0, 8.0), 3.0, 3.0, 3.0).Shape())
result = drilled - corner

# History predicates follow the history over several modelling steps
assert result.select_subshapes(is_face & pygeoml.is_descendent_of(top)).size() == 1
assert result.select_subshapes(is_face & pygeoml.is_descendent_of(drill_wall)).size() == 1
assert result.select_subshapes(is_face & pygeoml.is_descendent_of_subshape_in(corner)).size() == 3

# Faces of the result that originate from neither tool
box_only = result.select_subshapes(
    is_face
    & ~pygeoml.is_descendent_of_subshape_in(drill)
    & ~pygeoml.is_descendent_of_subshape_in(corner)
)
assert box_only.size() == 6

print("OK")
