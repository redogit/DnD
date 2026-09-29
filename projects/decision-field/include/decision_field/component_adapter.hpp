#pragma once
#include "decision_field/component_analysis.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <string>
#include <unordered_set>

namespace df::bridge {
using component::ComponentFrame;
using component::ComponentObservation;
using StateKey = component::ComponentStateId;
inline constexpr std::size_t direction_count=16, outward_count=12, baseline_count=192;

// These labels are required inputs, not automatically inferred authority.
struct RunContext {
    std::string run_id, obligation_id, obligation_version, projection_id, metric_policy_id, axis_version;
    friend bool operator==(const RunContext&, const RunContext&) = default;
};
struct ObservationAssessment {
    double obligation_progress{}, information_gain{}, evidence_strength{}, residual_cost{}, recovery_cost{}, decay_risk{};
    friend bool operator==(const ObservationAssessment&, const ObservationAssessment&) = default;
};
struct SourceReference {
    StateKey state_id{};
    std::vector<StateKey> parents;
    std::string fingerprint;
    std::size_t depth{};
    std::vector<std::string> provenance;
};
struct ProbeSource {
    SourceReference source;
    std::size_t direction{}, outward{}, wave{1};
    // Baseline order: progress, information, counterexample, uncertainty, convergence,
    // transferability, complexity, execution, recovery, decay. Never discarded by scoring.
    std::vector<double> raw_metrics;
};
struct ObservedFrame {
    RunContext context;
    ComponentFrame frame;
    SourceReference core;
    std::vector<SourceReference> first_ring;
    std::vector<ProbeSource> sources;
};
namespace detail {
inline void require(bool yes, const char* reason) {
    if (!yes) throw std::invalid_argument(reason);
}
inline void validate_context(const RunContext& c) {
    require(!c.run_id.empty() && !c.obligation_id.empty() && !c.obligation_version.empty() &&
        !c.projection_id.empty() && !c.metric_policy_id.empty() && !c.axis_version.empty(),
        "run, obligation, projection, metric and axis identities are required");
}
inline bool finite(std::span<const double> values) {
    return std::all_of(values.begin(),values.end(),[](double x){return std::isfinite(x);});
}
inline bool same_bits(std::span<const double> a, std::span<const double> b) {
    if(a.size()!=b.size()) return false;
    for(std::size_t i=0;i<a.size();++i)
        if(std::bit_cast<std::uint64_t>(a[i])!=std::bit_cast<std::uint64_t>(b[i])) return false;
    return true;
}
template<class Node> SourceReference source(const Node& n) {
    require(n.id!=0 && n.value && !n.stale && !n.fingerprint.empty(), "missing or stale source state");
    return {n.id,n.parent_states,n.fingerprint,n.depth,n.provenance};
}
inline void require_parent(const SourceReference& source_ref, StateKey expected) {
    require(std::find(source_ref.parents.begin(),source_ref.parents.end(),expected)!=source_ref.parents.end(),
            "state lineage does not name its declared parent");
}
template<class Node,class Projection> std::vector<double> project(const Node& node,Projection& projection) {
    auto a=projection(node);
    auto b=projection(node);
    require(!a.empty() && finite(a) && finite(b) && same_bits(a,b), "projection is empty, non-finite or not replay-stable");
    return a;
}
inline ComponentObservation observation(StateKey id,std::vector<double> values,const ObservationAssessment& a) {
    const double fields[]{a.obligation_progress,a.information_gain,a.evidence_strength,a.residual_cost,a.recovery_cost,a.decay_risk};
    require(finite(fields),"non-finite assessment");
    return {id,std::move(values),a.obligation_progress,a.information_gain,a.evidence_strength,a.residual_cost,a.recovery_cost,a.decay_risk};
}
inline void validate_frame(const ObservedFrame& f) {
    validate_context(f.context);
    require(f.core.state_id!=0 && !f.core.fingerprint.empty(),"missing core identity");
    require(!f.frame.core.empty() && finite(f.frame.core),"invalid core coordinates");
    require(f.frame.observations.size()==f.sources.size(),"source/observation count mismatch");
    std::unordered_set<StateKey> ids;
    for(std::size_t i=0;i<f.sources.size();++i) {
        const auto& o=f.frame.observations[i]; const auto& s=f.sources[i];
        require(o.state_id!=0 && o.state_id==s.source.state_id && ids.insert(o.state_id).second,
                "missing, duplicated or mismatched observation identity");
        require(o.values.size()==f.frame.core.size() && finite(o.values),"invalid observation coordinates");
        require(!s.source.fingerprint.empty() && s.direction<direction_count && s.outward<outward_count && s.wave>0,
                "invalid observation provenance or address");
        require(finite(s.raw_metrics),"non-finite source metrics");
        (void)observation(o.state_id,o.values,{o.obligation_progress,o.information_gain,o.evidence_strength,
                                             o.residual_cost,o.recovery_cost,o.decay_risk});
    }
}
} // namespace detail

// Structural templates accept the actual v0.3 engine and field without coupling this
// standalone module to its source tree. Assessment is explicit: the engine does not
// provide an evidence-strength oracle, so this adapter must not invent one.
template<class Engine,class Field,class Projection,class Assessment>
ObservedFrame make_baseline_frame(const Engine& engine,const Field& field,RunContext context,
                                 Projection projection,Assessment assessment) {
    detail::validate_context(context);
    detail::require(field.baseline_executions==baseline_count && field.first_ring.size()==direction_count &&
                    field.outward.size()==direction_count,"incomplete 16 x 12 baseline");
    ObservedFrame out;
    out.context=std::move(context);
    const auto& core_node=engine.state(field.core);
    out.core=detail::source(core_node);
    out.frame.core=detail::project(core_node,projection);
    std::unordered_set<StateKey> ids{field.core};
    for(std::size_t d=0;d<direction_count;++d) {
        const auto& parent=engine.state(field.first_ring[d]);
        auto parent_ref=detail::source(parent);
        detail::require(ids.insert(parent.id).second,"duplicate first-ring identity");
        detail::require_parent(parent_ref,field.core);
        out.first_ring.push_back(parent_ref);
        detail::require(field.outward[d].size()==outward_count,"incomplete outward fan");
        for(std::size_t j=0;j<outward_count;++j) {
            const auto& p=field.outward[d][j];
            detail::require(static_cast<std::size_t>(p.address.parent.sector)==d/4 &&
                            static_cast<std::size_t>(p.address.parent.subdivision)==d%4 &&
                            static_cast<std::size_t>(p.address.outward_index)==j,"misaddressed baseline probe");
            const auto& node=engine.state(p.state_id);
            auto ref=detail::source(node);
            detail::require(ids.insert(node.id).second,"duplicate probe identity");
            detail::require_parent(ref,parent.id);
            auto values=detail::project(node,projection);
            const auto a=assessment(p.evaluation), b=assessment(p.evaluation);
            detail::require(a==b,"assessment is not replay-stable");
            out.frame.observations.push_back(detail::observation(node.id,std::move(values),a));
            const auto& e=p.evaluation;
            out.sources.push_back({std::move(ref),d,j,1,{e.obligation_progress,e.information_gain,
                e.counterexample_value,e.uncertainty_reduction,e.convergence_potential,e.transferability,
                e.complexity_cost,e.execution_cost,e.recovery_cost,e.decay_risk}});
        }
    }
    detail::validate_frame(out);
    return out;
}
} // namespace df::bridge
