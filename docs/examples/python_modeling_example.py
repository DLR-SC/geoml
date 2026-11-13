from geoml import pygeoml


box = pygeoml.create_box(10.0, 10.0, 10.0)
cylinder = pygeoml.create_cylinder(3.0, 12.0)

cut_shape = pygeoml.boolean_subtract(box, cylinder)
union_shape = box + cylinder

edges = cut_shape.select_subshapes(pygeoml.is_edge)
tagged_edges = cut_shape.select_subshapes(pygeoml.has_tag("inlet"))
