.. _geoml-api:

API Reference
=============

The geoml C++ API is organised around a small set of public headers in
``src/geoml``. The reference pages, listed in the navigation sidebar, mirror
that structure and document the types and functions exposed in namespace
``geoml``.

Most functions operate on OpenCASCADE geometry and topology types such as
``gp_Pnt``, ``Geom_Curve``, ``Geom_Surface`` and ``TopoDS_Shape``. Modelling
operations that need history-aware naming use :cpp:class:`geoml::Shape`.

.. The list of reference pages is only shown in the navigation sidebar.

.. toctree::
   :maxdepth: 2
   :hidden:

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
