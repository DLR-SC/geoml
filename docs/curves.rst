Curves
======

This page gives a short introduction to the curve functionality and a few
small examples. For the full API reference see :ref:`curves-api` in the
API section.

Quick example
-------------

The following minimal example shows how to interpolate a small set of points
into a B-spline curve using the library. The full example source is included
below.

.. literalinclude:: examples/curve_example.cpp
   :language: cpp
   :caption: Interpolate points to a B-spline curve

Blend curve example
-------------------

Blend curves connect two input edges with a requested geometric continuity at
each end. The following example creates two interpolated curves, converts them
to edges and builds a transition curve between them.

.. literalinclude:: examples/blend_curve_example.cpp
   :language: cpp
   :caption: Create a blend curve between two edges

More
----

- **API reference:** :ref:`curves-api`
- **Usage and examples:** :doc:`usage`
- **Python examples:** :doc:`python_examples`
