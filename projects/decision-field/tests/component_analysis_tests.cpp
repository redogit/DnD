#include "decision_field/component_analysis.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace df;
using namespace df::component;

#define CHECK(x) do { if (!(x)) throw std::runtime_error(std::string("CHECK failed: ") + #x); } while(false)

static bool approx(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) <= eps;
}

static void test_basis_covers_directional_relational_and_parametric_views() {
    const auto basis = make_directional_basis(
        3,
        std::vector<double>{0.0, 0.0, 1.0},
        std::vector<double>{1.0, 1.0, 0.0},
        {ParametricAxis{"load", {0.0, 1.0, 1.0}}});

    auto has = [&](std::string_view id) {
        for (const auto& b : basis) if (b.id == id) return true;
        return false;
    };

    CHECK(has("right"));
    CHECK(has("left"));
    CHECK(has("up"));
    CHECK(has("down"));
    CHECK(has("diagonal:up-right"));
    CHECK(has("diagonal:down-left"));
    CHECK(has("lateral:+"));
    CHECK(has("lateral:-"));
    CHECK(has("orthogonal:+"));
    CHECK(has("orthogonal:-"));
    CHECK(has("param:load:+"));
    CHECK(has("param:load:-"));
}

static void test_analysis_joins_only_stable_primary_structure_into_simpler_core() {
    ComponentFrame frame;
    frame.core = {1.0, 0.0, 0.0};
    frame.observations = {
        ComponentObservation{1, {1.0,  4.0,  0.0}, 1.0, 1.0, 1.0, 0.1, 0.1, 0.0},
        ComponentObservation{2, {1.0, -4.0,  0.0}, 1.0, 1.0, 1.0, 0.1, 0.1, 0.0},
        ComponentObservation{3, {1.0,  0.0,  3.0}, 0.9, 1.0, 1.0, 0.1, 0.1, 0.0},
        ComponentObservation{4, {1.0,  0.0, -3.0}, 0.9, 1.0, 1.0, 0.1, 0.1, 0.0}
    };

    const auto basis = make_directional_basis(
        3,
        std::vector<double>{0.0, 0.0, 1.0},
        std::vector<double>{1.0, 0.0, 0.0},
        {});

    AnalysisConfig config;
    config.max_primary_components = 4;
    config.max_recursive_depth = 2;
    config.minimum_members_for_recursion = 2;
    config.core_tolerance = 1e-9;

    const auto result = analyze(frame, basis, config);

    CHECK(!result.primary_components.empty());
    CHECK(result.simpler_core.stable_parameters.size() == 3);
    CHECK(result.simpler_core.stable_parameters[0].has_value());
    CHECK(approx(*result.simpler_core.stable_parameters[0], 1.0));
    CHECK(!result.simpler_core.stable_parameters[1].has_value());
    CHECK(!result.simpler_core.stable_parameters[2].has_value());
    CHECK(result.simpler_core.residual_dimensions.size() == 2);
}

static void test_component_of_component_analysis_preserves_substructure() {
    ComponentFrame frame;
    frame.core = {0.0, 0.0, 0.0};
    frame.observations = {
        ComponentObservation{10, { 5.0,  1.0, 0.0}, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        ComponentObservation{11, { 5.0, -1.0, 0.0}, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        ComponentObservation{12, {-5.0,  1.0, 0.0}, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0},
        ComponentObservation{13, {-5.0, -1.0, 0.0}, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0}
    };

    const auto basis = make_directional_basis(
        3,
        std::vector<double>{0.0, 0.0, 1.0},
        std::vector<double>{0.0, 1.0, 0.0},
        {});

    AnalysisConfig config;
    config.max_primary_components = 4;
    config.max_recursive_depth = 2;
    config.minimum_members_for_recursion = 2;

    const auto result = analyze(frame, basis, config);

    bool found_child = false;
    for (const auto& component : result.primary_components) {
        if (!component.children.empty()) {
            found_child = true;
            for (const auto& child : component.children) {
                CHECK(child.depth == component.depth + 1);
                CHECK(!child.member_observation_indices.empty());
            }
        }
    }
    CHECK(found_child);
}

int main() {
    test_basis_covers_directional_relational_and_parametric_views();
    test_analysis_joins_only_stable_primary_structure_into_simpler_core();
    test_component_of_component_analysis_preserves_substructure();
    std::cout << "all component-analysis tests passed\n";
}
