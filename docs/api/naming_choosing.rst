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
