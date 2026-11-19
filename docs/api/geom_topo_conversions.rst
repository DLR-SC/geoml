.. _geom-topo-conversions-api:

Geometry and Topology Conversions
=================================

Conversion helpers bridge OpenCASCADE geometric entities and topological
entities, for example curves to edges and surfaces to faces.

Functions
---------

.. doxygenfunction:: geoml::SurfaceToFace

.. doxygenfunction:: geoml::CurveToEdge

.. doxygenfunction:: geoml::EdgeToCurve

Python
------

Available in Python as ``from geoml import pygeoml``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``SurfaceToFace(surface, tolDegEd)``
     - Returns a ``TopoDS_Face``.
   * - ``CurveToEdge(curve)``
     - Returns a ``TopoDS_Edge``.
   * - ``EdgeToCurve(edge)``
     - Returns a ``Geom_Curve``.
