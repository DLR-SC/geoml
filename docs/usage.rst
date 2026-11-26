Usage
=====

.. _metadata-usage:

Metadata on a shape
-------------------

Select one wing edge and give it a tag. A new selection of the same edge
in this model—including through an adjacent face—sees the same metadata.

.. code-block:: cpp

   namespace meta = geoml::metadata;

   auto edge = wing.select_subshapes(edge_selector).unique_element();
   auto const& tags = meta::keys::tags();
   edge.metadata().set(tags, {"wing-edge"});
   bool tagged = edge.has_tag("wing-edge");

``set()`` stores the supplied value on that shape, including duplicate tags.
Register a track on a ``geoml::ModelingContext`` to carry metadata through
modeling operations. ``find(key)`` returns an optional copy, and ``at(key)``
borrows a value or throws ``std::out_of_range`` if it is missing. After
replacing or erasing a value, look it up again before using a reference from
``at()``. After a successful merge, borrow values and iterators from the store
again. See the :ref:`metadata-api` for the full C++ reference.

Following an edge through modeling
----------------------------------

.. code-block:: cpp

   #include <geoml/boolean_ops/modeling.hpp>
   #include <geoml/modeling/ModelingContext.h>

   geoml::ModelingContext context;

   auto descendants = geoml::is_edge && geoml::is_descendent_of(edge);
   auto track = context.add_tag_track(wing, descendants, "wing-edge");
   context.add_metadata_track(wing, descendants, meta::keys::name(), "wing-edge");

   auto aircraft = geoml::boolean_union(wing, fuselage, context);
   auto wing_edges = aircraft.select_subshapes(
       geoml::is_edge && geoml::has_metadata_tag("wing-edge"));
   context.cancel_metadata_track(track);

Tracks and policy scopes are owned by an explicit ``geoml::ModelingContext``
rather than by ``Shape`` or thread-local state. Predicates may freely capture
``Shape`` handles without introducing reference cycles; all tracks and their
captured references are released when the ``ModelingContext`` is destroyed.
Shapes never own their ``ModelingContext``.

``add_metadata_track()``, ``add_tag_track()``, and
``add_metadata_track_from()`` apply immediately at registration without
consuming a step. Modeling operations with history mapping—such as
``boolean_union()``, ``boolean_subtract()``, and ``make_fillet()``—accept a
``ModelingContext`` as an optional last argument and evaluate active tracks on
the result. Operations called without a context remain usable and do not
propagate tracks. A raw ``TopoDS_Shape`` carries no tracking context.

Tracks are associated with the shared per-entity metadata identity of the
target shape: a track registered on an edge selected through one face
propagates when modeling with the same edge reached through another face of the
same model, while independent ``Shape`` models remain isolated.

Tracks are unlimited by default. ``geoml::TrackLifetime::steps(2)`` allows the
next two modeling results on each branch, even if neither matches. Passing the
returned ``MetadataTrackHandle`` to ``context.cancel_metadata_track(handle)``
removes the rule from all branches within that context while leaving already
written metadata values intact.

The direct tag helpers (``add_meta_tag()``, ``add_meta_tag_to_subshapes()``,
``has_tag()``, and ``has_metadata_tag()``) read and write the typed ``tags``
key. The legacy ``TagTrack`` class and shape-based tracking methods
(``Shape::add_tag_track()``, ``Shape::apply_tag_tracks()``,
``Shape::get_tag_tracks()``) were removed in favor of ``ModelingContext``.

Resolving competing contributions
---------------------------------

Create a scoped policy on the ``ModelingContext`` to control how competing
contributions are resolved during modeling. This policy keeps the wing's tags
when both inputs contribute to the same edge. An edge with contributions only
from the fuselage keeps those tags.

.. code-block:: cpp

   context.add_tag_track(fuselage, geoml::is_edge, "fuselage-edge");

   auto scope = context.create_policy_scope(
       meta::MetadataPolicy{}
           .for_key(tags, meta::prefer_input(0)));

   auto tagged_aircraft = geoml::boolean_union(wing, fuselage, context);

``create_policy_scope()`` returns a ``[[nodiscard]]`` RAII
``geoml::metadata::MetadataScope`` guard. Nested scopes override only the keys
they specify and inherit all other keys from enclosing scopes on the same
context. Destruction of the guard restores the previous policy state, including
during exception unwinding. A ``ModelingContext`` is not thread-safe for
concurrent access.

A modeling operation selects one resolver per key, even for a single
contribution. It checks the following in order:

#. The innermost active policy scope on the ``ModelingContext`` that defines
   a resolver for the key.
#. Enclosing policy scopes on that ``ModelingContext``.
#. The registered default resolver for the key.
#. The generic resolver, which accepts one value or equal values.

Names and colors require equality by default. The default tags resolver
keeps the first occurrence of each tag. To choose a default for a custom
key, call ``register_default_resolver(key, resolver)`` once at setup. Keys
with the same name and type are interchangeable.

Policies affect only the operations executed while the scope is active and do
not modify or cancel tracks. Later operations can apply the same tracks under a
different policy scope. See the :ref:`metadata-resolvers-api` for
``prefer_input()``, custom callbacks, and conflict diagnostics.
