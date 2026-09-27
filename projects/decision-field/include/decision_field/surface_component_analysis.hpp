#pragma once
#include "decision_field/core_replay.hpp"
#include <unordered_map>

namespace df::bridge {
struct SurfaceCriteria {
    double balance_tolerance{}, minimum_radial_departure{};
    std::size_t max_observations{100000}; // Resource guard, not a semantic depth.
};
struct SurfaceObject {
    std::string object_id, source_surface_id;
    ObservedFrame observed;
    SourceReference left_pole,right_pole;
    SurfaceCriteria criteria;
    std::size_t discovered_wave{},discovered_depth{};
    std::vector<StateKey> representatives;
    std::vector<std::string> definitions;
    std::array<std::size_t,16> qualifying_counts{};
    std::vector<Verdict> validation;
    double mean_polar_imbalance{}, mean_radial_departure{}, min_radial_departure{},max_radial_departure{};
};
struct NextRegion {
    std::string predecessor_surface,projection_id,metric_policy_id;
    double minimum_radial_departure{};
};

// Capture a v0.3 triad result as an owned, scoped object. All explored waves remain
// present. The 16 representatives index the observations; they never replace them.
template<class Engine,class Exploration,class Projection,class Assessment,class Verifier>
SurfaceObject capture_surface(const Engine& engine,const Exploration& search,RunContext context,
                              Projection projection,Assessment assessment,Verifier verifier,SurfaceCriteria criteria) {
    detail::validate_context(context);
    detail::require(search.surface.has_value(),"no discovered surface to capture");
    const auto& s=*search.surface;
    detail::require(criteria.max_observations>0 && search.observations.size()<=criteria.max_observations,
                    "surface snapshot resource limit exceeded");
    detail::require(std::isfinite(criteria.balance_tolerance) && criteria.balance_tolerance>=0 &&
                    std::isfinite(criteria.minimum_radial_departure) && criteria.minimum_radial_departure>=0,
                    "invalid surface criteria");
    detail::require(!s.id.empty() && s.spans_all_directions && s.discovered_wave>0 &&
                    s.discovered_wave==search.completed_waves &&
                    search.directional_executions==search.observations.size(),"incomplete surface exploration");
    std::size_t expected=baseline_count,total=0;
    for(std::size_t w=0;w<search.completed_waves;++w) {
        detail::require(expected<=criteria.max_observations-total,"surface wave exceeds resource guard");
        total+=expected;
        if(w+1<search.completed_waves) {
            detail::require(expected<=criteria.max_observations/outward_count,"surface expansion exceeds resource guard");
            expected*=outward_count;
        }
    }
    detail::require(total==search.observations.size(),"partial directional wave cannot define a complete surface");
    SurfaceObject out;
    out.object_id=context.run_id+"::"+s.id; out.source_surface_id=s.id;
    out.criteria=criteria; out.observed.context=std::move(context);
    out.left_pole=detail::source(engine.state(search.left_pole));
    out.right_pole=detail::source(engine.state(search.right_pole));
    const auto& middle=engine.state(search.midpoint);
    out.observed.core=detail::source(middle);
    out.observed.frame.core=detail::project(middle,projection);
    out.discovered_wave=s.discovered_wave; out.discovered_depth=s.discovered_depth;
    out.definitions=s.definitions;
    std::vector<std::array<std::size_t,16>> counts(search.completed_waves);
    std::unordered_map<StateKey,std::size_t> index;
    std::vector<bool> qualifying;
    for(const auto& record:search.observations) {
        const auto sector=static_cast<std::size_t>(record.root_direction.sector);
        const auto subdivision=static_cast<std::size_t>(record.root_direction.subdivision);
        detail::require(sector<4 && subdivision<4 && record.wave>0 && record.wave<=search.completed_waves,
                        "invalid surface record address");
        const std::size_t d=4*sector+subdivision;
        const auto& node=engine.state(record.state_id);
        auto ref=detail::source(node);
        detail::require(node.probe.has_value(),"surface record lacks an outward probe address");
        detail::require(index.emplace(node.id,index.size()).second,"duplicate surface observation");
        const auto a=assessment(record.evaluation),b=assessment(record.evaluation);
        detail::require(a==b,"surface assessment is not replay-stable");
        out.observed.frame.observations.push_back(detail::observation(node.id,detail::project(node,projection),a));
        const auto& e=record.evaluation;
        std::vector<double> raw{e.left_affinity,e.right_affinity,e.midpoint_affinity,e.polar_imbalance,e.radial_departure,e.surface_strength};
        detail::require(detail::finite(raw),"non-finite triadic metrics");
        out.observed.sources.push_back({std::move(ref),d,static_cast<std::size_t>(node.probe->outward_index),record.wave,std::move(raw)});
        const auto v=verifier(node),vr=verifier(node);
        detail::require(v==vr,"surface verifier is not replay-stable");
        detail::require(v==Verdict::Pass || v==Verdict::Fail || v==Verdict::Unknown,"invalid surface verdict");
        out.validation.push_back(v);
        const bool qualifies=record.wave==s.discovered_wave && v==Verdict::Pass &&
            e.polar_imbalance<=criteria.balance_tolerance && e.radial_departure>=criteria.minimum_radial_departure;
        qualifying.push_back(qualifies);
        if(qualifies) ++out.qualifying_counts[d];
        ++counts[record.wave-1][d];
    }
    expected=outward_count;
    for(std::size_t w=0;w<counts.size();++w) {
        for(auto n:counts[w]) detail::require(n==expected,"incomplete root-direction coverage in surface wave");
        if(w+1<counts.size()) expected*=outward_count;
    }
    detail::require(s.representative_states.size()==direction_count,"surface requires 16 distinct representatives");
    double lo=std::numeric_limits<double>::infinity(),hi=-lo,radial=0,balance=0;
    std::size_t max_depth=0;
    std::unordered_set<StateKey> reps;
    for(std::size_t d=0;d<direction_count;++d) {
        detail::require(s.direction_member_counts[d]==out.qualifying_counts[d] && out.qualifying_counts[d]>0,
                        "surface qualifying counts do not match observations and criteria");
        const auto id=s.representative_states[d]; const auto it=index.find(id);
        detail::require(it!=index.end() && reps.insert(id).second,"missing or duplicate surface representative");
        const auto i=it->second; const auto& p=out.observed.sources[i];
        detail::require(p.direction==d && qualifying[i],"representative is not a qualifying member of its direction");
        out.representatives.push_back(id);
        const auto& e=search.observations[i].evaluation;
        lo=std::min(lo,e.radial_departure);hi=std::max(hi,e.radial_departure);
        radial+=e.radial_departure;balance+=e.polar_imbalance;max_depth=std::max(max_depth,p.source.depth);
    }
    for(std::size_t q=0;q<4;++q) {
        std::size_t n=0;for(std::size_t j=0;j<4;++j)n+=out.qualifying_counts[q*4+j];
        detail::require(s.sector_member_counts[q]==n,"surface sector count mismatch");
    }
    auto close=[](double a,double b){return std::isfinite(a)&&std::isfinite(b)&&std::abs(a-b)<=1e-12;};
    detail::require(close(s.min_radial_departure,lo)&&close(s.max_radial_departure,hi)&&
                    close(s.mean_radial_departure,radial/16)&&close(s.mean_polar_imbalance,balance/16)&&
                    s.discovered_depth==max_depth,"surface statistics do not match retained representatives");
    out.min_radial_departure=lo;out.max_radial_departure=hi;
    out.mean_radial_departure=radial/16;out.mean_polar_imbalance=balance/16;
    detail::validate_frame(out.observed);
    return out;
}

inline component::AnalysisResult analyze_surface(const SurfaceObject& object,
                                                std::vector<double> lateral,std::vector<double> normal,
                                                std::vector<component::ParametricAxis> parameters,
                                                const component::AnalysisConfig& config={}) {
    detail::validate_frame(object.observed);
    detail::require(!object.object_id.empty() && object.representatives.size()==16,"invalid surface object");
    auto unit=[](std::vector<double> v) {
        detail::require(!v.empty()&&detail::finite(v),"non-finite surface axis");
        double scale=0;for(double x:v)scale=std::max(scale,std::abs(x));
        detail::require(scale>0,"zero surface axis");
        double sum=0;for(double& x:v){x/=scale;sum+=x*x;}
        const double n=std::sqrt(sum);for(double& x:v)x/=n;return v;
    };
    lateral=unit(std::move(lateral));normal=unit(std::move(normal));
    detail::require(lateral.size()==object.observed.frame.core.size()&&normal.size()==lateral.size(),"surface basis dimension mismatch");
    double dot=0;for(std::size_t i=0;i<normal.size();++i)dot+=lateral[i]*normal[i];
    detail::require(std::abs(dot)<=1e-9,"declared surface tangent and normal are not orthogonal in this numeric metric");
    const auto dimensions=lateral.size();
    auto basis=component::make_directional_basis(dimensions,std::move(lateral),std::move(normal),std::move(parameters));
    return component::analyze(object.observed.frame,basis,config);
}

inline NextRegion next_region(const SurfaceObject& object) {
    detail::require(!object.object_id.empty()&&std::isfinite(object.max_radial_departure),"invalid predecessor surface");
    const double lower=std::nextafter(object.max_radial_departure,std::numeric_limits<double>::infinity());
    detail::require(std::isfinite(lower),"no finite next-region lower bound");
    return {object.object_id,object.observed.context.projection_id,object.observed.context.metric_policy_id,lower};
}
} // namespace df::bridge
