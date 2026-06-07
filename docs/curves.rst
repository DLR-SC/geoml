Curves
======

This page gives a short introduction to the curve functionality and a few
small examples. For the full API reference see :ref:`curves-api` in the
API section.

Overview
--------

Geoml supports creation and manipulation of B-spline and NURBS curves, as
well as utility routines such as interpolation and parameter computation.

Quick example
-------------

The following minimal example shows how to interpolate a small set of points
into a B-spline curve using the library. The full example source is included
below.

.. literalinclude:: examples/curve_example.cpp
   :language: cpp
   :caption: Interpolate points to a B-spline curve

More
----

- **API reference:** :ref:`curves-api`
- **Usage and examples:** :doc:`usage`
