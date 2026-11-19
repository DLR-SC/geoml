.. _core-api:

Core
====

Core headers define library initialisation, error handling and shared enum
types used by the rest of the public API.

Initialisation
--------------

.. doxygenfunction:: geoml::init

Errors
------

.. doxygenenum:: geoml::ReturnCode

.. doxygenclass:: geoml::Error
   :members:

Continuity
----------

.. doxygenenum:: geoml::GContinuity

Python
------

Available in Python as ``from geoml import pygeoml``.

``geoml::ReturnCode`` and ``geoml::Error`` are not available in Python. A
``geoml::Error`` is raised as a Python exception that depends on its return
code: ``INDEX_ERROR`` as ``IndexError``, ``MATH_ERROR`` as ``ArithmeticError``,
``NULL_POINTER`` as ``ValueError`` and all other codes as ``RuntimeError``.
OpenCASCADE errors are raised as ``RuntimeError``.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - ``pygeoml``
     - Description
   * - ``init()``
     - Initialises the library.
   * - ``GContinuity_G0``, ``GContinuity_G1``, ``GContinuity_G2``
     - The values of ``geoml::GContinuity`` as module constants.
