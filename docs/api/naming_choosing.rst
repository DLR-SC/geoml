.. _naming-choosing-api:

Naming and Choosing
===================

The naming and choosing API wraps OpenCASCADE topology in ``geoml::Shape``. It
tracks topological children, modelling history and metadata tags so generated
shapes can be selected robustly after modelling operations.

Predicates and Tags
-------------------

.. doxygenclass:: geoml::ShapePredicate
   :members:

.. doxygenstruct:: geoml::TagTrack
   :members:

Shape
-----

.. doxygenclass:: geoml::Shape
   :members:

Helpers
-------

.. doxygenfunction:: geoml::add_persistent_meta_tag_to_subshapes

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``Shape()``, ``Shape(topods_shape)``
     - Wraps an OpenCASCADE shape. ``shape.shape()`` returns the wrapped ``TopoDS_Shape``.
   * - ``Shape`` methods
     - The same names as in C++, e.g. ``select_subshapes``, ``filter``, ``unique_element``, ``add_meta_tag``, ``add_meta_tag_to_subshapes``, ``add_tag_track``, ``apply_tag_tracks``, ``is_descendent_of``, ``has_tag``, ``size``. ``shape[i]`` returns the i-th direct subshape. Iterate with ``for i in range(shape.size())``; iterating over a ``Shape`` directly does not stop at the last subshape.
   * - ``shape_1 + shape_2``, ``shape - tool``
     - Boolean union and subtraction, see :ref:`boolean-ops-api`.
   * - ``TagTrack(tag, criterion, remaining_steps)``
     - The attributes are ``m_tag``, ``m_criterion`` and ``m_remainingSteps``.
   * - ``ShapePredicate``
     - Combine predicates with ``&``, ``|`` and ``~``; ``predicate(shape)`` evaluates a predicate. Python functions cannot be wrapped as predicates, see :ref:`predicates-api`.
   * - ``add_persistent_meta_tag_to_subshapes(shape, predicate, tag)``
     - As in C++.
