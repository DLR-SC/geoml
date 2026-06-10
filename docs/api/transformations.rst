.. _transformations-api:

Transformations
===============

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
