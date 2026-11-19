.. _transformations-api:

Transformations
===============

Class
-----

.. doxygenclass:: geoml::Transform
   :members:

Functions
---------

.. doxygenfunction:: geoml::operator *

.. doxygenfunction:: geoml::translate(TopoDS_Shape const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::translate(gp_Pnt const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::translate(std::vector<gp_Pnt> const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::repeat_shape

Python
------

Available in Python as ``from geoml import pygeoml``.

``translate`` and ``repeat_shape`` are not available in Python. Use
pythonocc, e.g. ``gp_Trsf`` and ``BRepBuilderAPI_Transform``, instead.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``Transform()``, ``Transform(gp_Trsf)``, ``Transform(gp_GTrsf)``, ``Transform(gp_Vec)``
     - Creates a transformation.
   * - ``t1 * t2``
     - Concatenates two transformations.
   * - ``t.pre_multiply(other)``, ``t.Set(other)``
     - As in C++.
