#include "decision_field/decision_field.hpp"
#include "decision_field/self_architecture_domain.hpp"
#include "decision_field/surface_component_analysis.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;
using namespace df::self_architecture;
using namespace df::bridge;
#define CHECK(x) do { if(!(x)) throw std::runtime_error(#x); } while(false)
template<class F> void rejects(F f) {bool caught=false;try {f();}catch(const std::invalid_argument&){caught=true;}CHECK(caught);}
int main() {
 auto policy=make_policy(); const auto obs=obligations(); DecisionFieldEngine<SelfState> engine(policy,obs);
 auto a=architecture_self(), b=policy.mirror(a,lawful_axes(10),obs); auto m=policy.overlap(a,b,obs);
 auto l=engine.add_root(a), r=engine.add_root(b), c=engine.add_root(*m.shared);
 const auto first=engine.explore_triad_surface(l,r,c,3,0.02,0.05);
 RunContext context{"surface-run",obs.id,"v03","stance-only/v1","surface-fixture/v1","lawful/10"};
 auto projection=[](const auto& n){std::vector<double> v;for(auto x:n.value->stance)v.push_back(double(x));return v;};
 auto assessment=[](const auto& e){return ObservationAssessment{0,e.radial_departure,1,0,0,0};};
 auto verifier=[&](const auto& n){return policy.validate(*n.value,obs).status==ExecutionStatus::Succeeded?Verdict::Pass:Verdict::Fail;};
 const auto first_object=capture_surface(engine,first,context,projection,assessment,verifier,SurfaceCriteria{0.02,0.05,100000});
 CHECK(first_object.observed.frame.observations.size()==192);
 CHECK(first_object.representatives.size()==16);
 CHECK(first_object.definitions==first.surface->definitions);
 auto next=next_region(first_object);
 CHECK(next.predecessor_surface==first_object.object_id);
 CHECK(next.minimum_radial_departure>first.surface->max_radial_departure);
 const auto farther=engine.explore_triad_surface(l,r,c,4,0.02,next.minimum_radial_departure);
 auto farther_object=capture_surface(engine,farther,context,projection,assessment,verifier,SurfaceCriteria{0.02,next.minimum_radial_departure,100000});
 CHECK(farther_object.observed.frame.observations.size()==farther.directional_executions);
 CHECK(farther_object.observed.frame.observations.size()==30144);
 CHECK(farther_object.discovered_wave==3);
 // All source waves, not only 16 selected representatives, remain attached.
 CHECK(farther_object.observed.sources.front().wave==1);
 CHECK(farther_object.observed.sources.back().wave==3);
 std::vector<double> tangent(CommitmentCount),normal(CommitmentCount);tangent[6]=1;normal[7]=1;
 auto analysis=analyze_surface(farther_object,tangent,normal,{});
 CHECK(analysis.simpler_core.residual_dimensions.size()==10);
 rejects([&]{(void)analyze_surface(farther_object,tangent,tangent,{});});
 auto corrupted=first; corrupted.observations.pop_back();
 rejects([&]{(void)capture_surface(engine,corrupted,context,projection,assessment,verifier,SurfaceCriteria{0.02,0.05,100000});});
 corrupted=first; corrupted.surface->representative_states[1]=corrupted.surface->representative_states[0];
 rejects([&]{(void)capture_surface(engine,corrupted,context,projection,assessment,verifier,SurfaceCriteria{0.02,0.05,100000});});
 corrupted=first; corrupted.surface->direction_member_counts[0]++;
 rejects([&]{(void)capture_surface(engine,corrupted,context,projection,assessment,verifier,SurfaceCriteria{0.02,0.05,100000});});
 std::cout<<"surface objects: nearest=192 records, farther=30144 records, 16 representatives; full-wave checks passed\n";
}
