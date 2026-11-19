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

Python
------

Available in Python as ``from geoml import pygeoml``.

The conversion functions ``Array2d_to_TCol``, ``TCol_to_Array2d``,
``StdVector_to_TCol`` and ``TCol_to_StdVector`` are not available in Python.
The module ``geoml.occ_helpers.containers`` provides helpers to create the
container types from Python lists and to convert them:

* ``point_vector(points)``, ``standard_real_vector(numbers)``,
  ``int_vector(integers)`` and ``geomcurve_vector(curves)`` create the
  ``std::vector`` types from Python lists,
* ``point_array``, ``float_array``, ``int_array`` and ``bspline_array``
  create OpenCASCADE arrays from Python lists,
* ``point_vector_to_tcol``, ``point_tcol_to_vector``, ``float_vector_to_tcol``,
  ``float_tcol_to_vector``, ``point_array_2d_to_tcol``,
  ``point_tcol_to_array_2d``, ``float_array_2d_to_tcol`` and
  ``float_tcol_to_array_2d`` convert between them,
* ``iter_array(occ_array)`` iterates over an OpenCASCADE array.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``Array2dgp_Pnt(rows, cols)``, ``Array2dStandardReal(rows, cols)``
     - ``geoml::Array2d`` of points and of numbers, with ``setValue(i, j, value)``, ``at(i, j)``, ``rowLength()``, ``colLength()``, ``getRow(i)`` and ``getCol(j)``.
   * - ``CPointContainer``, ``StandardRealList``, ``IntList``, ``CurveList``, ``ShapeList``
     - The ``std::vector`` types used by the functions, for points, numbers, integers, curves and shapes.
