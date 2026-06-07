.. _geoml-api:

API Reference
=============

The geoml C++ API is organized around a small set of public headers in
``src/geoml``. The reference below mirrors that structure and documents the
types and functions exposed in namespace ``geoml``.

Most functions operate on OpenCASCADE geometry and topology types such as
``gp_Pnt``, ``Geom_Curve``, ``Geom_Surface`` and ``TopoDS_Shape``. Modeling
operations that need history-aware naming use :cpp:class:`geoml::Shape`.

.. toctree::
   :maxdepth: 2

   api/core
   api/curves
   api/surfaces
   api/primitives
   api/boolean_ops
   api/transformations
   api/naming_choosing
   api/predicates
   api/geom_topo_conversions
   api/data_structures
   api/utilities
