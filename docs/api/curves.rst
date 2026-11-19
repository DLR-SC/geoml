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

Python
------

Available in Python as ``from geoml import pygeoml``.

``containers`` is the module ``geoml.occ_helpers.containers``, see
:ref:`data-structures-api`.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``nurbs_curve(control_points, weights, knots, multiplicities, degree, periodic=False)``
     - Returns a ``Geom_BSplineCurve``. Create the lists with ``containers.point_vector``, ``containers.standard_real_vector`` and ``containers.int_vector``.
   * - ``interpolate_points_to_b_spline_curve(points, degree=3, continuousIfClosed=False, parameters=...)``
     - Returns a ``Geom_BSplineCurve``. ``points`` is created with ``containers.point_vector``.
   * - ``blend_curve(start, end)``
     - Returns the blend curve between two connections as ``TopoDS_Edge``.
   * - ``BlendCurveConnection(edge, near_connection_point, continuity, outward_direction=True, beta=1.0, gamma=1.0)``
     - ``continuity`` is one of ``GContinuity_G0``, ``GContinuity_G1``, ``GContinuity_G2``.
   * - ``BlendCurve(start, end)``
     - ``blend_curve()`` returns the blend curve as ``TopoDS_Edge``.
