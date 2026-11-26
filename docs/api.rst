.. `toctree`

.. _geoml-api:

*************
API Reference
*************

The GeoML API includes :ref:`surfacemodel-api` and :ref:`metadata-api`.

.. _basic-api:
.. doxygenfunction:: geoml::init    
.. doxygenclass:: geoml::Error

.. _surfacemodel-api:

Surface Modeling
----------------

.. doxygenfile:: surfaces.h

.. _metadata-api:

Shape metadata
--------------

See :ref:`metadata-usage` for a worked modeling example.

.. doxygenclass:: geoml::metadata::Key
   :members:

.. doxygenclass:: geoml::metadata::Metadata
   :members: set, contains, find, at, erase, entries

.. doxygenclass:: geoml::ModelingContext
   :members:

.. doxygenclass:: geoml::TrackLifetime
   :members:

.. doxygenclass:: geoml::MetadataTrackHandle
   :members:

.. _metadata-resolvers-api:

Metadata resolvers
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: geoml::metadata::register_default_resolver

.. doxygenclass:: geoml::metadata::MetadataPolicy
   :members: for_key

.. doxygenclass:: geoml::metadata::MetadataScope
   :members:

.. doxygenfunction:: geoml::metadata::prefer_input

.. doxygenstruct:: geoml::metadata::ResolutionContext
   :members:

.. doxygenstruct:: geoml::metadata::ResolutionContribution
   :members:

.. doxygenclass:: geoml::metadata::MetadataConflictException
   :members: key_name, value_type, existing_value, incoming_value, contributions, target_topology
