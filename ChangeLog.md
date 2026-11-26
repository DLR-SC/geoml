# Changelog

## Changes since last release

  * General changes:
    * Add Sphinx/Doxygen documentation for the C++ API and usage examples (#3)
    * Add typed metadata keys, per-entity metadata storage on `geoml::Shape`, conflict resolvers, and structured conflict exceptions (#74)
    * Introduce `geoml::ModelingContext` to manage metadata tracks and scoped conflict resolution policies outside of `geoml::Shape` (#74, #94)
    * Add optional `ModelingContext&` overloads to `boolean_union()`, `boolean_subtract()`, and `make_fillet()` (#74)
  * Breaking API changes:
    * Removed `geoml::TagTrack` and shape-owned tracking methods (`Shape::add_tag_track()`, `Shape::apply_tag_tracks()`, `Shape::get_tag_tracks()`). Register tag and metadata tracks through `geoml::ModelingContext::add_tag_track()` / `add_metadata_track()` and pass the context to modeling operations instead (#74, #94)
    * Removed Python bindings for `TagTrack`, `Shape.add_tag_track()`, and `Shape.apply_tag_tracks()` pending dedicated Python bindings for `ModelingContext` (#74, #94)

## Version 0.1.0

Initial geoml development version.

  * General changes:
    * Extract C++ geometry functions from TiGL
    * Provide Python bindings for selected geoml functionality.
    * Add CMake support for building the library, tests, bindings and documentation.
    * Add history tracking framework, allowing persistent tracking and meta data (tags) annotation
    * Add blending curves
    * Add geometric functionality, 2d-data type and conversion functions

