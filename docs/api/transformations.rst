.. _transformations-api:

Transformations
===============

Transformation helpers translate geometry directly or apply reusable
``Transform`` objects to points, vectors, shapes and surfaces.

Class
-----

.. doxygenclass:: geoml::Transform
   :members:

Functions
---------

.. doxygenfunction:: geoml::operator*

.. doxygenfunction:: geoml::translate(TopoDS_Shape const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::translate(gp_Pnt const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::translate(std::vector<gp_Pnt> const&, gp_Vec const&, double)

.. doxygenfunction:: geoml::repeat_shape
