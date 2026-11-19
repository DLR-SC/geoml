.. _utilities-api:

Utilities
=========

Utility functions expose common operations such as extracting B-spline control
points and creating fillets on history-aware shapes.

Functions
---------

.. doxygenfunction:: geoml::extract_control_points_surface

.. doxygenfunction:: geoml::extract_control_point_vector_in_U_direction

.. doxygenfunction:: geoml::extract_control_point_vector_in_V_direction

.. doxygenfunction:: geoml::make_fillet

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``extract_control_points_surface(b_spline_surface)``
     - Returns an ``Array2dgp_Pnt``.
   * - ``extract_control_point_vector_in_U_direction(b_spline_surface, j)``
     - Returns a ``CPointContainer``.
   * - ``extract_control_point_vector_in_V_direction(b_spline_surface, i)``
     - Returns a ``CPointContainer``.
   * - ``make_fillet(solid, edges, radius)``
     - Returns a ``Shape`` with history.
