.. _curves-api:

Curves
======

Curve functions are for the creation of B-spline curves. Blend curves allow to create geometric continuous transition between two given curves.

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
