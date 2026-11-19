.. _boolean-ops-api:

Boolean Operations
==================

Boolean operations combine or subtract ``geoml::Shape`` instances and preserve
modelling history for robust downstream subshape selection.

Functions
---------

.. doxygenfunction:: geoml::boolean_subtract

.. doxygenfunction:: geoml::operator-

.. doxygenfunction:: geoml::boolean_union

.. doxygenfunction:: geoml::operator+

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``boolean_subtract(shape, cutting_tool)``, ``shape - cutting_tool``
     - Returns a ``Shape`` with history.
   * - ``boolean_union(shape_1, shape_2)``, ``shape_1 + shape_2``
     - Returns a ``Shape`` with history.
