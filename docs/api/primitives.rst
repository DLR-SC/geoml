.. _primitives-api:

Primitives
==========

Primitive modelling functions create history-aware ``geoml::Shape`` objects for
common solids.

Functions
---------

.. doxygenfunction:: geoml::create_cylinder

.. doxygenfunction:: geoml::create_box

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``create_cylinder(radius, height)``
     - Returns a ``Shape``.
   * - ``create_box(dx, dy, dz)``
     - Returns a ``Shape``.
