.. _developer-guide:

Developer Guide
===============

This section describes the architecture of geoml for developers who want to
extend the library. It explains the structure of the repository, the public
geoml API and how to add modelling operations that track the history of
``geoml::Shape`` objects.

Repository layout
-----------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Directory
     - Content
   * - ``src/geoml/``
     - The **public API**. These headers are installed with geoml and are the
       only headers users include. One subdirectory per module, for example
       ``curves``, ``surfaces``, ``boolean_ops`` or ``naming_choosing``.
   * - ``src/geometry/``, ``src/topology/``, ``src/boolean_operations/``,
       ``src/math/``, ``src/common/``, ...
     - The **internal implementation**, largely taken over from
       `TiGL <https://github.com/DLR-SC/tigl>`_. These headers are not
       installed.
   * - ``bindings/python/``
     - The Python bindings, generated with SWIG from ``pygeoml.i``.
   * - ``tests/apitests/``
     - GoogleTest tests of the public API.
   * - ``tests/unittests/``
     - GoogleTest tests of the internal implementation.
   * - ``tests/python/``
     - pytest tests of the Python bindings.
   * - ``docs/``
     - This documentation (Sphinx, with the API reference extracted by
       Doxygen and Breathe).

The geoml API
-------------

The public API consists of free functions and a few classes in the namespace
``geoml``, declared in the headers below ``src/geoml/``. Each module has a
header and a source file next to each other, for example
``src/geoml/boolean_ops/modeling.hpp`` and
``src/geoml/boolean_ops/modeling.cpp``.

* Every public function is marked with ``GEOML_API_EXPORT``. The macro is
  defined in ``geoml/exports.h``, which CMake generates, and exports the
  function from the shared library.
* Every public function has a Doxygen comment. The :ref:`geoml-api` is
  generated from these comments.
* Public headers use only OpenCASCADE types and public geoml types, such as
  ``geoml::Shape`` or ``geoml::Array2d``. They must not include internal
  headers, because these are not installed.
* The source files implement the public functions, typically by calling the
  internal implementation. They may include internal headers, for example
  ``"topology/BRepBuilderAPI_MakeShape_Operation.hpp"``.

The modules are:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Module
     - Purpose
   * - ``curves``, ``surfaces``
     - Creation and interpolation of B-spline curves and surfaces
       (:ref:`curves-api`, :ref:`surfaces-api`).
   * - ``geom_topo_conversions``
     - Conversions between geometry and topology, e.g. curve to edge
       (:ref:`geom-topo-conversions-api`).
   * - ``naming_choosing``
     - ``geoml::Shape``, the history graph, meta tags and tag tracks
       (:ref:`naming-choosing-api`).
   * - ``predicates``
     - Predicates for selecting subshapes (:ref:`predicates-api`).
   * - ``primitives``
     - Primitive solids such as boxes and cylinders (:ref:`primitives-api`).
   * - ``boolean_ops``
     - Boolean operations on ``geoml::Shape`` with history
       (:ref:`boolean-ops-api`).
   * - ``transformations``
     - Transformations of shapes (:ref:`transformations-api`).
   * - ``utilities``
     - Further modelling and helper functions, e.g. ``make_fillet``
       (:ref:`utilities-api`).
   * - ``data_structures``
     - ``geoml::Array2d`` and conversions (:ref:`data-structures-api`).

How history tracking works
--------------------------

A ``geoml::Shape`` stores, for each of its subshapes, its *origins*: the
subshapes of the inputs of the last modelling operation it was created from.
The origins form the history graph, which the history predicates such as
``is_descendent_of`` search (see :ref:`robust-referencing`).

The origins are set by **operations**, classes derived from the class template
``geoml::Operation`` in ``src/topology/Operation.h``. ``Operation`` is a CRTP
base class: a derived class implements

* ``Shape perform()``, which performs the modelling operation and returns the
  result, and
* ``void map_subshapes(Shape& result)``, which sets the origins of the
  subshapes of the result with ``add_origin(result_subshape, input_subshape)``.

``Operation::value()`` calls both, then passes the tag tracks of the inputs on
to the result and applies them:

.. literalinclude:: ../src/topology/Operation.h
   :language: cpp
   :caption: src/topology/Operation.h (excerpt)
   :start-at: Shape value() {
   :end-before: protected:

``Shape`` grants ``Operation`` access to its internal data, so only operations
can set origins.

Most OpenCASCADE modelling algorithms derive from
``BRepBuilderAPI_MakeShape``, which reports the history of an operation with
``Modified()``. The operation ``geoml::BRepBuilderAPI_MakeShape_Operation`` in
``src/topology/`` wraps any such algorithm. It maps

* every subshape of the result that is the same as an input subshape to that
  input subshape (an *unmodified* descendent), and
* every subshape of the result that the algorithm reports in ``Modified()``
  of an input subshape to that input subshape (a *modified* descendent).

.. literalinclude:: ../src/topology/BRepBuilderAPI_MakeShape_Operation.cpp
   :language: cpp
   :caption: src/topology/BRepBuilderAPI_MakeShape_Operation.cpp (excerpt)
   :start-at: void BRepBuilderAPI_MakeShape_Operation::map_subshapes
   :end-before: } // namespace geoml

Subshapes that the algorithm creates from scratch, reported by
``Generated()`` in OpenCASCADE, are not mapped and have no origin. An example
is the new face of a fillet.

Where to put history-tracking functions
---------------------------------------

A modelling function whose result should be followed with history predicates
and tag tracks

* takes its input shapes as ``geoml::Shape`` and returns a ``geoml::Shape``,
* is declared in the public header of the module it belongs to, e.g.
  ``src/geoml/boolean_ops/modeling.hpp`` for Boolean operations or
  ``src/geoml/utilities/utilities.h`` for other modelling operations, and
* is implemented in the corresponding source file with an operation from
  ``src/topology/``.

If the OpenCASCADE algorithm derives from ``BRepBuilderAPI_MakeShape``, use
``BRepBuilderAPI_MakeShape_Operation``; this is the case for most algorithms
(Boolean operations, fillets, chamfers, offsets, sweeps, ...). Otherwise, add
a new operation to ``src/topology/`` that derives from ``Operation`` and
implements ``perform()`` and ``map_subshapes()``, using
``BRepBuilderAPI_MakeShape_Operation`` as template.

Functions that work on ``TopoDS_Shape``, such as ``translate``, do not record
history. Wrapping their result in a ``Shape`` starts a new history.

Example: adding ``boolean_common``
----------------------------------

The following example adds ``boolean_common``, the common part
(intersection) of two shapes, using the OpenCASCADE algorithm
``BRepAlgoAPI_Common``. It follows the existing ``boolean_subtract``:

.. literalinclude:: ../src/geoml/boolean_ops/modeling.cpp
   :language: cpp
   :caption: Existing: src/geoml/boolean_ops/modeling.cpp
   :start-at: Shape boolean_subtract
   :end-before: Shape operator-

**1. Declare the function** in the public header, with a Doxygen comment and
``GEOML_API_EXPORT``:

.. code-block:: cpp
   :caption: src/geoml/boolean_ops/modeling.hpp

   /**
    * @brief A function that returns the common part of two Shapes. This function has a history mapping.
    *
    * @param shape_1 The first Shape
    * @param shape_2 The second Shape
    */
   GEOML_API_EXPORT Shape boolean_common(Shape const& shape_1, Shape const& shape_2);

**2. Implement it** with ``BRepBuilderAPI_MakeShape_Operation``, exactly like
``boolean_subtract``. ``BRepAlgoAPI_Common`` derives from
``BRepBuilderAPI_MakeShape``, so no new operation is needed. The list
``{shape_1, shape_2}`` names the inputs whose subshapes the result is mapped
to and whose tag tracks are passed on:

.. code-block:: cpp
   :caption: src/geoml/boolean_ops/modeling.cpp

   #include "BRepAlgoAPI_Common.hxx"

   Shape boolean_common(Shape const& shape_1, Shape const& shape_2)
   {
       BRepAlgoAPI_Common common(shape_1, shape_2);
       auto operation = BRepBuilderAPI_MakeShape_Operation(common, {shape_1, shape_2});
       return operation.value();
   }

**3. Test the history** in ``tests/apitests/src``. The common part of a box
and a cylinder that passes through it consists of three faces: the lateral
face is a modified descendent of the cylinder's lateral face, the two discs
are modified descendents of the box's top and bottom faces.

.. code-block:: cpp
   :caption: tests/apitests/src/testBooleanOps.cpp

   TEST(BooleanOps, boolean_common_tracks_history)
   {
       using namespace geoml;
       Shape box = create_box(10., 10., 10.);
       Shape cylinder = translate(create_cylinder(3., 12.), gp_Vec(5., 5., -1.));

       Shape result = boolean_common(box, cylinder);

       EXPECT_EQ(result.select_subshapes(is_face).size(), 3);
       EXPECT_EQ(result.select_subshapes(
           is_face && is_modified_descendent_of_subshape_in(box)).size(), 2);
       EXPECT_EQ(result.select_subshapes(
           is_face && is_modified_descendent_of_subshape_in(cylinder)).size(), 1);
   }

**4. Make it available in Python.** ``bindings/python/pygeoml.i`` includes the
public headers with ``%include``, so a new function in an existing header is
wrapped automatically. For a new header, add it to the ``#include`` block and
to the ``%include`` list in ``pygeoml.i``. Add a test to
``tests/python/test_boolean_ops.py``:

.. code-block:: python
   :caption: tests/python/test_boolean_ops.py

   from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeCylinder
   from OCC.Core.gp import gp_Ax2, gp_Dir, gp_Pnt


   def test_boolean_common():
       box = pygeoml.create_box(10.0, 10.0, 10.0)
       axis = gp_Ax2(gp_Pnt(5.0, 5.0, -1.0), gp_Dir(0.0, 0.0, 1.0))
       cylinder = pygeoml.Shape(BRepPrimAPI_MakeCylinder(axis, 3.0, 12.0).Shape())

       result = pygeoml.boolean_common(box, cylinder)

       assert result.select_subshapes(
           pygeoml.is_face & pygeoml.is_modified_descendent_of_subshape_in(box)).size() == 2

**5. Document it**: add ``.. doxygenfunction:: geoml::boolean_common`` to
``docs/api/boolean_ops.rst`` and the function to the table of history-tracking
operations in :ref:`robust-referencing`.
