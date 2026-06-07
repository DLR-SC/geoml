.. _usage:

Usage
=====

Geoml can be used both via C++ and via Python.

It offers geometric functionalities for curves, surfaces and solids. Further it allows the formulation of 
subshape selection criteria using history tracking, to formulate robust parametric models.

C++
---

The best way to learn about the usage of the library is to look how it is used in 
the `api-tests <https://github.com/DLR-SC/geoml/tree/main/tests/apitests/src>`_ .

Python
------

The Python bindings are exposed through ``geoml.pygeoml``. Most functions map
directly to the C++ API, while ``geoml.occ_helpers.containers`` provides helper
functions for converting Python lists to the vector-like container types used by
the bindings.

See :doc:`python_examples` for small runnable snippets. Additional usage of the
Python bindings can be seen in the
`python-tests <https://github.com/DLR-SC/geoml/tree/main/tests/python>`_.
