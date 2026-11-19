.. _surfaces-api:

Surfaces
========

Surface functions are there to create B-spline surfaces, interpolate curve
networks and convert simple point definitions into OpenCASCADE surfaces or
faces.

Functions
---------

.. doxygenfunction:: geoml::interpolate_curve_network

.. doxygenfunction:: geoml::interpolate_curves

.. doxygenfunction:: geoml::revolving_shape

.. doxygenfunction:: geoml::nurbs_surface

.. doxygenfunction:: geoml::create_surface

.. doxygenfunction:: geoml::create_face

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``interpolate_curve_network(ucurves, vcurves, tolerance)``
     - Returns a ``Geom_BSplineSurface``. Create the curve lists with ``containers.geomcurve_vector``.
   * - ``interpolate_curves(curves, max_degree=3, join_continuously=False)``
     - Returns a ``Geom_BSplineSurface``. ``curves`` is created with ``containers.geomcurve_vector``.
   * - ``revolving_shape(profile_shape, start_point_rotation_axis, rotation_axis_direction, angle=2*pi)``
     - Returns a ``TopoDS_Shape``.
   * - ``nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, U_degree, V_degree, U_periodic=False, V_periodic=False)``
     - Returns a ``Geom_BSplineSurface``. ``control_points`` is an ``Array2dgp_Pnt``, ``weights`` an ``Array2dStandardReal``; create the knot and multiplicity lists with ``containers.standard_real_vector`` and ``containers.int_vector``.
   * - ``create_surface(p_1, p_2, p_3, p_4)``
     - Returns a ``Geom_Surface``.
   * - ``create_face(p_1, p_2, p_3, p_4)``
     - Returns a ``TopoDS_Face``.
