#pragma once

#include <cstdint>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace df::component {

using ComponentStateId = std::uint64_t;

enum class AxisKind {
    Horizontal,
    Vertical,
    Diagonal,
    Lateral,
    Orthogonal,
    Parametric
};

struct ParametricAxis {
    std::string id;
    std::vector<double> direction;
};

struct BasisDirection {
    std::string id;
    AxisKind kind{AxisKind::Parametric};
    std::vector<double> unit_direction;
};

struct ComponentObservation {
    ComponentStateId state_id{};
    std::vector<double> values;
    double obligation_progress{};
    double information_gain{};
    double evidence_strength{};
    double residual_cost{};
    double recovery_cost{};
    double decay_risk{};
};

struct ComponentFrame {
    std::vector<double> core;
    std::vector<ComponentObservation> observations;
};

struct AnalysisConfig {
    std::size_t max_primary_components{8};
    std::size_t max_recursive_depth{2};
    std::size_t minimum_members_for_recursion{2};
    double core_tolerance{1e-9};
};

struct ComponentNode {
    std::string basis_id;
    AxisKind axis_kind{AxisKind::Parametric};
    std::size_t depth{};
    double aggregate_score{};
    std::vector<std::size_t> member_observation_indices;
    std::size_t representative_observation_index{};
    std::vector<ComponentNode> children;
};

struct CoreResidualDimension {
    std::size_t dimension{};
    double minimum{};
    double maximum{};
};

struct SimplerCore {
    std::vector<std::optional<double>> stable_parameters;
    std::vector<CoreResidualDimension> residual_dimensions;
};

struct AnalysisResult {
    std::vector<ComponentNode> primary_components;
    SimplerCore simpler_core;
};

namespace detail {

inline double norm(std::span<const double> values) {
    double sum = 0.0;
    for (const double v : values) sum += v * v;
    return std::sqrt(sum);
}

inline std::vector<double> normalized(std::vector<double> values) {
    const double n = norm(values);
    if (n <= std::numeric_limits<double>::epsilon()) {
        throw std::invalid_argument("component basis direction must be non-zero");
    }
    for (double& v : values) v /= n;
    return values;
}

inline double dot_delta(const ComponentFrame& frame,
                        const ComponentObservation& observation,
                        const BasisDirection& basis) {
    if (observation.values.size() != frame.core.size() || basis.unit_direction.size() != frame.core.size()) {
        throw std::invalid_argument("component dimensions do not match frame core");
    }
    double dot = 0.0;
    for (std::size_t i = 0; i < frame.core.size(); ++i) {
        dot += (observation.values[i] - frame.core[i]) * basis.unit_direction[i];
    }
    return dot;
}

inline double observation_quality(const ComponentObservation& o) {
    return std::max(0.0,
        o.obligation_progress + o.information_gain + o.evidence_strength
        - o.residual_cost - o.recovery_cost - o.decay_risk);
}

inline double score(const ComponentFrame& frame,
                    const ComponentObservation& observation,
                    const BasisDirection& basis) {
    const double projection = dot_delta(frame, observation, basis);
    if (projection <= 0.0) return 0.0;
    return projection * observation_quality(observation);
}

inline bool contains_basis(std::span<const std::string> excluded, std::string_view id) {
    return std::find(excluded.begin(), excluded.end(), id) != excluded.end();
}

inline std::vector<ComponentNode> build_level(
    const ComponentFrame& frame,
    std::span<const BasisDirection> basis,
    const AnalysisConfig& config,
    std::span<const std::size_t> observation_indices,
    std::size_t depth,
    std::vector<std::string> excluded_basis) {

    struct Bucket {
        double aggregate{};
        double representative_score{-1.0};
        std::size_t representative{};
        std::vector<std::size_t> members;
    };

    std::vector<Bucket> buckets(basis.size());

    for (const std::size_t observation_index : observation_indices) {
        if (observation_index >= frame.observations.size()) {
            throw std::out_of_range("component observation index out of range");
        }
        const auto& observation = frame.observations[observation_index];
        std::size_t best_basis = basis.size();
        double best_score = 0.0;
        for (std::size_t i = 0; i < basis.size(); ++i) {
            if (contains_basis(excluded_basis, basis[i].id)) continue;
            const double candidate = score(frame, observation, basis[i]);
            if (candidate > best_score) {
                best_score = candidate;
                best_basis = i;
            }
        }
        if (best_basis == basis.size()) continue;
        auto& bucket = buckets[best_basis];
        bucket.aggregate += best_score;
        bucket.members.push_back(observation_index);
        if (best_score > bucket.representative_score) {
            bucket.representative_score = best_score;
            bucket.representative = observation_index;
        }
    }

    std::vector<ComponentNode> nodes;
    for (std::size_t i = 0; i < basis.size(); ++i) {
        if (buckets[i].members.empty()) continue;
        ComponentNode node;
        node.basis_id = basis[i].id;
        node.axis_kind = basis[i].kind;
        node.depth = depth;
        node.aggregate_score = buckets[i].aggregate;
        node.member_observation_indices = buckets[i].members;
        node.representative_observation_index = buckets[i].representative;

        if (depth < config.max_recursive_depth &&
            node.member_observation_indices.size() >= config.minimum_members_for_recursion) {
            auto child_excluded = excluded_basis;
            child_excluded.push_back(node.basis_id);
            node.children = build_level(frame, basis, config,
                                        node.member_observation_indices,
                                        depth + 1,
                                        std::move(child_excluded));
        }
        nodes.push_back(std::move(node));
    }

    std::sort(nodes.begin(), nodes.end(), [](const ComponentNode& a, const ComponentNode& b) {
        if (a.aggregate_score != b.aggregate_score) return a.aggregate_score > b.aggregate_score;
        return a.basis_id < b.basis_id;
    });
    if (nodes.size() > config.max_primary_components) nodes.resize(config.max_primary_components);
    return nodes;
}

inline SimplerCore join_retained_members(const ComponentFrame& frame,
                                         std::span<const ComponentNode> components,
                                         double tolerance) {
    SimplerCore core;
    core.stable_parameters.resize(frame.core.size());
    if (components.empty()) return core;

    std::vector<bool> retained(frame.observations.size(), false);
    std::size_t retained_count = 0;
    for (const auto& component : components) {
        for (const auto index : component.member_observation_indices) {
            if (index >= frame.observations.size()) throw std::out_of_range("component member index out of range");
            if (!retained[index]) {
                retained[index] = true;
                ++retained_count;
            }
        }
    }
    if (retained_count == 0) return core;

    for (std::size_t dimension = 0; dimension < frame.core.size(); ++dimension) {
        double minimum = std::numeric_limits<double>::infinity();
        double maximum = -std::numeric_limits<double>::infinity();
        std::optional<double> anchor;
        for (std::size_t index = 0; index < frame.observations.size(); ++index) {
            if (!retained[index]) continue;
            const auto& values = frame.observations[index].values;
            if (values.size() != frame.core.size()) throw std::invalid_argument("observation dimensionality mismatch");
            const double value = values[dimension];
            minimum = std::min(minimum, value);
            maximum = std::max(maximum, value);
            if (!anchor) anchor = value;
        }
        if (maximum - minimum <= tolerance) {
            // An observed anchor avoids overflow and inventing a new value.
            core.stable_parameters[dimension] = anchor;
        } else {
            core.residual_dimensions.push_back({dimension, minimum, maximum});
        }
    }
    return core;
}

inline void add_basis(std::vector<BasisDirection>& out,
                      std::string id,
                      AxisKind kind,
                      std::vector<double> direction) {
    out.push_back({std::move(id), kind, normalized(std::move(direction))});
}

} // namespace detail

inline std::vector<BasisDirection> make_directional_basis(
    std::size_t dimensions,
    std::vector<double> lateral,
    std::vector<double> orthogonal,
    std::vector<ParametricAxis> parametric) {

    if (dimensions < 2) throw std::invalid_argument("directional basis requires at least two dimensions");
    if (lateral.size() != dimensions || orthogonal.size() != dimensions) {
        throw std::invalid_argument("lateral/orthogonal basis dimensions must match");
    }

    std::vector<BasisDirection> out;
    auto axis = [dimensions](std::size_t index, double sign) {
        std::vector<double> v(dimensions, 0.0);
        v[index] = sign;
        return v;
    };

    detail::add_basis(out, "right", AxisKind::Horizontal, axis(0, 1.0));
    detail::add_basis(out, "left", AxisKind::Horizontal, axis(0, -1.0));
    detail::add_basis(out, "up", AxisKind::Vertical, axis(1, 1.0));
    detail::add_basis(out, "down", AxisKind::Vertical, axis(1, -1.0));

    auto diagonal = [dimensions](double x, double y) {
        std::vector<double> v(dimensions, 0.0);
        v[0] = x; v[1] = y;
        return v;
    };
    detail::add_basis(out, "diagonal:up-right", AxisKind::Diagonal, diagonal(1.0, 1.0));
    detail::add_basis(out, "diagonal:up-left", AxisKind::Diagonal, diagonal(-1.0, 1.0));
    detail::add_basis(out, "diagonal:down-right", AxisKind::Diagonal, diagonal(1.0, -1.0));
    detail::add_basis(out, "diagonal:down-left", AxisKind::Diagonal, diagonal(-1.0, -1.0));

    detail::add_basis(out, "lateral:+", AxisKind::Lateral, lateral);
    for (double& v : lateral) v = -v;
    detail::add_basis(out, "lateral:-", AxisKind::Lateral, lateral);

    detail::add_basis(out, "orthogonal:+", AxisKind::Orthogonal, orthogonal);
    for (double& v : orthogonal) v = -v;
    detail::add_basis(out, "orthogonal:-", AxisKind::Orthogonal, orthogonal);

    for (auto& parameter : parametric) {
        if (parameter.direction.size() != dimensions) {
            throw std::invalid_argument("parametric axis dimensions must match");
        }
        auto negative = parameter.direction;
        for (double& v : negative) v = -v;
        detail::add_basis(out, "param:" + parameter.id + ":+", AxisKind::Parametric, parameter.direction);
        detail::add_basis(out, "param:" + parameter.id + ":-", AxisKind::Parametric, std::move(negative));
    }
    return out;
}

inline AnalysisResult analyze(const ComponentFrame& frame,
                              std::span<const BasisDirection> basis,
                              const AnalysisConfig& config = {}) {
    if (frame.core.empty()) throw std::invalid_argument("component frame core cannot be empty");
    if (basis.empty()) throw std::invalid_argument("component analysis requires a basis");
    if (config.max_primary_components == 0) throw std::invalid_argument("max_primary_components must be non-zero");

    if (!std::isfinite(config.core_tolerance) || config.core_tolerance < 0.0)
        throw std::invalid_argument("core tolerance must be finite and non-negative");
    if (config.minimum_members_for_recursion == 0)
        throw std::invalid_argument("recursion member threshold must be non-zero");
    const auto finite = [](std::span<const double> xs) {
        return std::all_of(xs.begin(), xs.end(), [](double x) { return std::isfinite(x); });
    };
    if (!finite(frame.core)) throw std::invalid_argument("non-finite core coordinate");
    std::unordered_set<ComponentStateId> states;
    for (const auto& o : frame.observations) {
        const double metrics[]{o.obligation_progress, o.information_gain, o.evidence_strength,
                               o.residual_cost, o.recovery_cost, o.decay_risk};
        if (o.values.size() != frame.core.size() || !finite(o.values) || !finite(metrics))
            throw std::invalid_argument("invalid observation coordinates or metrics");
        if (o.state_id == 0 || !states.insert(o.state_id).second)
            throw std::invalid_argument("observation identities must be nonzero and unique");
    }
    std::unordered_set<std::string> names;
    for (const auto& b : basis) {
        const double n = detail::norm(b.unit_direction);
        if (b.id.empty() || !names.insert(b.id).second ||
            b.unit_direction.size() != frame.core.size() || !finite(b.unit_direction) ||
            !std::isfinite(n) || std::abs(n - 1.0) > 1e-9)
            throw std::invalid_argument("basis must have unique names and finite unit directions");
    }

    std::vector<std::size_t> indices(frame.observations.size());
    for (std::size_t i = 0; i < indices.size(); ++i) indices[i] = i;

    AnalysisResult result;
    result.primary_components = detail::build_level(frame, basis, config, indices, 0, {});
    // Ranking limits execution attention; it never reduces the preservation scope.
    ComponentNode complete_frame;
    complete_frame.member_observation_indices = indices;
    result.simpler_core = detail::join_retained_members(
        frame, std::span<const ComponentNode>(&complete_frame, 1), config.core_tolerance);
    return result;
}

} // namespace df::component
