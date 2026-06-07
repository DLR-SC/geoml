.. _python-examples:

Python Examples
===============

The Python bindings expose the C++ API through ``geoml.pygeoml``. Container
helpers in ``geoml.occ_helpers.containers`` convert Python lists into the
vector-like types expected by the bindings.

Curves and Shapes
-----------------

.. literalinclude:: examples/python_curve_example.py
   :language: python
   :caption: Interpolate a curve and wrap it as a Shape

Blend Curve
-----------

.. literalinclude:: examples/python_blend_curve_example.py
   :language: python
   :caption: Create a blend curve between two edges

Curve Network Surface
---------------------

.. literalinclude:: examples/python_curve_network_example.py
   :language: python
   :caption: Interpolate a surface through a curve network

Modeling Operations
-------------------

.. literalinclude:: examples/python_modeling_example.py
   :language: python
   :caption: Create and combine history-aware shapes
