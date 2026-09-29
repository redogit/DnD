#include "decision_field/decision_field.hpp"
#include "decision_field/self_architecture_domain.hpp"
#include "decision_field/component_adapter.hpp"
#include "decision_field/core_replay.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;
using namespace df::self_architecture;
using namespace df::bridge;
#define CHECK(x) do { if(!(x)) throw std::runtime_error(#x); } while(false)
int main() {
 auto policy=make_policy(); auto obligations_value=obligations();
 DecisionFieldEngine<SelfState> engine(policy,obligations_value);
 auto a=architecture_self(); auto b=policy.mirror(a,lawful_axes(10),obligations_value);
 auto m=policy.overlap(a,b,obligations_value); CHECK(m.shared.has_value());
 auto core=engine.add_root(*m.shared,{"v0.5:actual-v0.3-midpoint"});
 auto field=engine.build_directional_field(core);
 RunContext scope{"actual-v03-run",obligations_value.id,"frozen-v03","stance-only/v1","fixture-assessment/v1","lawful-axes/10"};
 auto projection=[](const auto& node){std::vector<double> v; for(auto x:node.value->stance) v.push_back(double(x)); return v;};
 // Evidence weight is an explicit fixture parameter, not an inferred scientific confidence.
 auto metrics=[](const auto& e){return ObservationAssessment{e.obligation_progress,e.information_gain,1,0,e.recovery_cost,e.decay_risk};};
 auto observed=make_baseline_frame(engine,field,scope,projection,metrics);
 auto basis=component::make_directional_basis(CommitmentCount,std::vector<double>(CommitmentCount,1),[] {std::vector<double> v(CommitmentCount);v[6]=1;return v;}(),{});
 auto analysis=component::analyze(observed.frame,basis);
 auto packet=make_core_packet(observed,0);
 auto check=ObligationOracle{"mandatory-stances","v1",{},[](std::span<const double> v) {
  std::vector<Verdict> answers;for(std::size_t i=0;i<kHardCount;++i) answers.push_back(v[i]==1?Verdict::Pass:Verdict::Fail);return answers;
 }};
 for(std::size_t i=0;i<kHardCount;++i) check.obligation_ids.push_back(kNames[i]);
 auto replay=replay_core(observed,packet,check);
 CHECK(replay.disposition==Disposition::Keep); CHECK(replay.rows_checked==192);
 CHECK(replay.all_obligations_pass); CHECK(packet.summary.residual_dimensions.size()==10);
 auto stable=std::count_if(packet.summary.stable_parameters.begin(),packet.summary.stable_parameters.end(),[](auto x){return x.has_value();});
 CHECK(stable==6);
 std::size_t entries=static_cast<std::size_t>(stable);for(const auto& row:packet.rows) entries+=row.overrides.size();
 std::cout<<"actual v0.3 baseline -> component frame -> independently decoded packet -> obligation replay\n"
          <<"source observations="<<observed.frame.observations.size()<<" stable="<<stable
          <<" residual_dimensions="<<packet.summary.residual_dimensions.size()<<" exact_replayed="<<replay.rows_checked
          <<" raw_numeric_entries="<<192*CommitmentCount<<" packet_numeric_entries="<<entries<<"\n";
}
