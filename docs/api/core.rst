.. _core-api:

Core
====

Core headers define library initialization, error handling and shared enum
types used by the rest of the public API.

Initialization
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
