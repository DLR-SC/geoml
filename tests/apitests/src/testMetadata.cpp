#include <geoml/metadata/Metadata.h>
#include <geoml/modeling/ModelingContext.h>
#include <geoml/naming_choosing/Shape.h>
#include <geoml/boolean_ops/modeling.hpp>
#include <geoml/predicates/predicate_functions.h>
#include <geoml/utilities/utilities.h>
#include <topology/Operation.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <any>
#include <atomic>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <stdexcept>
#include <string>
#include <memory>
#include <thread>
#include <set>
#include <typeindex>
#include <utility>
#include <vector>

namespace {

const geoml::metadata::Key<int> integer_key{"test.integer"};
const geoml::metadata::Key<int> replacing_integer_key{"test.replacing-integer"};
const geoml::metadata::Key<std::string> conflicting_integer_key{"test.integer"};
const geoml::metadata::Key<std::string> unrelated_key{"test.unrelated"};
const geoml::metadata::Key<std::string> detail_key{"test.detail"};
const auto pre_main_name_key = geoml::metadata::keys::name();

struct OpaqueValue
{
    int value;
};

struct ResolverCopyState
{
    bool enabled = false;
    int copies = 0;
};

struct CopyCountingAddResolver
{
    explicit CopyCountingAddResolver(std::shared_ptr<ResolverCopyState> state)
        : state(std::move(state))
    {}

    CopyCountingAddResolver(CopyCountingAddResolver const& other)
        : state(other.state)
    {
        if (state->enabled && ++state->copies == 2) {
            throw std::runtime_error("second resolver copy");
        }
    }

    int operator()(geoml::metadata::ResolutionContext<int> const& context) const
    {
        auto result = context.existing_value.value_or(0);
        for (auto const& contribution : context.contributions) {
            result += contribution.value;
        }
        return result;
    }

    std::shared_ptr<ResolverCopyState> state;
};

struct WrongMetadataReturnType
{
    std::string operator()(geoml::metadata::ResolutionContext<int> const&) const { return {}; }
};

static_assert(!geoml::metadata::details::IsMetadataResolver<int, WrongMetadataReturnType>::value,
              "resolver return types must exactly match the metadata value type");

const geoml::metadata::Key<OpaqueValue> opaque_value_key{"test.opaque"};

class MetadataCopyOperation : public geoml::Operation<MetadataCopyOperation>
{
public:
    MetadataCopyOperation(std::vector<geoml::Shape> const& inputs, bool add_existing = false)
        : geoml::Operation<MetadataCopyOperation>(inputs)
        , m_add_existing(add_existing)
    {}

    geoml::Shape perform()
    {
        geoml::Shape result{m_inputs.front().shape()};
        if (m_add_existing) {
            result.metadata().set(unrelated_key, std::string("preserve-me"));
        }
        return result;
    }

    void map_subshapes(geoml::Shape&) {}

private:
    bool m_add_existing;
};

} // namespace

TEST(Metadata, merge_does_not_copy_the_default_resolver_before_invoking_it)
{
    using namespace geoml::metadata;
    auto state = std::make_shared<ResolverCopyState>();
    Key<int> key{"test.copy-counting-add"};
    register_default_resolver(key, CopyCountingAddResolver{state});
    Metadata destination;
    Metadata source;
    destination.set(key, 2);
    source.set(key, 3);

    state->copies = 0;
    state->enabled = true;
    EXPECT_NO_THROW(destination.merge_from(source));
    state->enabled = false;
    EXPECT_EQ(state->copies, 0);

    EXPECT_EQ(destination.at(key), 5);
}

TEST(MetadataResolution, conflict_from_an_override_carries_the_complete_operation_context)
{
    using namespace geoml;
    using namespace geoml::metadata;
    const Key<int> key{"test.override-conflict"};
    ModelingContext modeling_context;
    Shape left{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 2., 3.}).Shape()};
    Shape right{BRepBuilderAPI_MakeVertex(gp_Pnt{4., 5., 6.}).Shape()};
    modeling_context.add_metadata_track(left, is_vertex, key, 2);
    modeling_context.add_metadata_track(right, is_vertex, key, 3);
    MetadataPolicy policy;
    policy.for_key(key, [](ResolutionContext<int> const& context) -> int {
        throw MetadataConflictException(context.key_name, typeid(int), 2, 3);
    });
    auto scope = modeling_context.create_policy_scope(policy);

    try {
        MetadataCopyOperation({left, right}).value(modeling_context);
        FAIL() << "Expected a metadata conflict from the override";
    }
    catch (MetadataConflictException const& conflict) {
        ASSERT_TRUE(conflict.target_topology());
        EXPECT_TRUE(conflict.target_topology()->IsSame(left.shape()));
        ASSERT_EQ(conflict.contributions().size(), 2u);
        for (std::size_t i = 0; i < 2; ++i) {
            auto const& contribution = conflict.contributions()[i];
            EXPECT_EQ(contribution.input_position, i);
            EXPECT_TRUE(contribution.source_track_id);
            ASSERT_TRUE(contribution.source_topology);
            EXPECT_TRUE(contribution.source_topology->IsSame(i == 0 ? left.shape() : right.shape()));
        }
    }
}

TEST(Metadata, typed_operations_and_absence)
{
    geoml::Shape shape;
    auto& metadata = shape.metadata();

    EXPECT_FALSE(metadata.contains(geoml::metadata::keys::name()));
    EXPECT_FALSE(metadata.find(geoml::metadata::keys::name()).has_value());
    EXPECT_THROW(metadata.at(geoml::metadata::keys::name()), std::out_of_range);

    metadata.set(geoml::metadata::keys::name(), std::string("wing"));
    EXPECT_TRUE(metadata.contains(geoml::metadata::keys::name()));
    EXPECT_EQ(metadata.at(geoml::metadata::keys::name()), "wing");
    EXPECT_EQ(metadata.find(geoml::metadata::keys::name()), std::string("wing"));

    metadata.set(geoml::metadata::keys::name(), std::string("replacement"));
    EXPECT_EQ(metadata.at(geoml::metadata::keys::name()), "replacement");

    EXPECT_TRUE(metadata.erase(geoml::metadata::keys::name()));
    EXPECT_FALSE(metadata.erase(geoml::metadata::keys::name()));
    EXPECT_FALSE(metadata.contains(geoml::metadata::keys::name()));
}

TEST(Metadata, namespace_scope_builtin_key_copy_is_valid)
{
    geoml::metadata::Metadata metadata;
    metadata.set(pre_main_name_key, std::string("initialized before main"));

    EXPECT_EQ(metadata.at(geoml::metadata::keys::name()), "initialized before main");
}

TEST(Metadata, built_in_color_and_tags)
{
    geoml::metadata::Metadata metadata;
    auto const color = geoml::Color(0x12, 0x34, 0x56);
    metadata.set(geoml::metadata::keys::color(), color);
    metadata.set(geoml::metadata::keys::tags(),
                 std::vector<std::string>{"structural", "visible", "structural"});

    EXPECT_EQ(metadata.at(geoml::metadata::keys::color()), color);
    EXPECT_EQ(metadata.at(geoml::metadata::keys::tags()),
              (std::vector<std::string>{"structural", "visible", "structural"}));

    geoml::metadata::Metadata incoming;
    incoming.set(geoml::metadata::keys::tags(), {"visible", "outer", "outer"});
    metadata.merge_from(incoming);
    EXPECT_EQ(metadata.at(geoml::metadata::keys::tags()),
              (std::vector<std::string>{"structural", "visible", "outer"}));

    geoml::metadata::Metadata empty;
    empty.merge_from(incoming);
    EXPECT_EQ(empty.at(geoml::metadata::keys::tags()),
              (std::vector<std::string>{"visible", "outer"}));
}

TEST(Metadata, keys_of_the_same_value_type_use_their_own_strategy)
{
    using namespace geoml::metadata;
    using Groups = std::vector<std::string>;
    const Key<Groups> unique_groups{"unique-groups"};
    const Key<Groups> ordered_groups{"ordered-groups"};
    register_default_resolver(unique_groups, SetUnionResolver{});
    register_default_resolver(ordered_groups, ReplaceResolver{});

    Metadata destination;
    Metadata source;
    for (Key<Groups> const& key : {unique_groups, ordered_groups}) {
        destination.set(key, Groups{"wing", "wing"});
        source.set(key, Groups{"tail"});
    }
    EXPECT_EQ(destination.at(unique_groups), (Groups{"wing", "wing"}));
    EXPECT_EQ(destination.at(ordered_groups), (Groups{"wing", "wing"}));

    destination.merge_from(source);
    EXPECT_EQ(destination.at(unique_groups), (Groups{"wing", "tail"}));
    EXPECT_EQ(destination.at(ordered_groups), (Groups{"tail"}));
}

TEST(Metadata, custom_set_key_accepts_braced_values_and_merges_groups)
{
    using namespace geoml::metadata;
    Key<std::set<std::string>> groups{"simulation.groups"};
    register_default_resolver(groups, SetUnionResolver{});
    Metadata wing, fuselage;
    wing.set(groups, {"aerodynamics", "wing"});
    fuselage.set(groups, {"aerodynamics", "body"});
    wing.merge_from(fuselage);
    EXPECT_EQ(wing.at(groups), (std::set<std::string>{"aerodynamics", "body", "wing"}));
}

TEST(Metadata, registered_default_resolver_can_capture_and_set_still_replaces)
{
    auto const bias = 4;
    const geoml::metadata::Key<int> accumulated_key{"test.accumulated"};
    geoml::metadata::register_default_resolver(accumulated_key,
        [bias](geoml::metadata::ResolutionContext<int> const& context) -> int {
            int result = context.existing_value.value_or(0);
            for (auto const& contribution : context.contributions) {
                result += contribution.value;
            }
            return result + bias;
        });

    geoml::metadata::Metadata destination;
    destination.set(accumulated_key, 2);
    geoml::metadata::Metadata source;
    source.set(accumulated_key, 3);
    destination.merge_from(source);
    EXPECT_EQ(destination.at(accumulated_key), 9);

    destination.set(accumulated_key, 11);
    EXPECT_EQ(destination.at(accumulated_key), 11);
}

TEST(MetadataResolution, default_and_override_resolvers_handle_a_single_contribution)
{
    using namespace geoml;
    using namespace geoml::metadata;
    auto calls = std::make_shared<int>(0);
    const Key<int> key{"test.singleton-biased"};
    register_default_resolver(key, [calls](ResolutionContext<int> const& context) {
        ++*calls;
        return context.contributions.front().value + 4;
    });

    ModelingContext modeling_context;
    Shape input{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    modeling_context.add_metadata_track(input, is_vertex, key, 3);
    EXPECT_EQ(input.metadata().at(key), 7);
    EXPECT_EQ(*calls, 1);

    MetadataCopyOperation operation({input});
    EXPECT_EQ(operation.value(modeling_context).metadata().at(key), 7);
    EXPECT_EQ(*calls, 2);

    MetadataPolicy policy;
    policy.for_key(Key<int>{key.name()}, [](ResolutionContext<int> const& context) {
        return context.contributions.front().value * 2;
    });
    auto scope = modeling_context.create_policy_scope(policy);
    EXPECT_EQ(operation.value(modeling_context).metadata().at(key), 6);
    EXPECT_EQ(*calls, 2);
    EXPECT_EQ(input.metadata().at(key), 7);
}

TEST(MetadataTrack, tracks_resolve_by_name_after_key_handles_expire)
{
    auto calls = std::make_shared<int>(0);
    geoml::ModelingContext modeling_context;
    geoml::Shape first{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape second{BRepPrimAPI_MakeBox(2., 1., 1.).Shape()};

    auto register_tracks = [&](geoml::Shape& a, geoml::Shape& b) {
        geoml::metadata::Key<int> local_key{"test.expired-key-handle"};
        geoml::metadata::register_default_resolver(local_key,
            [calls, bias = 4](geoml::metadata::ResolutionContext<int> const& context) -> int {
                ++*calls;
                int value = context.existing_value.value_or(0);
                for (auto const& contribution : context.contributions) {
                    value += contribution.value;
                }
                return value + bias;
            });
        geoml::metadata::Key<int> another_key{local_key.name()};
        modeling_context.add_metadata_track(
            a, [](geoml::Shape const&) { return true; }, local_key, 2,
            geoml::TrackLifetime::steps(2));
        modeling_context.add_metadata_track(
            b, [](geoml::Shape const&) { return true; }, another_key, 3,
            geoml::TrackLifetime::steps(2));
    };
    register_tracks(first, second);

    MetadataCopyOperation operation({first, second});
    auto result = operation.value(modeling_context);
    auto const& entries = result.metadata().entries();
    auto resolved = entries.find("test.expired-key-handle");
    ASSERT_NE(resolved, entries.end());
    auto const value = std::any_cast<int>(&resolved->second.value);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 9);
    EXPECT_GT(*calls, 0);
}

TEST(Metadata, throwing_default_resolver_rolls_back_store_merge)
{
    const geoml::metadata::Key<int> throwing_key{"test.throwing-default"};
    geoml::metadata::register_default_resolver(throwing_key,
        [](geoml::metadata::ResolutionContext<int> const&) -> int {
            throw std::runtime_error("default resolver failed");
        });
    geoml::metadata::Metadata destination;
    destination.set(throwing_key, 10);
    geoml::metadata::Metadata source;
    source.set(throwing_key, 20);

    EXPECT_THROW(destination.merge_from(source), std::runtime_error);
    EXPECT_EQ(destination.at(throwing_key), 10);
    EXPECT_EQ(source.at(throwing_key), 20);
}

TEST(Metadata, independent_keys_with_the_same_name_and_type_are_interchangeable)
{
    using namespace geoml::metadata;
    const Key<int> first{"test.same-name"};
    const Key<int> second{"test.same-name"};
    Metadata destination;
    Metadata source;
    destination.set(first, 2);
    destination.set(second, 3);
    EXPECT_EQ(destination.find(first), 3);
    EXPECT_TRUE(destination.contains(second));
    source.set(first, 3);
    EXPECT_NO_THROW(destination.merge_from(source));
    EXPECT_EQ(destination.at(second), 3);
    EXPECT_TRUE(destination.erase(second));
    EXPECT_FALSE(destination.contains(first));
}

TEST(Metadata, resolver_registration_rejects_duplicate_names)
{
    using namespace geoml::metadata;
    const Key<int> key{"test.duplicate-default"};
    register_default_resolver(key, ReplaceResolver{});
    EXPECT_THROW(register_default_resolver(Key<int>{key.name()}, RequireEqualResolver{}),
                 std::logic_error);
    EXPECT_THROW(register_default_resolver(Key<std::string>{key.name()}, ReplaceResolver{}),
                 std::logic_error);
    EXPECT_THROW(register_default_resolver(Key<std::string>{"name"}, ReplaceResolver{}),
                 std::logic_error);

    Metadata destination, source;
    destination.set(key, 1);
    source.set(Key<int>{key.name()}, 2);
    destination.merge_from(source);
    EXPECT_EQ(destination.at(key), 2);

    Metadata incompatible;
    incompatible.set(Key<std::string>{key.name()}, "wrong type");
    Metadata empty;
    EXPECT_THROW(empty.merge_from(incompatible), std::logic_error);
    EXPECT_TRUE(empty.empty());
}

TEST(Metadata, concurrent_registration_keeps_one_resolver_and_allows_reentrant_registration)
{
    using namespace geoml::metadata;
    const Key<int> key{"test.concurrent-default"};
    std::atomic<int> registrations{0};
    auto register_key = [&] {
        try {
            register_default_resolver(key, [](ResolutionContext<int> const& context) {
                register_default_resolver(Key<int>{"test.reentrant-default"}, ReplaceResolver{});
                return context.contributions.front().value + 1;
            });
            ++registrations;
        }
        catch (std::logic_error const&) {
        }
    };
    std::thread first(register_key);
    std::thread second(register_key);
    first.join();
    second.join();
    EXPECT_EQ(registrations.load(), 1);

    Metadata source, destination;
    source.set(key, 2);
    destination.merge_from(source);
    EXPECT_EQ(destination.at(key), 3);
}

TEST(Metadata, empty_source_does_not_invoke_resolvers_or_change_values)
{
    using namespace geoml::metadata;
    const Key<int> key{"test.empty-resolution"};
    register_default_resolver(key, [](ResolutionContext<int> const&) -> int {
        throw std::runtime_error("unexpected resolver call");
    });
    Metadata metadata;
    metadata.set(key, 12);
    EXPECT_NO_THROW(metadata.merge_from(Metadata{}));
    EXPECT_EQ(metadata.at(key), 12);
}

TEST(Metadata, merges_using_key_resolvers)
{
    geoml::metadata::register_default_resolver(replacing_integer_key, geoml::metadata::ReplaceResolver{});
    geoml::metadata::Metadata destination;
    destination.set(geoml::metadata::keys::name(), std::string("existing"));
    destination.set(geoml::metadata::keys::tags(), std::vector<std::string>{"shared", "left"});
    destination.set(replacing_integer_key, 7);

    geoml::metadata::Metadata source;
    source.set(geoml::metadata::keys::tags(), std::vector<std::string>{"left", "right"});
    source.set(replacing_integer_key, 9);

    destination.merge_from(source);

    EXPECT_EQ(destination.at(geoml::metadata::keys::name()), "existing");
    EXPECT_EQ(destination.at(geoml::metadata::keys::tags()),
              (std::vector<std::string>{"shared", "left", "right"}));
    EXPECT_EQ(destination.at(replacing_integer_key), 9);
}

TEST(Metadata, explicit_resolver_supports_values_without_equality)
{
    geoml::metadata::register_default_resolver(opaque_value_key, geoml::metadata::ReplaceResolver{});
    geoml::metadata::Metadata destination;
    destination.set(opaque_value_key, OpaqueValue{1});

    geoml::metadata::Metadata source;
    source.set(opaque_value_key, OpaqueValue{2});

    EXPECT_NO_THROW(destination.merge_from(source));
    EXPECT_EQ(destination.at(opaque_value_key).value, 2);
}

TEST(Metadata, unregistered_key_without_equality_accepts_one_value_and_reports_a_conflict)
{
    using namespace geoml::metadata;
    const Key<std::vector<OpaqueValue>> key{"test.opaque-list"};
    Metadata destination, source;
    source.set(key, {{1}});
    destination.merge_from(source);
    ASSERT_EQ(destination.at(key).size(), 1u);
    EXPECT_EQ(destination.at(key).front().value, 1);

    source.set(key, {{2}});
    EXPECT_THROW(destination.merge_from(source), MetadataConflictException);
    EXPECT_EQ(destination.at(key).front().value, 1);
}

TEST(Metadata, default_scalar_resolver_requires_equal_values)
{
    geoml::metadata::Metadata destination;
    destination.set(geoml::metadata::keys::name(), std::string("same-name"));
    destination.set(geoml::metadata::keys::color(), geoml::Color(10, 20, 30));
    destination.set(integer_key, 7);

    geoml::metadata::Metadata equal_values;
    equal_values.set(geoml::metadata::keys::name(), std::string("same-name"));
    equal_values.set(geoml::metadata::keys::color(), geoml::Color(10, 20, 30));
    equal_values.set(integer_key, 7);
    EXPECT_NO_THROW(destination.merge_from(equal_values));

    geoml::metadata::Metadata conflicting_values;
    conflicting_values.set(geoml::metadata::keys::name(), std::string("other-name"));
    conflicting_values.set(geoml::metadata::keys::color(), geoml::Color(10, 20, 31));
    conflicting_values.set(integer_key, 8);

    try {
        destination.merge_from(conflicting_values);
        FAIL() << "Expected a structured metadata conflict";
    }
    catch (geoml::metadata::MetadataConflictException const& error) {
        EXPECT_EQ(error.key_name(), "color");
        EXPECT_EQ(error.value_type(), std::type_index(typeid(geoml::Color)));
        ASSERT_TRUE(std::any_cast<geoml::Color>(&error.existing_value()));
        EXPECT_EQ(*std::any_cast<geoml::Color>(&error.existing_value()), geoml::Color(10, 20, 30));
        EXPECT_EQ(*std::any_cast<geoml::Color>(&error.incoming_value()), geoml::Color(10, 20, 31));
        EXPECT_NE(std::string(error.what()).find("color"), std::string::npos);
    }

    EXPECT_EQ(destination.at(geoml::metadata::keys::name()), "same-name");
    EXPECT_EQ(destination.at(geoml::metadata::keys::color()), geoml::Color(10, 20, 30));
    EXPECT_EQ(destination.at(integer_key), 7);

    geoml::metadata::Metadata different_integer;
    different_integer.set(integer_key, 8);
    try {
        destination.merge_from(different_integer);
        FAIL() << "Expected a structured integer conflict";
    }
    catch (geoml::metadata::MetadataConflictException const& error) {
        EXPECT_EQ(error.key_name(), integer_key.name());
        EXPECT_EQ(error.value_type(), std::type_index(typeid(int)));
        EXPECT_EQ(*std::any_cast<int>(&error.existing_value()), 7);
        EXPECT_EQ(*std::any_cast<int>(&error.incoming_value()), 8);
    }
    EXPECT_EQ(destination.at(integer_key), 7);
}

TEST(Metadata, conflict_exception_owns_values_after_merge_inputs_expire)
{
    auto make_error = []() -> geoml::metadata::MetadataConflictException {
        geoml::metadata::Metadata destination;
        destination.set(geoml::metadata::keys::name(), std::string("left-name"));

        geoml::metadata::Metadata source;
        source.set(geoml::metadata::keys::name(), std::string("right-name"));

        try {
            destination.merge_from(source);
        }
        catch (geoml::metadata::MetadataConflictException const& error) {
            return error;
        }
        throw std::logic_error("Expected a metadata conflict");
    };

    auto const error = make_error();
    EXPECT_EQ(error.key_name(), "name");
    EXPECT_EQ(error.value_type(), std::type_index(typeid(std::string)));
    ASSERT_TRUE(std::any_cast<std::string>(&error.existing_value()));
    EXPECT_EQ(*std::any_cast<std::string>(&error.existing_value()), "left-name");
    EXPECT_EQ(*std::any_cast<std::string>(&error.incoming_value()), "right-name");
}

TEST(Metadata, merge_from_rolls_back_earlier_keys_when_a_resolver_throws)
{
    using namespace geoml::metadata;
    const Key<int> earlier_key{"a.earlier"};
    const Key<int> throwing_merge_key{"z.throwing"};
    register_default_resolver(earlier_key, ReplaceResolver{});
    register_default_resolver(throwing_merge_key, [](ResolutionContext<int> const&) -> int {
        throw std::runtime_error("merge failed");
    });

    geoml::metadata::Metadata destination;
    destination.set(earlier_key, 1);
    destination.set(throwing_merge_key, 10);
    destination.set(unrelated_key, std::string("keep"));

    geoml::metadata::Metadata source;
    source.set(earlier_key, 2);
    source.set(throwing_merge_key, 20);

    EXPECT_THROW(destination.merge_from(source), std::runtime_error);
    EXPECT_EQ(destination.at(earlier_key), 1);
    EXPECT_EQ(destination.at(throwing_merge_key), 10);
    EXPECT_EQ(destination.at(unrelated_key), "keep");
    EXPECT_EQ(source.at(earlier_key), 2);
    EXPECT_EQ(source.at(throwing_merge_key), 20);
}

TEST(Metadata, enumerates_in_stable_key_order)
{
    geoml::metadata::Metadata metadata;
    metadata.set(geoml::metadata::keys::tags(), std::vector<std::string>{"x"});
    metadata.set(geoml::metadata::keys::name(), std::string("body"));
    metadata.set(geoml::metadata::keys::color(), geoml::Color(1, 2, 3));

    std::vector<std::string> names;
    std::vector<std::type_index> types;
    for (auto const& [name, entry] : metadata.entries()) {
        names.push_back(name);
        types.emplace_back(entry.value.type());
    }
    EXPECT_EQ(names, (std::vector<std::string>{"color", "name", "tags"}));
    EXPECT_EQ(types, (std::vector<std::type_index>{
        typeid(geoml::Color), typeid(std::string), typeid(std::vector<std::string>)}));
}

TEST(Metadata, rejects_incompatible_key_types)
{
    geoml::metadata::Metadata metadata;
    metadata.set(integer_key, 4);
    metadata.set(geoml::metadata::keys::tags(), std::vector<std::string>{"typed"});

    EXPECT_THROW(metadata.contains(conflicting_integer_key), std::logic_error);
    EXPECT_THROW(metadata.set(conflicting_integer_key, "wrong type"), std::logic_error);
    EXPECT_THROW(metadata.erase(conflicting_integer_key), std::logic_error);
    EXPECT_THROW(metadata.merge_from([] {
        geoml::metadata::Metadata other;
        other.set(conflicting_integer_key, std::string("wrong schema"));
        return other;
    }()), std::logic_error);

    const geoml::metadata::Key<int> integer_policy_key{"test.policy-schema"};
    const geoml::metadata::Key<std::string> string_policy_key{"test.policy-schema"};
    geoml::metadata::MetadataPolicy policy;
    policy.for_key(integer_policy_key,
                   [](geoml::metadata::ResolutionContext<int> const&) { return 1; });
    EXPECT_THROW(policy.for_key(string_policy_key,
                                [](geoml::metadata::ResolutionContext<std::string> const&) {
                                    return std::string("wrong schema");
                                }),
                 std::logic_error);
}

TEST(ShapeMetadata, tag_predicate_reads_the_typed_value)
{
    geoml::Shape shape;
    auto copy = shape;
    auto const& tags = geoml::metadata::keys::tags();
    copy.metadata().set(tags, {"typed-tag"});
    EXPECT_TRUE(geoml::has_metadata_tag("typed-tag")(shape));
    EXPECT_FALSE(geoml::has_metadata_tag("absent")(shape));
    copy.metadata().erase(tags);
    EXPECT_FALSE(geoml::has_metadata_tag("typed-tag")(shape));
}

TEST(ShapeMetadata, tag_helpers_share_the_typed_store_and_tracks)
{
    using namespace geoml;
    ModelingContext context;
    Shape shape{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    auto const& tags = metadata::keys::tags();

    shape.add_meta_tag("direct");
    EXPECT_EQ(shape.metadata().at(tags), (std::vector<std::string>{"direct"}));
    context.add_tag_track(shape, is_vertex, "convenience", TrackLifetime::steps(2));
    EXPECT_TRUE(shape.has_tag("convenience"));
    EXPECT_TRUE(has_tag("convenience")(shape));

    context.add_metadata_track(shape, is_vertex, tags, {"typed"}, TrackLifetime::steps(1));
    EXPECT_TRUE(shape.has_tag("typed"));

    auto first = MetadataCopyOperation({shape}).value(context);
    EXPECT_EQ(first.metadata().at(tags), (std::vector<std::string>{"convenience", "typed"}));
    auto second = MetadataCopyOperation({first}).value(context);
    EXPECT_EQ(second.metadata().at(tags), (std::vector<std::string>{"convenience"}));
    EXPECT_FALSE(MetadataCopyOperation({second}).value(context).metadata().contains(tags));
}

TEST(MetadataTrack, returning_an_input_or_its_selected_edge_isolates_each_operation)
{
    using namespace geoml;
    class ReturnHandle : public Operation<ReturnHandle>
    {
    public:
        ReturnHandle(Shape input, Shape output) : Operation({input}), output(output) {}
        Shape perform() { return output; }
        void map_subshapes(Shape& result) { add_origin(result, m_inputs.front()); }
        Shape output;
    };

    ModelingContext context;
    Shape model{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto edge = model.select_subshapes(is_edge)[0];
    auto const& tags = metadata::keys::tags();
    context.add_metadata_track(model, is_same(edge), tags, {"tracked"}, TrackLifetime::steps(1));
    edge.metadata().erase(tags);

    auto result = ReturnHandle(model, model).value(context);
    EXPECT_FALSE(model.has_origin());
    EXPECT_TRUE(result.has_origin());
    EXPECT_TRUE(has_metadata_tag("tracked")(result.select_subshapes(is_same(edge)).unique_element()));
    EXPECT_FALSE(has_metadata_tag("tracked")(edge));

    auto second = ReturnHandle(model, edge).value(context);
    EXPECT_TRUE(has_metadata_tag("tracked")(second));
    EXPECT_FALSE(edge.has_origin());
    EXPECT_FALSE(has_metadata_tag("tracked")(edge));
    second.metadata().erase(tags);
    EXPECT_FALSE(has_metadata_tag("tracked")(ReturnHandle(second, second).value(context)));

    // A selection has a fresh compound root but borrows its children.
    auto selection = model.select_subshapes(is_same(edge));
    auto selected = ReturnHandle(model, selection).value(context);
    EXPECT_TRUE(has_metadata_tag("tracked")(selected.unique_element()));
    EXPECT_FALSE(has_metadata_tag("tracked")(edge));
}

TEST(MetadataTrack, tag_tracks_merge_and_expire_independently)
{
    using namespace geoml;
    ModelingContext context;
    Shape input{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    auto const& tags = metadata::keys::tags();
    context.add_metadata_track(input, is_vertex, tags, {"two-steps"}, TrackLifetime::steps(2));
    context.add_metadata_track(input, is_vertex, tags, {"one-step", "one-step"},
                               TrackLifetime::steps(1));

    auto first = MetadataCopyOperation({input}).value(context);
    EXPECT_EQ(first.metadata().at(tags), (std::vector<std::string>{"two-steps", "one-step"}));
    auto second = MetadataCopyOperation({first}).value(context);
    EXPECT_EQ(second.metadata().at(tags), (std::vector<std::string>{"two-steps"}));
    EXPECT_FALSE(MetadataCopyOperation({second}).value(context).metadata().contains(tags));
    EXPECT_EQ(MetadataCopyOperation({input}).value(context).metadata().at(tags),
              first.metadata().at(tags));
}

TEST(ShapeMetadata, metadata_survives_reselection_and_is_model_scoped)
{
    using geoml::Shape;

    Shape model{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto faces = model.select_subshapes(
        [](Shape const& candidate) { return candidate.is_type(TopAbs_FACE); });
    ASSERT_GT(faces.size(), 0u);

    auto original_face = faces[0];
    auto const original_topology = original_face.shape();
    original_face.metadata().set(geoml::metadata::keys::name(), std::string("front"));

    auto reselected_face = model.select_subshapes(
        [&](Shape const& candidate) { return candidate.is_same(original_topology); }
    ).unique_element();
    EXPECT_EQ(reselected_face.metadata().at(geoml::metadata::keys::name()), "front");

    Shape independent_model{model.shape()};
    auto independent_face = independent_model.select_subshapes(
        [&](Shape const& candidate) { return candidate.is_same(original_topology); }
    ).unique_element();
    EXPECT_FALSE(independent_face.metadata().contains(geoml::metadata::keys::name()));
}

TEST(ShapeMetadata, shared_edges_have_shared_metadata_through_face_paths)
{
    using geoml::Shape;

    Shape model{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto faces = model.select_subshapes(
        [](Shape const& candidate) { return candidate.is_type(TopAbs_FACE); });
    ASSERT_GT(faces.size(), 1u);

    bool found_shared_edge = false;
    for (std::size_t first_face = 0; first_face < faces.size() && !found_shared_edge; ++first_face) {
        auto first_edges = faces[first_face].select_subshapes(
            [](Shape const& candidate) { return candidate.is_type(TopAbs_EDGE); });
        for (std::size_t second_face = first_face + 1;
             second_face < faces.size() && !found_shared_edge;
             ++second_face) {
            for (auto& first_edge : first_edges) {
                auto const topology = first_edge.shape();
                auto matching_edges = faces[second_face].select_subshapes(
                    [&](Shape const& candidate) { return candidate.is_same(topology); });
                if (matching_edges.size() == 1) {
                    first_edge.metadata().set(geoml::metadata::keys::name(),
                                              std::string("shared-edge"));
                    EXPECT_EQ(matching_edges[0].metadata().at(geoml::metadata::keys::name()),
                              "shared-edge");
                    found_shared_edge = true;
                    break;
                }
            }
        }
    }

    EXPECT_TRUE(found_shared_edge);
}

TEST(ShapeMetadata, topology_identity_includes_location)
{
    using geoml::Shape;

    BRepBuilderAPI_MakeVertex make_vertex(gp_Pnt(0., 0., 0.));
    auto vertex = make_vertex.Shape();

    gp_Trsf translation;
    translation.SetTranslation(gp_Vec(5., 0., 0.));
    auto moved_vertex = vertex.Moved(TopLoc_Location(translation));

    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, vertex);
    builder.Add(compound, moved_vertex);

    Shape model{compound};
    ASSERT_EQ(model.size(), 2u);
    ASSERT_FALSE(model[0].is_same(model[1]));

    model[0].metadata().set(geoml::metadata::keys::name(), std::string("origin"));
    EXPECT_FALSE(model[1].metadata().contains(geoml::metadata::keys::name()));
}

TEST(MetadataTrack, applies_immediately_and_keeps_its_contribution)
{
    geoml::ModelingContext context;
    geoml::Shape source{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    context.add_metadata_track(
        source, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "tracked",
        geoml::TrackLifetime::steps(2));

    EXPECT_EQ(source.metadata().at(geoml::metadata::keys::name()), "tracked");
    source.metadata().set(geoml::metadata::keys::name(), std::string("manual edit"));

    MetadataCopyOperation operation({source}, true);
    auto result = operation.value(context);

    EXPECT_EQ(result.metadata().at(geoml::metadata::keys::name()), "tracked");
    EXPECT_EQ(result.metadata().at(unrelated_key), "preserve-me");
}

TEST(MetadataTrack, wing_edge_can_be_found_after_modeling_without_a_step_limit)
{
    using namespace geoml;
    namespace keys = geoml::metadata::keys;

    ModelingContext context;
    Shape wing{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    Shape fuselage{BRepPrimAPI_MakeBox(gp_Pnt(2., 0., 0.), 1., 1., 1.).Shape()};
    Shape cutting_tool{BRepPrimAPI_MakeBox(gp_Pnt(2.5, 0., 0.), 1., 1., 1.).Shape()};
    Shape tail{BRepPrimAPI_MakeBox(gp_Pnt(4., 0., 0.), 1., 1., 1.).Shape()};
    auto edge = wing.select_subshapes(is_edge)[0];
    auto tracked_edges = is_edge && is_descendent_of(edge);
    context.add_tag_track(wing, tracked_edges, "wing-edge");

    EXPECT_TRUE(has_metadata_tag("wing-edge")(edge));
    auto aircraft = boolean_union(wing, fuselage, context);
    auto trimmed_aircraft = boolean_subtract(aircraft, cutting_tool, context);
    auto finished = boolean_union(trimmed_aircraft, tail, context);

    auto wing_edges = finished.select_subshapes(is_edge && has_metadata_tag("wing-edge"));
    ASSERT_EQ(wing_edges.size(), 1u);
    EXPECT_TRUE(wing_edges.unique_element().is_descendent_of(edge));
}

TEST(MetadataTrack, tracks_a_snapshot_of_all_source_metadata)
{
    using namespace geoml;
    namespace meta = geoml::metadata;

    ModelingContext modeling_context;
    Shape wing{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto edges = wing.select_subshapes(is_edge);
    auto edge = edges[0];
    auto other_edge = edges[1];
    meta::Key<int> weight{"test.weight"};
    meta::register_default_resolver(weight, [](meta::ResolutionContext<int> const& context) {
        return context.existing_value.value_or(0) + context.contributions.front().value;
    });
    edge.metadata().set(meta::keys::name(), std::string{"wing-edge"});
    edge.metadata().set(meta::keys::color(), Color{255, 0, 0});
    edge.metadata().set(weight, 7);

    auto targets = is_same(edge) || is_same(other_edge);
    modeling_context.add_metadata_track_from(wing, edge, targets, TrackLifetime::steps(1));
    EXPECT_EQ(edge.metadata().at(weight), 7);
    EXPECT_EQ(other_edge.metadata().at(meta::keys::name()), "wing-edge");
    EXPECT_EQ(other_edge.metadata().at(weight), 7);
    edge.metadata().set(meta::keys::color(), Color{0, 0, 255});
    edge.metadata().set(weight, 9);

    auto result = MetadataCopyOperation({wing}).value(modeling_context);
    auto result_edge = result.select_subshapes(is_same(edge)).unique_element();
    EXPECT_EQ(result_edge.metadata().at(meta::keys::name()), "wing-edge");
    EXPECT_EQ(result_edge.metadata().at(meta::keys::color()), (Color{255, 0, 0}));
    EXPECT_EQ(result_edge.metadata().at(weight), 7);

    auto next_result = MetadataCopyOperation({result}).value(modeling_context);
    auto next_edge = next_result.select_subshapes(is_same(edge)).unique_element();
    EXPECT_FALSE(next_edge.metadata().contains(weight));
}

TEST(MetadataTrack, rejects_nonpositive_step_counts)
{
    EXPECT_THROW(geoml::TrackLifetime::steps(0), std::invalid_argument);
    EXPECT_THROW(geoml::TrackLifetime::steps(-1), std::invalid_argument);
}

TEST(MetadataTrack, failed_registration_commits_no_target_values_or_rule)
{
    auto merge_count = std::make_shared<int>(0);
    const geoml::metadata::Key<int> throwing_track_key{"test.track-throw"};
    geoml::metadata::register_default_resolver(throwing_track_key,
        [merge_count](geoml::metadata::ResolutionContext<int> const& context) {
            if (++*merge_count == 2) {
                throw std::runtime_error("second target resolver failed");
            }
            return context.contributions.front().value;
        });
    geoml::ModelingContext modeling_context;
    geoml::Shape source{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto all_shapes = source.get_subshapes();
    ASSERT_GT(all_shapes.size(), 1u);
    for (auto& target : all_shapes) {
        target.metadata().set(throwing_track_key, 1);
    }

    EXPECT_THROW(modeling_context.add_metadata_track(
                     source, [](geoml::Shape const&) { return true; }, throwing_track_key, 2,
                     geoml::TrackLifetime::steps(2)),
                 std::runtime_error);
    EXPECT_EQ(*merge_count, 2);

    for (auto const& target : all_shapes) {
        EXPECT_EQ(target.metadata().at(throwing_track_key), 1);
    }

    MetadataCopyOperation operation({source});
    auto result = operation.value(modeling_context);
    EXPECT_FALSE(result.metadata().contains(throwing_track_key));
}

TEST(MetadataTrack, selected_subshape_track_is_visible_from_model_root)
{
    geoml::ModelingContext context;
    geoml::Shape source{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto faces = source.select_subshapes(
        [](geoml::Shape const& candidate) { return candidate.is_type(TopAbs_FACE); });
    ASSERT_FALSE(faces.direct_subshapes().empty());
    auto tracked_face = faces.direct_subshapes().front();
    auto const tracked_topology = tracked_face.shape();

    context.add_metadata_track(
        tracked_face,
        [tracked_topology](geoml::Shape const& candidate) {
            return candidate.is_same(tracked_topology);
        }, geoml::metadata::keys::name(), "tracked-face",
        geoml::TrackLifetime::steps(1));

    MetadataCopyOperation operation({source});
    auto result = operation.value(context);
    auto result_faces = result.select_subshapes(
        [](geoml::Shape const& candidate) { return candidate.is_type(TopAbs_FACE); });

    bool found_tracked_face = false;
    for (auto const& face : result_faces.direct_subshapes()) {
        if (face.is_same(tracked_topology)) {
            EXPECT_EQ(face.metadata().at(geoml::metadata::keys::name()), "tracked-face");
            found_tracked_face = true;
            break;
        }
    }
    EXPECT_TRUE(found_tracked_face);

    MetadataCopyOperation selection_operation({faces});
    auto selection_result = selection_operation.value(context);
    bool found_tracked_face_in_selection = false;
    for (auto const& face : selection_result.direct_subshapes()) {
        if (face.is_same(tracked_topology)) {
            EXPECT_EQ(face.metadata().at(geoml::metadata::keys::name()), "tracked-face");
            found_tracked_face_in_selection = true;
            break;
        }
    }
    EXPECT_TRUE(found_tracked_face_in_selection);
}

TEST(MetadataTrack, mismatch_keeps_track_and_lifetime_branches_independently)
{
    geoml::ModelingContext context;
    geoml::Shape source{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto const source_topology = source.shape();
    auto matching_calls = std::make_shared<int>(0);
    context.add_metadata_track(
        source,
        [source_topology, matching_calls](geoml::Shape const& candidate) {
            if (!candidate.is_same(source_topology)) {
                return false;
            }
            ++*matching_calls;
            return *matching_calls == 1 || *matching_calls >= 3;
        },
        geoml::metadata::keys::name(), std::string{"persists"}, geoml::TrackLifetime::steps(2));

    MetadataCopyOperation first_operation({source});
    auto first_result = first_operation.value(context);
    EXPECT_EQ(*matching_calls, 2);
    EXPECT_FALSE(first_result.metadata().contains(geoml::metadata::keys::name()));

    MetadataCopyOperation first_branch_operation({first_result});
    auto first_branch = first_branch_operation.value(context);
    MetadataCopyOperation second_branch_operation({first_result});
    auto second_branch = second_branch_operation.value(context);

    EXPECT_EQ(first_branch.metadata().at(geoml::metadata::keys::name()), "persists");
    EXPECT_EQ(second_branch.metadata().at(geoml::metadata::keys::name()), "persists");
    EXPECT_EQ(*matching_calls, 4);

    MetadataCopyOperation after_expiration_operation({first_branch});
    auto after_expiration = after_expiration_operation.value(context);
    EXPECT_FALSE(after_expiration.metadata().contains(geoml::metadata::keys::name()));
    EXPECT_EQ(first_branch.metadata().at(geoml::metadata::keys::name()), "persists");
    EXPECT_EQ(*matching_calls, 4);
}

TEST(MetadataTrack, shared_track_reports_both_input_positions_and_propagates_once)
{
    using namespace geoml;
    metadata::Key<int> load{"test.load"};
    metadata::register_default_resolver(load, [](metadata::ResolutionContext<int> const& context) {
        if (context.contributions.size() == 1) {
            return context.contributions.front().value;
        }
        EXPECT_EQ(context.contributions.size(), 2u);
        auto const& first = context.contributions.at(0);
        auto const& second = context.contributions.at(1);
        EXPECT_EQ(first.input_position, 0u);
        EXPECT_EQ(second.input_position, 1u);
        EXPECT_EQ(first.source_track_id, second.source_track_id);
        return first.value + second.value;
    });
    ModelingContext modeling_context;
    Shape vertex{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 2., 3.}).Shape()};
    modeling_context.add_metadata_track(vertex, [](Shape const&) { return true; }, load, 3, TrackLifetime::steps(2));

    auto joined = MetadataCopyOperation({vertex, vertex}).value(modeling_context);
    EXPECT_EQ(joined.metadata().at(load), 6);
    auto next = MetadataCopyOperation({joined}).value(modeling_context);
    EXPECT_EQ(next.metadata().at(load), 3);
    EXPECT_FALSE(MetadataCopyOperation({next}).value(modeling_context).metadata().contains(load));
}

TEST(MetadataTrack, cancellation_stops_all_branches_in_context_and_keeps_written_values)
{
    using namespace geoml;
    auto const& tags = metadata::keys::tags();
    ModelingContext context;
    Shape input{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    auto handle = context.add_tag_track(input, is_vertex, "stop");
    context.add_tag_track(input, is_vertex, "keep");
    ASSERT_TRUE(handle);

    MetadataCopyOperation branch({input});
    auto first = branch.value(context);
    auto sibling = branch.value(context);
    auto later = MetadataCopyOperation({first}).value(context);

    EXPECT_TRUE(context.cancel_metadata_track(handle));
    EXPECT_FALSE(context.cancel_metadata_track(handle));

    // Previously written metadata stays intact.
    EXPECT_TRUE(input.has_tag("stop"));
    EXPECT_TRUE(first.has_tag("stop"));
    EXPECT_TRUE(sibling.has_tag("stop"));
    EXPECT_TRUE(later.has_tag("stop"));

    // All branches in the context stop propagating the cancelled rule.
    auto const expected_remaining = std::vector<std::string>{"keep"};
    EXPECT_EQ(MetadataCopyOperation({later}).value(context).metadata().at(tags), expected_remaining);
    EXPECT_EQ(MetadataCopyOperation({first}).value(context).metadata().at(tags), expected_remaining);
    EXPECT_EQ(MetadataCopyOperation({sibling}).value(context).metadata().at(tags),
              expected_remaining);
    EXPECT_EQ(branch.value(context).metadata().at(tags), expected_remaining);
}

TEST(MetadataTrack, cancelling_a_rejoined_rule_removes_all_inherited_copies)
{
    using namespace geoml;
    metadata::Key<int> load{"test.rejoined-track"};
    metadata::register_default_resolver(load, [](metadata::ResolutionContext<int> const& context) {
        int result = 0;
        for (auto const& contribution : context.contributions) {
            result += contribution.value;
        }
        return result;
    });
    ModelingContext modeling_context;
    Shape input{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    auto handle = modeling_context.add_metadata_track(input, is_vertex, load, 3);
    MetadataCopyOperation branch({input});
    auto left = branch.value(modeling_context);
    auto right = branch.value(modeling_context);
    auto joined = MetadataCopyOperation({left, right}).value(modeling_context);

    EXPECT_EQ(joined.metadata().at(load), 6);
    EXPECT_TRUE(modeling_context.cancel_metadata_track(handle));
    EXPECT_EQ(joined.metadata().at(load), 6);
    EXPECT_FALSE(MetadataCopyOperation({joined}).value(modeling_context).metadata().contains(load));
    EXPECT_FALSE(MetadataCopyOperation({left}).value(modeling_context).metadata().contains(load));
    EXPECT_FALSE(MetadataCopyOperation({right}).value(modeling_context).metadata().contains(load));
}

TEST(MetadataTrack, snapshot_handle_is_empty_without_values_and_cannot_cancel_an_expired_track)
{
    using namespace geoml;
    auto const& key = metadata::keys::name();
    ModelingContext context;
    Shape source{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 0., 0.}).Shape()};
    Shape target{BRepBuilderAPI_MakeVertex(gp_Pnt{}).Shape()};
    auto empty = context.add_metadata_track_from(target, source, is_vertex);
    EXPECT_FALSE(empty);
    EXPECT_FALSE(context.cancel_metadata_track(empty));

    source.metadata().set(key, "snapshot");
    auto handle = context.add_metadata_track_from(target, source, is_vertex, TrackLifetime::steps(1));
    ASSERT_TRUE(handle);
    auto result = MetadataCopyOperation({target}).value(context);
    EXPECT_EQ(result.metadata().at(key), "snapshot");
    // The original track on `target` is still active in `context`, while `result` has no remaining steps.
    EXPECT_TRUE(context.cancel_metadata_track(handle));
    EXPECT_FALSE(context.cancel_metadata_track(handle));
    EXPECT_EQ(target.metadata().at(key), "snapshot");
    EXPECT_FALSE(MetadataCopyOperation({target}).value(context).metadata().contains(key));
}

TEST(MetadataResolution, nested_policy_scopes_are_deterministic_and_unwind_on_exception)
{
    geoml::ModelingContext context;
    geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape right{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    context.add_metadata_track(
        left, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "left",
        geoml::TrackLifetime::steps(3));
    context.add_metadata_track(
        right, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "right",
        geoml::TrackLifetime::steps(3));

    MetadataCopyOperation operation({left, right});
    EXPECT_THROW(operation.value(context), geoml::metadata::MetadataConflictException);
    EXPECT_EQ(left.metadata().at(geoml::metadata::keys::name()), "left");
    EXPECT_EQ(right.metadata().at(geoml::metadata::keys::name()), "right");

    MetadataCopyOperation left_after_conflict({left});
    MetadataCopyOperation right_after_conflict({right});
    EXPECT_EQ(left_after_conflict.value(context).metadata().at(geoml::metadata::keys::name()), "left");
    EXPECT_EQ(right_after_conflict.value(context).metadata().at(geoml::metadata::keys::name()), "right");

    {
        auto outer_scope = context.create_policy_scope(
            geoml::metadata::MetadataPolicy{}.for_key(geoml::metadata::keys::name(),
                                                      geoml::metadata::prefer_input(1)));
        EXPECT_EQ(operation.value(context).metadata().at(geoml::metadata::keys::name()), "right");

        {
            auto inner_scope = context.create_policy_scope(
                geoml::metadata::MetadataPolicy{}.for_key(geoml::metadata::keys::name(),
                                                          geoml::metadata::prefer_input(0)));
            EXPECT_EQ(operation.value(context).metadata().at(geoml::metadata::keys::name()), "left");
        }

        EXPECT_EQ(operation.value(context).metadata().at(geoml::metadata::keys::name()), "right");
    }

    {
        auto missing_preferred_scope = context.create_policy_scope(
            geoml::metadata::MetadataPolicy{}.for_key(geoml::metadata::keys::name(),
                                                      geoml::metadata::prefer_input(9)));
        EXPECT_THROW(operation.value(context), geoml::metadata::MetadataConflictException);
    }

    try {
        auto throwing_scope = context.create_policy_scope(
            geoml::metadata::MetadataPolicy{}.for_key(geoml::metadata::keys::name(),
                                                      geoml::metadata::prefer_input(0)));
        throw std::runtime_error("scope unwinding");
    }
    catch (std::runtime_error const&) {
    }
    EXPECT_THROW(operation.value(context), geoml::metadata::MetadataConflictException);
}

TEST(MetadataResolution, resolver_receives_complete_owned_operation_context)
{
    geoml::ModelingContext modeling_context;
    geoml::Shape first{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape second{BRepPrimAPI_MakeBox(2., 1., 1.).Shape()};
    geoml::Shape third{BRepPrimAPI_MakeBox(3., 1., 1.).Shape()};
    modeling_context.add_metadata_track(
        first, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "first",
        geoml::TrackLifetime::steps(2));
    modeling_context.add_metadata_track(
        second, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "second",
        geoml::TrackLifetime::steps(2));
    modeling_context.add_metadata_track(
        third, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "third",
        geoml::TrackLifetime::steps(2));
    MetadataCopyOperation operation({first, second, third});

    try {
        operation.value(modeling_context);
        FAIL() << "Expected a structured metadata conflict";
    }
    catch (geoml::metadata::MetadataConflictException const& error) {
        EXPECT_EQ(error.contributions().size(), 3u);
        ASSERT_TRUE(error.target_topology());
        EXPECT_FALSE(error.target_topology()->IsNull());
        for (std::size_t i = 0; i < error.contributions().size(); ++i) {
            auto const& contribution = error.contributions()[i];
            ASSERT_TRUE(contribution.input_position);
            EXPECT_EQ(*contribution.input_position, i);
            ASSERT_TRUE(contribution.source_track_id);
            EXPECT_NE(*contribution.source_track_id, 0u);
            ASSERT_TRUE(contribution.source_topology);
            EXPECT_FALSE(contribution.source_topology->IsNull());
            auto const value = std::any_cast<std::string>(&contribution.value);
            ASSERT_NE(value, nullptr);
            EXPECT_EQ(*value, (std::vector<std::string>{"first", "second", "third"})[i]);
        }
    }

    auto contexts = std::make_shared<std::vector<geoml::metadata::ResolutionContext<std::string>>>();
    geoml::metadata::MetadataPolicy policy;
    policy.for_key(geoml::metadata::keys::name(),
                   [contexts, separator = std::string("|")](
                       geoml::metadata::ResolutionContext<std::string> const& context) -> std::string {
                       contexts->push_back(context);
                       std::string result;
                       for (auto const& contribution : context.contributions) {
                           if (!result.empty()) {
                               result += separator;
                           }
                           result += contribution.value;
                       }
                       return result;
                   });

    {
        auto scope = modeling_context.create_policy_scope(policy);
        auto result = operation.value(modeling_context);
        EXPECT_EQ(result.metadata().at(geoml::metadata::keys::name()), "first|second|third");
        auto const target_context = std::find_if(
            contexts->begin(), contexts->end(), [&](auto const& context) {
                return context.target_topology && context.target_topology->IsSame(result.shape());
            });
        ASSERT_NE(target_context, contexts->end());
        ASSERT_EQ(target_context->contributions.size(), 3u);
        EXPECT_EQ(target_context->contributions[0].input_position, 0u);
        EXPECT_EQ(target_context->contributions[1].input_position, 1u);
        EXPECT_EQ(target_context->contributions[2].input_position, 2u);
        EXPECT_TRUE(target_context->contributions[0].source_topology->IsSame(first.shape()));
        EXPECT_TRUE(target_context->contributions[1].source_topology->IsSame(second.shape()));
        EXPECT_TRUE(target_context->contributions[2].source_topology->IsSame(third.shape()));
    }

    const geoml::metadata::Key<int> wrong_schema{"name"};
    geoml::metadata::MetadataPolicy invalid_policy;
    invalid_policy.for_key(wrong_schema, [](geoml::metadata::ResolutionContext<int> const&) {
        return 1;
    });
    auto invalid_scope = modeling_context.create_policy_scope(invalid_policy);
    EXPECT_THROW(operation.value(modeling_context), std::logic_error);
}

TEST(MetadataResolution, capturing_scopes_are_context_local_and_inherit_per_key)
{
    geoml::ModelingContext modeling_context;
    geoml::ModelingContext other_context;
    geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape right{BRepPrimAPI_MakeBox(2., 1., 1.).Shape()};
    for (auto const& shape_value : {std::make_pair(&left, "left"),
                                    std::make_pair(&right, "right")}) {
        modeling_context.add_metadata_track(
            *shape_value.first, [](geoml::Shape const&) { return true; },
            geoml::metadata::keys::name(), std::string(shape_value.second),
            geoml::TrackLifetime::steps(3));
        modeling_context.add_metadata_track(
            *shape_value.first, [](geoml::Shape const&) { return true; },
            detail_key, std::string(shape_value.second) + "-detail",
            geoml::TrackLifetime::steps(3));
        other_context.add_metadata_track(
            *shape_value.first, [](geoml::Shape const&) { return true; },
            geoml::metadata::keys::name(), std::string(shape_value.second),
            geoml::TrackLifetime::steps(3));
    }
    MetadataCopyOperation operation({left, right});

    geoml::metadata::MetadataPolicy outer_policy;
    outer_policy.for_key(geoml::metadata::keys::name(),
                         [suffix = std::string("-outer")](
                             geoml::metadata::ResolutionContext<std::string> const& context)
                             -> std::string {
                             return context.contributions.front().value + suffix;
                         });
    outer_policy.for_key(detail_key,
                         [](geoml::metadata::ResolutionContext<std::string> const& context) {
                             return context.contributions.front().value + "-outer";
                         });
    auto outer_scope = modeling_context.create_policy_scope(outer_policy);
    EXPECT_EQ(operation.value(modeling_context).metadata().at(geoml::metadata::keys::name()), "left-outer");
    // A separate context does not inherit outer_scope.
    EXPECT_THROW(operation.value(other_context), geoml::metadata::MetadataConflictException);

    geoml::metadata::MetadataPolicy inner_policy;
    inner_policy.for_key(detail_key,
                         [suffix = std::string("-inner")](
                             geoml::metadata::ResolutionContext<std::string> const& context)
                             -> std::string {
                             return context.contributions.back().value + suffix;
                         });
    {
        auto inner_scope = modeling_context.create_policy_scope(inner_policy);
        auto nested = operation.value(modeling_context);
        EXPECT_EQ(nested.metadata().at(geoml::metadata::keys::name()), "left-outer");
        EXPECT_EQ(nested.metadata().at(detail_key), "right-detail-inner");
    }

    geoml::metadata::MetadataPolicy failing_inner_policy;
    failing_inner_policy.for_key(detail_key,
                                 [](geoml::metadata::ResolutionContext<std::string> const&)
                                     -> std::string {
                                     throw std::runtime_error("inner resolver failed");
                                 });
    {
        auto failing_inner_scope = modeling_context.create_policy_scope(failing_inner_policy);
        EXPECT_THROW(operation.value(modeling_context), std::runtime_error);
    }
    EXPECT_EQ(operation.value(modeling_context).metadata().at(detail_key), "left-detail-outer");
}

TEST(MetadataResolution, prefer_input_keeps_conflicts_inside_the_preferred_input)
{
    geoml::metadata::ResolutionContext<int> context;
    context.key_name = "test.prefer-input";
    context.existing_value = 1;
    context.key_default = geoml::metadata::RequireEqualResolver{};
    context.contributions = {{2, 0u, 11u, std::nullopt},
                             {3, 1u, 12u, std::nullopt}};

    EXPECT_EQ(geoml::metadata::prefer_input(0)(context), 2);
    context.contributions.push_back({4, 0u, 13u, std::nullopt});
    EXPECT_THROW(geoml::metadata::prefer_input(0)(context),
                 geoml::metadata::MetadataConflictException);
    for (auto& contribution : context.contributions) {
        contribution.input_position = 5;
    }
    EXPECT_THROW(geoml::metadata::prefer_input(0)(context),
                 geoml::metadata::MetadataConflictException);
}

TEST(MetadataResolution, preferred_contribution_replaces_unrelated_existing_target)
{
    using namespace geoml;
    using namespace geoml::metadata;
    const Key<int> key{"test.preferred-existing-target"};
    auto default_calls = std::make_shared<int>(0);
    register_default_resolver(key, [default_calls](ResolutionContext<int> const& context) {
        ++*default_calls;
        return RequireEqualResolver{}(context);
    });

    class ExistingTargetOperation : public Operation<ExistingTargetOperation>
    {
    public:
        ExistingTargetOperation(std::vector<Shape> const& inputs, Key<int> const& key)
            : Operation(inputs), m_key(key) {}

        Shape perform()
        {
            Shape result{m_inputs.front().shape()};
            result.metadata().set(m_key, 1);
            return result;
        }

        void map_subshapes(Shape&) {}

    private:
        Key<int> m_key;
    };

    ModelingContext context;
    Shape left{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 0., 0.}).Shape()};
    Shape right{BRepBuilderAPI_MakeVertex(gp_Pnt{2., 0., 0.}).Shape()};
    context.add_metadata_track(left, is_vertex, key, 2);
    context.add_metadata_track(right, is_vertex, key, 3);
    *default_calls = 0;

    MetadataPolicy policy;
    policy.for_key(key, prefer_input(0));
    auto scope = context.create_policy_scope(policy);
    EXPECT_EQ(ExistingTargetOperation({left, right}, key).value(context).metadata().at(key), 2);
    EXPECT_EQ(*default_calls, 1);
}

TEST(MetadataResolution, throwing_policy_preserves_inputs_and_scope_state)
{
    geoml::ModelingContext context;
    geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape right{BRepPrimAPI_MakeBox(2., 1., 1.).Shape()};
    context.add_metadata_track(
        left, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "left",
        geoml::TrackLifetime::steps(2));
    context.add_metadata_track(
        right, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "right",
        geoml::TrackLifetime::steps(2));
    MetadataCopyOperation operation({left, right});

    geoml::metadata::MetadataPolicy throwing_policy;
    throwing_policy.for_key(geoml::metadata::keys::name(),
                            [](geoml::metadata::ResolutionContext<std::string> const&)
                                -> std::string {
                                throw std::runtime_error("operation resolver failed");
                            });
    {
        auto scope = context.create_policy_scope(throwing_policy);
        EXPECT_THROW(operation.value(context), std::runtime_error);
        EXPECT_THROW(operation.value(context), std::runtime_error);
    }
    EXPECT_THROW(operation.value(context), geoml::metadata::MetadataConflictException);
    EXPECT_EQ(left.metadata().at(geoml::metadata::keys::name()), "left");
    EXPECT_EQ(right.metadata().at(geoml::metadata::keys::name()), "right");

    geoml::metadata::MetadataPolicy succeeding_policy;
    succeeding_policy.for_key(geoml::metadata::keys::name(),
                              [](geoml::metadata::ResolutionContext<std::string> const&) {
                                  return std::string("resolved");
                              });
    auto scope = context.create_policy_scope(succeeding_policy);
    EXPECT_EQ(operation.value(context).metadata().at(geoml::metadata::keys::name()),
              "resolved");
}

TEST(MetadataResolution, policies_override_tag_union_without_changing_tracks)
{
    using namespace geoml;
    using namespace geoml::metadata;
    auto const& tags = keys::tags();
    ModelingContext context;
    Shape left{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 0., 0.}).Shape()};
    Shape right{BRepBuilderAPI_MakeVertex(gp_Pnt{2., 0., 0.}).Shape()};
    context.add_metadata_track(left, is_vertex, tags, {"left", "shared"});
    context.add_metadata_track(right, is_vertex, tags, {"shared", "right"});

    MetadataCopyOperation operation({left, right});
    std::vector<std::string> const all_tags{"left", "shared", "right"};
    EXPECT_EQ(operation.value(context).metadata().at(tags), all_tags);

    MetadataPolicy outer_policy;
    outer_policy.for_key(tags, prefer_input(1));
    MetadataPolicy inner_policy;
    inner_policy.for_key(tags, ReplaceResolver{});
    // A policy can replace a resolver previously set with a different key object.
    inner_policy.for_key(Key<std::vector<std::string>>{"tags"},
                         [](ResolutionContext<std::vector<std::string>> const&) {
                             return std::vector<std::string>{"inner"};
                         });
    MetadataPolicy prefer_left_policy;
    prefer_left_policy.for_key(Key<std::vector<std::string>>{"tags"}, prefer_input(0));
    {
        auto outer = context.create_policy_scope(outer_policy);
        EXPECT_EQ(operation.value(context).metadata().at(tags),
                  (std::vector<std::string>{"shared", "right"}));
        {
            auto inner = context.create_policy_scope(inner_policy);
            EXPECT_EQ(operation.value(context).metadata().at(tags),
                      (std::vector<std::string>{"inner"}));
            {
                auto deepest = context.create_policy_scope(prefer_left_policy);
                EXPECT_EQ(operation.value(context).metadata().at(tags),
                          (std::vector<std::string>{"left", "shared"}));
            }
            EXPECT_EQ(operation.value(context).metadata().at(tags),
                      (std::vector<std::string>{"inner"}));
        }
        EXPECT_EQ(operation.value(context).metadata().at(tags),
                  (std::vector<std::string>{"shared", "right"}));
    }

    Shape preferred;
    {
        auto prefer_left = context.create_policy_scope(prefer_left_policy);
        preferred = operation.value(context);
    }
    EXPECT_EQ(preferred.metadata().at(tags), (std::vector<std::string>{"left", "shared"}));
    EXPECT_EQ(MetadataCopyOperation({preferred}).value(context).metadata().at(tags), all_tags);
    EXPECT_EQ(left.metadata().at(tags), (std::vector<std::string>{"left", "shared"}));
    EXPECT_EQ(right.metadata().at(tags), (std::vector<std::string>{"shared", "right"}));
}

TEST(MetadataTrack, boolean_union_rejects_incompatible_key_types_across_tracks)
{
    const geoml::metadata::Key<int> left_key{"test.r4.track-definition"};
    const geoml::metadata::Key<std::string> right_key{"test.r4.track-definition"};

    auto make_inputs = [&](geoml::ModelingContext& context) {
        geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
        geoml::Shape right{BRepPrimAPI_MakeBox(gp_Pnt(3., 0., 0.), 1., 1., 1.).Shape()};
        auto const left_topology = left.shape();
        auto const right_topology = right.shape();
        context.add_metadata_track(
            left,
            [left_topology](geoml::Shape const& candidate) {
                return candidate.is_same(left_topology);
            }, left_key, 11,
            geoml::TrackLifetime::steps(1));
        context.add_metadata_track(
            right,
            [right_topology](geoml::Shape const& candidate) {
                return candidate.is_same(right_topology);
            }, right_key, "right",
            geoml::TrackLifetime::steps(1));
        return std::make_pair(std::move(left), std::move(right));
    };

    auto expect_named_type_conflict = [](geoml::Shape const& first,
                                         geoml::Shape const& second,
                                         geoml::ModelingContext& context) {
        try {
            (void)geoml::boolean_union(first, second, context);
            FAIL() << "Expected incompatible track key types to fail";
        }
        catch (std::logic_error const& error) {
            EXPECT_NE(std::string(error.what()).find("test.r4.track-definition"),
                      std::string::npos);
        }
    };

    {
        geoml::ModelingContext context;
        auto forward = make_inputs(context);
        expect_named_type_conflict(forward.first, forward.second, context);
    }
    {
        geoml::ModelingContext context;
        auto reversed = make_inputs(context);
        expect_named_type_conflict(reversed.second, reversed.first, context);
    }
}

TEST(MetadataTrack, boolean_union_accepts_independent_keys_on_separate_solids)
{
    const geoml::metadata::Key<int> canonical_key{"test.r4.canonical-track-definition"};
    const geoml::metadata::Key<int> another_key{canonical_key.name()};

    geoml::ModelingContext context;
    geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape right{BRepPrimAPI_MakeBox(gp_Pnt(3., 0., 0.), 1., 1., 1.).Shape()};
    auto const left_topology = left.shape();
    auto const right_topology = right.shape();
    context.add_metadata_track(
        left,
        [left_topology](geoml::Shape const& candidate) {
            return candidate.is_same(left_topology);
        }, canonical_key, 11,
        geoml::TrackLifetime::steps(1));
    context.add_metadata_track(
        right,
        [right_topology](geoml::Shape const& candidate) {
            return candidate.is_same(right_topology);
        }, another_key, 22,
        geoml::TrackLifetime::steps(1));

    auto result = geoml::boolean_union(left, right, context);
    auto solids = result.select_subshapes([](geoml::Shape const& candidate) {
        return candidate.is_type(TopAbs_SOLID);
    });

    bool saw_left_value = false;
    bool saw_right_value = false;
    for (auto const& solid : solids) {
        auto value = solid.metadata().find(canonical_key);
        if (value && *value == 11) {
            saw_left_value = true;
        }
        if (value && *value == 22) {
            saw_right_value = true;
        }
    }
    EXPECT_TRUE(saw_left_value);
    EXPECT_TRUE(saw_right_value);
}

TEST(MetadataResolution, public_modeling_operations_use_context_and_policy_scopes)
{
    geoml::ModelingContext context;
    geoml::Shape left{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    geoml::Shape right{BRepPrimAPI_MakeBox(gp_Pnt(2., 0., 0.), 1., 1., 1.).Shape()};
    context.add_metadata_track(
        left, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "left",
        geoml::TrackLifetime::steps(1));
    context.add_metadata_track(
        right, [](geoml::Shape const&) { return true; }, geoml::metadata::keys::name(), "right",
        geoml::TrackLifetime::steps(1));

    // Without a context, modeling operations do not propagate tracked metadata.
    auto untracked = geoml::boolean_union(left, right);
    EXPECT_FALSE(untracked.metadata().contains(geoml::metadata::keys::name()));

    auto scope = context.create_policy_scope(
        geoml::metadata::MetadataPolicy{}.for_key(geoml::metadata::keys::name(),
                                                  geoml::metadata::prefer_input(0)));
    auto result = geoml::boolean_union(left, right, context);
    EXPECT_EQ(result.metadata().at(geoml::metadata::keys::name()), "left");
}

TEST(ModelingContext, ownership_cycles_are_released_and_lookup_keys_do_not_extend_lifetimes)
{
    using namespace geoml;
    using namespace geoml::metadata;
    const Key<std::shared_ptr<int>> sentinel_key{"test.lifetime-sentinel"};

    // 1. Self-referencing predicates capturing the tracked shape release everything on context exit.
    std::weak_ptr<int> metadata_sentinel;
    std::weak_ptr<int> predicate_sentinel;
    {
        ModelingContext context;
        Shape box{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
        auto meta_token = std::make_shared<int>(1);
        auto pred_token = std::make_shared<int>(2);
        metadata_sentinel = meta_token;
        predicate_sentinel = pred_token;
        box.metadata().set(sentinel_key, std::move(meta_token));

        // Direct reproducer from issue #94 plus direct lambda capture and Shape-capturing predicates.
        context.add_tag_track(box, is_descendent_of(box), "Box", TrackLifetime::steps(10));
        context.add_tag_track(box, is_unmodified_descendent_of(box), "Unmodified");
        context.add_tag_track(box, has_subshape(box), "HasSelf");
        context.add_tag_track(
            box,
            [box, pred_token = std::move(pred_token)](Shape const& candidate) {
                return candidate.is_descendent_of(box);
            },
            "CapturedLambda");
        EXPECT_TRUE(box.has_tag("Box"));
        EXPECT_TRUE(box.has_tag("CapturedLambda"));
    }
    EXPECT_TRUE(metadata_sentinel.expired());
    EXPECT_TRUE(predicate_sentinel.expired());

    // 2. Destroying the context releases captured references even while the shape stays alive.
    std::weak_ptr<int> context_owned_token;
    Shape surviving_box{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    {
        ModelingContext context;
        auto token = std::make_shared<int>(3);
        context_owned_token = token;
        context.add_tag_track(
            surviving_box,
            [surviving_box, token = std::move(token)](Shape const& candidate) {
                return candidate.is_same(surviving_box);
            },
            "ScopedRule");
        EXPECT_FALSE(context_owned_token.expired());
    }
    EXPECT_TRUE(context_owned_token.expired());
    EXPECT_TRUE(surviving_box.has_tag("ScopedRule"));

    // 3. Context lookup keys do not extend shape/metadata lifetime when predicates do not capture the shape.
    ModelingContext long_lived_context;
    std::weak_ptr<int> ephemeral_metadata_token;
    {
        Shape ephemeral{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
        auto token = std::make_shared<int>(4);
        ephemeral_metadata_token = token;
        ephemeral.metadata().set(sentinel_key, std::move(token));
        long_lived_context.add_tag_track(ephemeral, is_face, "Face");
        EXPECT_FALSE(ephemeral_metadata_token.expired());
    }
    EXPECT_TRUE(ephemeral_metadata_token.expired());
}

TEST(ModelingContext, shared_edge_track_registered_via_one_face_propagates_through_other_face)
{
    using namespace geoml;
    using namespace geoml::metadata;

    ModelingContext context;
    Shape box{BRepPrimAPI_MakeBox(1., 1., 1.).Shape()};
    auto faces = box.select_subshapes(is_face);
    ASSERT_GT(faces.size(), 1u);

    Shape face_one;
    Shape face_two;
    Shape edge_via_face_one;
    Shape edge_via_face_two;
    bool found_shared_edge = false;
    for (std::size_t i = 0; i < faces.size() && !found_shared_edge; ++i) {
        auto edges_i = faces[i].select_subshapes(is_edge);
        for (std::size_t j = i + 1; j < faces.size() && !found_shared_edge; ++j) {
            for (auto const& candidate_edge : edges_i) {
                auto matches = faces[j].select_subshapes(is_edge && is_same(candidate_edge));
                if (matches.size() == 1u) {
                    face_one = faces[i];
                    face_two = faces[j];
                    edge_via_face_one = candidate_edge;
                    edge_via_face_two = matches.unique_element();
                    found_shared_edge = true;
                    break;
                }
            }
        }
    }
    ASSERT_TRUE(found_shared_edge);

    auto const tracked_edge_topology = edge_via_face_one.shape();
    context.add_metadata_track(
        edge_via_face_one,
        is_edge && is_same(tracked_edge_topology),
        keys::name(),
        std::string{"shared-edge"},
        TrackLifetime::steps(2));

    // Immediate evaluation is visible through the other face's handle.
    EXPECT_EQ(edge_via_face_two.metadata().at(keys::name()), "shared-edge");

    // Modeling with the handle selected via face_two propagates the track.
    auto copied_edge = MetadataCopyOperation({edge_via_face_two}).value(context);
    EXPECT_EQ(copied_edge.metadata().at(keys::name()), "shared-edge");

    // Modeling with face_two itself also discovers the track on its shared edge.
    auto copied_face = MetadataCopyOperation({face_two}).value(context);
    auto copied_face_edge = copied_face.select_subshapes(
        is_edge && is_same(tracked_edge_topology)).unique_element();
    EXPECT_EQ(copied_face_edge.metadata().at(keys::name()), "shared-edge");

    // Modeling with the full box collects the shared edge's track exactly once.
    auto copied_box = MetadataCopyOperation({box}).value(context);
    auto copied_box_edge = copied_box.select_subshapes(
        is_edge && is_same(tracked_edge_topology)).unique_element();
    EXPECT_EQ(copied_box_edge.metadata().at(keys::name()), "shared-edge");

    // An independent GeoML model wrapping the same OCCT topology does not share the track.
    Shape independent_box{box.shape()};
    auto independent_result = MetadataCopyOperation({independent_box}).value(context);
    auto independent_edge = independent_result.select_subshapes(
        is_edge && is_same(tracked_edge_topology)).unique_element();
    EXPECT_FALSE(independent_edge.metadata().contains(keys::name()));
}

TEST(ModelingContext, contexts_isolate_tracks_cancellation_and_policy_scopes)
{
    using namespace geoml;
    using namespace geoml::metadata;

    ModelingContext context_a;
    ModelingContext context_b;
    Shape left{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 0., 0.}).Shape()};
    Shape right{BRepBuilderAPI_MakeVertex(gp_Pnt{2., 0., 0.}).Shape()};

    auto handle_a = context_a.add_metadata_track(left, is_vertex, keys::name(), "ctx-a-left");
    context_a.add_metadata_track(right, is_vertex, keys::name(), "ctx-a-right");

    // Reset stored values on inputs before registering into context_b to keep direct metadata clean.
    left.metadata().erase(keys::name());
    right.metadata().erase(keys::name());

    auto handle_b = context_b.add_tag_track(left, is_vertex, "ctx-b-only");

    auto scope_a = context_a.create_policy_scope(
        MetadataPolicy{}.for_key(keys::name(), prefer_input(0)));

    MetadataCopyOperation operation({left, right});
    auto result_a = operation.value(context_a);
    EXPECT_EQ(result_a.metadata().at(keys::name()), "ctx-a-left");
    EXPECT_FALSE(result_a.has_tag("ctx-b-only"));

    auto result_b = operation.value(context_b);
    EXPECT_FALSE(result_b.metadata().contains(keys::name()));
    EXPECT_TRUE(result_b.has_tag("ctx-b-only"));

    auto result_untracked = operation.value();
    EXPECT_FALSE(result_untracked.metadata().contains(keys::name()));
    EXPECT_FALSE(result_untracked.has_tag("ctx-b-only"));

    // Cancelling a handle from context_a has no effect on context_b.
    EXPECT_FALSE(context_b.cancel_metadata_track(handle_a));
    EXPECT_TRUE(context_a.cancel_metadata_track(handle_a));
    EXPECT_TRUE(operation.value(context_b).has_tag("ctx-b-only"));
    EXPECT_TRUE(context_b.cancel_metadata_track(handle_b));
    EXPECT_FALSE(operation.value(context_b).has_tag("ctx-b-only"));
}

TEST(ModelingContext, make_fillet_propagates_tracked_metadata_with_context)
{
    using namespace geoml;
    using namespace geoml::metadata;

    ModelingContext context;
    Shape box{BRepPrimAPI_MakeBox(2., 2., 2.).Shape()};
    auto edges = box.select_subshapes(is_edge);
    ASSERT_GE(edges.size(), 2u);

    auto fillet_edge = edges[0];
    auto faces = box.select_subshapes(is_face);
    ASSERT_GT(faces.size(), 0u);
    auto tracked_face = faces[0];

    context.add_tag_track(box, is_face && is_descendent_of(tracked_face), "fillet-tracked-face");

    auto untracked_fillet = make_fillet(box, fillet_edge, 0.2);
    EXPECT_TRUE(untracked_fillet.select_subshapes(is_face && has_metadata_tag("fillet-tracked-face")).is_empty());

    auto tracked_fillet = make_fillet(box, fillet_edge, 0.2, context);
    auto matching_faces =
        tracked_fillet.select_subshapes(is_face && has_metadata_tag("fillet-tracked-face"));
    EXPECT_FALSE(matching_faces.is_empty());
}

TEST(ModelingContext, policy_scope_handles_moves_and_context_destruction_safely)
{
    using namespace geoml;
    using namespace geoml::metadata;

    Shape left{BRepBuilderAPI_MakeVertex(gp_Pnt{1., 0., 0.}).Shape()};
    Shape right{BRepBuilderAPI_MakeVertex(gp_Pnt{2., 0., 0.}).Shape()};
    MetadataCopyOperation operation({left, right});

    std::optional<MetadataScope> lingering_scope;
    {
        ModelingContext original;
        original.add_metadata_track(left, is_vertex, keys::name(), "left");
        original.add_metadata_track(right, is_vertex, keys::name(), "right");

        auto scope = original.create_policy_scope(
            MetadataPolicy{}.for_key(keys::name(), prefer_input(0)));
        ModelingContext moved = std::move(original);
        lingering_scope.emplace(std::move(scope));
        EXPECT_EQ(operation.value(moved).metadata().at(keys::name()), "left");
    }
    // Destroying the guard after its ModelingContext is already gone must be a safe no-op.
    lingering_scope.reset();
}

TEST(ModelingContext, shared_subshape_history_and_metadata_propagation_across_adjacent_faces)
{
    using namespace geoml;
    using namespace geoml::metadata;

    Shape box{BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape()};
    Shape cutter1{BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape()};
    Shape cutter2{BRepPrimAPI_MakeBox(gp_Pnt(1.5, 1.5, 1.5), 1.0, 1.0, 1.0).Shape()};

    // Probe step 1 without tracking to pick an edge shortened by cutting [0,1]^3.
    Shape probe = boolean_subtract(box, cutter1);
    Shape orig_edge;
    for (auto const& candidate : box.select_subshapes(is_edge)) {
        if (probe.select_subshapes(is_edge && is_modified_descendent_of(candidate)).size() == 1u) {
            orig_edge = candidate;
            break;
        }
    }
    ASSERT_FALSE(orig_edge.is_null());

    // Obtain handles to orig_edge through both of its adjacent faces on the input box.
    Shape box_faces = box.select_subshapes(is_face && has_subshape(orig_edge));
    ASSERT_EQ(box_faces.size(), 2u);
    Shape orig_edge_via_face1 =
        box_faces[0].select_subshapes(is_edge && is_same(orig_edge)).unique_element();
    Shape orig_edge_via_face2 =
        box_faces[1].select_subshapes(is_edge && is_same(orig_edge)).unique_element();
    ASSERT_TRUE(orig_edge_via_face1.is_same(orig_edge_via_face2));

    // Register typed metadata and tag tracks via face 1's handle using history predicates.
    ModelingContext context;
    context.add_metadata_track(
        orig_edge_via_face1,
        is_edge && has_origin && is_modified_descendent_of(orig_edge_via_face1),
        keys::name(),
        std::string{"shortened-edge"},
        TrackLifetime::steps(2));
    context.add_tag_track(
        orig_edge_via_face1,
        is_edge && has_origin && is_modified_descendent_of(orig_edge_via_face1),
        "modified-step1",
        TrackLifetime::steps(2));

    // Step 1: subtract [0,1]^3 -> orig_edge is modified into a shorter shared edge.
    Shape step1 = boolean_subtract(box, cutter1, context);
    Shape step1_edge =
        step1.select_subshapes(is_edge && is_modified_descendent_of(orig_edge)).unique_element();
    Shape step1_faces = step1.select_subshapes(is_face && has_subshape(step1_edge));
    ASSERT_EQ(step1_faces.size(), 2u);

    Shape step1_e1 = step1_faces[0].select_subshapes(is_edge && is_same(step1_edge)).unique_element();
    Shape step1_e2 = step1_faces[1].select_subshapes(is_edge && is_same(step1_edge)).unique_element();
    ASSERT_TRUE(step1_e1.is_same(step1_e2));

    for (auto const* edge_handle : {&step1_e1, &step1_e2}) {
        EXPECT_TRUE(edge_handle->has_origin());
        EXPECT_TRUE(edge_handle->is_modified_descendent_of(orig_edge_via_face1));
        EXPECT_TRUE(edge_handle->is_modified_descendent_of(orig_edge_via_face2));
        EXPECT_EQ(edge_handle->metadata().at(keys::name()), "shortened-edge");
        EXPECT_TRUE(edge_handle->has_tag("modified-step1"));
    }
    auto const& step1_tags = step1_e1.metadata().at(keys::tags());
    EXPECT_EQ(std::count(step1_tags.begin(), step1_tags.end(), "modified-step1"), 1);

    // Register a second track via face 2's handle matching the unmodified descendant in step 2.
    context.add_metadata_track(
        step1_e2,
        is_edge && has_origin && is_unmodified_descendent_of(step1_e2),
        keys::color(),
        Color{255, 128, 0},
        TrackLifetime::steps(1));
    context.add_tag_track(
        step1_e2,
        is_edge && has_origin && is_unmodified_descendent_of(step1_e2),
        "survived-step2",
        TrackLifetime::steps(1));

    // Step 2: subtract [1.5,2.5]^3 at the opposite corner -> step1_edge survives unmodified.
    Shape step2 = boolean_subtract(step1, cutter2, context);
    Shape step2_edge = step2.select_subshapes(is_edge && is_same(step1_edge)).unique_element();
    Shape step2_faces = step2.select_subshapes(is_face && has_subshape(step2_edge));
    ASSERT_EQ(step2_faces.size(), 2u);

    Shape step2_e1 = step2_faces[0].select_subshapes(is_edge && is_same(step2_edge)).unique_element();
    Shape step2_e2 = step2_faces[1].select_subshapes(is_edge && is_same(step2_edge)).unique_element();
    ASSERT_TRUE(step2_e1.is_same(step2_e2));

    for (auto const* edge_handle : {&step2_e1, &step2_e2}) {
        EXPECT_TRUE(edge_handle->has_origin());
        EXPECT_TRUE(edge_handle->is_unmodified_descendent_of(step1_e1));
        EXPECT_TRUE(edge_handle->is_unmodified_descendent_of(step1_e2));
        EXPECT_TRUE(edge_handle->is_descendent_of(orig_edge_via_face1));
        EXPECT_TRUE(edge_handle->is_modified_descendent_of(orig_edge_via_face1));
        EXPECT_EQ(edge_handle->metadata().at(keys::name()), "shortened-edge");
        EXPECT_EQ(edge_handle->metadata().at(keys::color()), (Color{255, 128, 0}));
        EXPECT_TRUE(edge_handle->has_tag("modified-step1"));
        EXPECT_TRUE(edge_handle->has_tag("survived-step2"));
    }
    auto const& step2_tags = step2_e1.metadata().at(keys::tags());
    EXPECT_EQ(std::count(step2_tags.begin(), step2_tags.end(), "modified-step1"), 1);
    EXPECT_EQ(std::count(step2_tags.begin(), step2_tags.end(), "survived-step2"), 1);
}
