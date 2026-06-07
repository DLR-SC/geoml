.. _curves-api:

Curves
======

Curve functions create and interpolate OpenCASCADE B-spline and NURBS curves.
Blend helpers construct transition curves between edges with prescribed
continuity.

Functions
---------

.. doxygenfunction:: geoml::nurbs_curve

.. doxygenfunction:: geoml::interpolate_points_to_b_spline_curve

.. doxygenfunction:: geoml::blend_curve

Blend Curves
------------

.. doxygenclass:: geoml::BlendCurveConnection
   :members:

.. doxygenclass:: geoml::BlendCurve
   :members:
