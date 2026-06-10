Surfaces
========

This page gives a short introduction to surface functionality and a few
small examples. For the full API reference see :ref:`surfaces-api` in the
API section.

Quick example
-------------

The following minimal example shows how to create a simple NURBS surface
from a small control point grid. The full example source is included below.

.. literalinclude:: examples/surface_example.cpp
   :language: cpp
   :caption: Create a simple NURBS surface

Curve network example
---------------------

``interpolate_curve_network`` creates a Gordon surface from two sets of
crossing curves. Each ``u`` curve must intersect each ``v`` curve within the
given tolerance.

.. literalinclude:: examples/curve_network_example.cpp
   :language: cpp
   :caption: Interpolate a surface through a curve network

More
----

- **API reference:** :ref:`surfaces-api`
- **Usage and examples:** :doc:`usage`
- **Python examples:** :doc:`python_examples`
