.. _python-examples:

Python Examples
===============

The Python bindings expose the C++ API through ``geoml.pygeoml``.

For more usage patterns, see the
`Python tests <https://github.com/DLR-SC/geoml/tree/main/tests/python>`_;
they can be used as additional application examples and as orientation for how
geoml is used in practice.

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

Modelling Operations
--------------------

.. literalinclude:: examples/python_modeling_example.py
   :language: python
   :caption: Create and combine history-aware shapes

Selecting, Tracking and Tagging Subshapes
-----------------------------------------

The following examples are the Python versions of the C++ examples in
:ref:`shapes-examples`, which also explains the concepts of predicates,
history, meta tags and tag tracks. Each example checks its results with
``assert`` and prints ``OK``.

Compared to C++, note the following:

* Predicates are combined with ``&`` (and), ``|`` (or) and ``~`` (not).
* Python functions cannot be used as predicates. To select subshapes by
  geometric properties, find them with pythonocc and combine
  ``pygeoml.is_same`` predicates, as the helper functions in the examples do.
* Iterate over a selection with ``for i in range(selection.size())`` and
  ``selection[i]``. Iterating over a ``Shape`` directly does not stop at the
  last subshape.
* OpenCASCADE shapes created with pythonocc can be wrapped with
  ``pygeoml.Shape(topods_shape)``, and ``shape.shape()`` returns the wrapped
  OpenCASCADE shape.

Selecting subshapes with predicates
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: examples/python_shape_selection_example.py
   :language: python
   :caption: Select subshapes with predicates

Tracking subshapes through modelling operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: examples/python_shape_history_example.py
   :language: python
   :caption: Track subshapes with history predicates

Tagging subshapes and tag tracks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. literalinclude:: examples/python_shape_tagging_example.py
   :language: python
   :caption: Tag subshapes and use tag tracks
