.. _shapes-examples:

Selecting, Tracking and Tagging Subshapes
=========================================

``geoml::Shape`` wraps an OpenCASCADE ``TopoDS_Shape`` and additionally stores
the modelling history and metadata tags of the shape and its subshapes. This
page shows how to

* select subshapes with **predicates**,
* follow subshapes through modelling operations with **history predicates**, and
* name subshapes with **tags** and keep the names alive with **tag tracks**.

For the full API see :ref:`naming-choosing-api` and :ref:`predicates-api`.

Concepts
--------

**Predicates** are functions that take a ``geoml::Shape`` and return a
Boolean, wrapped in ``geoml::ShapePredicate``. ``select_subshapes(predicate)``
returns all subshapes satisfying the predicate as a compound ``Shape``.
Predicates can be combined with ``&&``, ``||`` and ``!``. geoml provides

* type predicates: ``is_vertex``, ``is_edge``, ``is_face``, ``is_solid``,
  ``is_type(...)``,
* topological predicates: ``is_same``, ``is_subshape_of``, ``has_subshape``,
  ``has_subshape_that``,
* history predicates: ``is_descendent_of``, ``is_modified_descendent_of``,
  ``is_unmodified_descendent_of`` and their ``..._subshape_in`` variants, which
  test against all subshapes of a shape,
* tag predicates: ``has_tag``.

Any callable with the signature ``bool(geoml::Shape const&)`` can be wrapped in
a ``geoml::ShapePredicate``, for example to select faces by geometric
properties.

**History.** Modelling operations such as ``boolean_subtract`` (``operator-``)
and ``boolean_union`` (``operator+``) record for every subshape of the result
which input subshapes it originates from, and whether it was modified. The
history predicates use this information, also over several modelling steps.

**Meta tags** are strings attached to a subshape. They belong to the subshape
object itself and are therefore not part of the result of a modelling operation.

**Tag tracks** make tags persistent: a ``geoml::TagTrack`` consists of a tag, a
predicate and a number of modelling steps. Each modelling operation passes the
tag tracks of its inputs on to the result and tags all subshapes of the result
that satisfy the predicate, until the number of steps is used up.

.. note::

   Do not pick subshapes by their index in a selection, for example
   ``box.select_subshapes(is_edge)[0]``. The order of a selection depends on
   memory addresses and changes between program runs. Select subshapes by their
   properties and relations instead, as shown below.

Building the examples
---------------------

Each example is a complete program. It checks its results, prints them and
returns a non-zero exit code if a check fails. Build and run it like the
example in :ref:`installation` (section *Use geoml from C++*), using the
example as ``main.cpp``. The examples need the OpenCASCADE headers, which the
``CMakeLists.txt`` shown there adds with ``${OpenCASCADE_INCLUDE_DIR}``.

Selecting subshapes with predicates
-----------------------------------

Select the vertices, edges and faces of a box by type, by a custom geometric
predicate and by their topological relations.

.. literalinclude:: examples/shape_selection_example.cpp
   :language: cpp
   :caption: Select subshapes with predicates

Tracking subshapes through modelling operations
-----------------------------------------------

Drill a hole through a box and cut away a corner. History predicates find the
faces of the result that originate from the top face of the box, from the wall
of the drill or from the corner tool.

.. literalinclude:: examples/shape_history_example.cpp
   :language: cpp
   :caption: Track subshapes with history predicates

Tagging subshapes and tag tracks
--------------------------------

Meta tags name subshapes of a shape, but are not part of the result of a
modelling operation. Tag tracks re-apply a tag after each of the next modelling
operations and are a simple way to give subshapes a persistent name.

.. literalinclude:: examples/shape_tagging_example.cpp
   :language: cpp
   :caption: Tag subshapes and use tag tracks
