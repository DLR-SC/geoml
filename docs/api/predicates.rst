.. _predicates-api:

Predicates
==========

Predicates provide composable selection criteria for ``geoml::Shape``. They are
intended for use with shape filtering, history queries and persistent tagging.

Logical Composition
-------------------

.. doxygenfunction:: geoml::operator &&

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

Python
------

Available in Python as ``from geoml import pygeoml``.

Python functions cannot be used as predicates. To select subshapes by
geometric properties, find them with pythonocc and combine ``is_same``
predicates, as shown in :ref:`python-examples`.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``p & q``, ``p | q``, ``~p``
     - Logical and, or and not (``&&``, ``||`` and ``!`` in C++).
   * - ``predicate(shape)``
     - Evaluates a predicate for a shape and returns a ``bool``.
   * - ``is_vertex``, ``is_edge``, ``is_face``, ``is_solid``, ``has_origin``
     - Predicate constants.
   * - ``has_tag(tag)``, ``is_type(shape_type)``, ``is_same(shape)``
     - Return a ``ShapePredicate``.
   * - ``has_subshape(shape)``, ``is_subshape_of(shape)``, ``has_subshape_that(predicate)``
     - Return a ``ShapePredicate``.
   * - ``is_descendent_of(shape, max_depth=...)``, ``is_modified_descendent_of``, ``is_unmodified_descendent_of`` and their ``..._subshape_in`` variants
     - Return a ``ShapePredicate``. ``max_depth`` is optional.
