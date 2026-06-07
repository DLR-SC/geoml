.. _predicates-api:

Predicates
==========

Predicates provide composable selection criteria for ``geoml::Shape``. They are
intended for use with shape filtering, history queries and persistent tagging.

Logical Composition
-------------------

.. doxygenfunction:: geoml::operator&&

.. doxygenfunction:: geoml::operator||

.. doxygenfunction:: geoml::operator!

Basic Predicates
----------------

.. doxygenvariable:: geoml::is_vertex

.. doxygenvariable:: geoml::is_edge

.. doxygenvariable:: geoml::is_face

.. doxygenvariable:: geoml::is_solid

.. doxygenvariable:: geoml::has_origin

.. doxygenfunction:: geoml::has_tag

.. doxygenfunction:: geoml::is_type

.. doxygenfunction:: geoml::is_same(Shape const&)

.. doxygenfunction:: geoml::is_same(TopoDS_Shape const&)

Topology and History Predicates
-------------------------------

.. doxygenfunction:: geoml::has_subshape

.. doxygenfunction:: geoml::is_subshape_of

.. doxygenfunction:: geoml::is_unmodified_descendent_of

.. doxygenfunction:: geoml::is_unmodified_descendent_of_subshape_in

.. doxygenfunction:: geoml::is_modified_descendent_of

.. doxygenfunction:: geoml::is_modified_descendent_of_subshape_in

.. doxygenfunction:: geoml::is_descendent_of_subshape_in

.. doxygenfunction:: geoml::is_descendent_of

.. doxygenfunction:: geoml::has_subshape_that
