.. _data-structures-api:

Data Structures
===============

Data structure helpers provide lightweight containers and conversion utilities
used by the public API, especially for point grids and OpenCASCADE collection
types.

Containers
----------

.. doxygenclass:: geoml::Array2d
   :members:

Conversions
-----------

.. doxygenfunction:: geoml::Array2d_to_TCol

.. doxygenfunction:: geoml::TCol_to_Array2d

.. doxygenfunction:: geoml::StdVector_to_TCol

.. doxygenfunction:: geoml::TCol_to_StdVector
