#include "decision_field/component_adapter.hpp"
#include "decision_field/core_replay.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <unordered_map>
using namespace df::bridge;
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while(false)
template<class F> void rejects(F f) { bool ok=false; try {f();} catch(const std::invalid_argument&) {ok=true;} CHECK(ok); }
// Owned synthetic records test the adapter contract; actual v0.3 execution is tested separately.
struct Direction {std::size_t sector{},subdivision{};};
struct Address {Direction parent; std::size_t outward_index{};};
struct Metrics {double obligation_progress{1},information_gain{1},counterexample_value{},uncertainty_reduction{},convergence_potential{},transferability{},complexity_cost{},execution_cost{},recovery_cost{},decay_risk{};};
struct Probe {Address address; std::uint64_t state_id; Metrics evaluation;};
struct Node {std::uint64_t id; std::shared_ptr<const std::vector<double>> value; std::string fingerprint; std::vector<std::uint64_t> parent_states; bool stale{}; std::size_t depth{}; std::vector<std::string> provenance;};
struct Store {std::unordered_map<std::uint64_t,Node> nodes; const Node& state(std::uint64_t id) const {return nodes.at(id);} };
struct Field {std::uint64_t core{1}; std::array<std::uint64_t,16> first_ring{}; std::array<std::array<Probe,12>,16> outward{}; std::size_t baseline_executions{192};};
struct Fixture {
 Store store; Field field;
 Fixture() {
  add(1,{0,0,1},{},0);
  for(std::size_t d=0;d<16;++d) {
   auto parent=std::uint64_t(2+d); field.first_ring[d]=parent; add(parent,{0,0,1},{1},1);
   for(std::size_t j=0;j<12;++j) {
    auto id=std::uint64_t(100+d*12+j);
    add(id,{double(d)-8,double(j)-6,1},{parent},2);
    field.outward[d][j]={{{d/4,d%4},j},id,{}};
   }
  }
 }
 void add(std::uint64_t id,std::vector<double> values,std::vector<std::uint64_t> parents,std::size_t depth) {
  store.nodes.emplace(id,Node{id,std::make_shared<const std::vector<double>>(std::move(values)),"source:"+std::to_string(id),std::move(parents),false,depth,{"synthetic-fixture"}});
 }
};
RunContext context() {return {"synthetic-run", "O", "1", "coordinates/v1", "quality/v1", "axes/v1"};}
auto project=[](const auto& node){return *node.value;};
auto assess=[](const auto& m){return ObservationAssessment{m.obligation_progress,m.information_gain,1,0,m.recovery_cost,m.decay_risk};};
auto fixture_frame(Fixture& f) {return make_baseline_frame(f.store,f.field,context(),project,assess);}
void adapter_coverage_and_snapshot() {
 Fixture f; auto frame=fixture_frame(f);
 CHECK(frame.frame.observations.size()==192); CHECK(frame.sources.size()==192);
 CHECK(frame.first_ring.size()==16); CHECK(frame.sources.back().source.state_id==291);
 CHECK(frame.sources[0].source.parents==std::vector<std::uint64_t>{2});
 CHECK(frame.sources[0].raw_metrics.size()==10);
 f.store.nodes.at(100).value=std::make_shared<const std::vector<double>>(std::vector<double>{999,999,999});
 CHECK(frame.frame.observations[0].values[0]==-8); // snapshot, not mutable alias
 f.field.baseline_executions=191; rejects([&]{(void)fixture_frame(f);});
}
void adapter_rejects_alias_staleness_and_wrong_direction() {
 Fixture f; f.field.outward[0][1].state_id=100; rejects([&]{(void)fixture_frame(f);});
 Fixture g; g.store.nodes.at(100).stale=true; rejects([&]{(void)fixture_frame(g);});
 Fixture h; h.field.outward[0][0].address.parent.sector=3; rejects([&]{(void)fixture_frame(h);});
 Fixture q; q.store.nodes.at(100).parent_states={999}; rejects([&]{(void)fixture_frame(q);});
 Fixture u; int calls=0;
 rejects([&]{(void)make_baseline_frame(u.store,u.field,context(),[&](const auto& n){auto v=*n.value; v[0]+=double(calls++);return v;},assess);});
}
ObligationOracle oracle() {
 return {"fixture-oracle","1",{"constant-z","nonnegative-x"},[](std::span<const double> v){
  return std::vector<Verdict>{v[2]==1 ? Verdict::Pass:Verdict::Fail,v[0]>=0 ? Verdict::Pass:Verdict::Fail};
 }};
}
void packet_roundtrip_preserves_negative_cases() {
 Fixture f; auto frame=fixture_frame(f); auto packet=make_core_packet(frame,0);
 CHECK(packet.summary.stable_parameters[2].value()==1);
 CHECK(packet.summary.residual_dimensions.size()==2);
 auto restored=reconstruct(packet);
 CHECK(restored.size()==192);
 CHECK(restored.front().values==frame.frame.observations.front().values);
 auto report=replay_core(frame,packet,oracle());
 CHECK(report.disposition==Disposition::Keep); CHECK(report.rows_checked==192);
 CHECK(report.preserved_negative_checks==96); CHECK(!report.all_obligations_pass);
 CHECK(report.cases.size()==192);
 // Range summaries alone are insufficient: remove a required per-row value.
 packet.rows.front().overrides.clear();
 CHECK(replay_core(frame,packet,oracle()).disposition==Disposition::Repair);
}
void oracle_unknown_and_tampering_fail_closed() {
 Fixture f; auto frame=fixture_frame(f); auto packet=make_core_packet(frame,0);
 auto unknown=oracle(); unknown.evaluate=[](std::span<const double>){return std::vector<Verdict>{Verdict::Pass,Verdict::Unknown};};
 CHECK(replay_core(frame,packet,unknown).disposition==Disposition::Unresolved);
 auto missing=oracle(); missing.evaluate=[](std::span<const double>){return std::vector<Verdict>{Verdict::Pass};};
 CHECK(replay_core(frame,packet,missing).disposition==Disposition::Unresolved);
 packet.rows.front().overrides[0].value=99;
 CHECK(replay_core(frame,packet,oracle()).disposition==Disposition::Repair);
 packet=make_core_packet(frame,0); packet.context.obligation_version="changed";
 CHECK(replay_core(frame,packet,oracle()).disposition==Disposition::Unresolved);
}
void tolerant_core_still_recovers_exact_projection_bits() {
 Fixture f;
 f.store.nodes.at(100).value=std::make_shared<const std::vector<double>>(std::vector<double>{-0.0,0,1.00000001});
 auto frame=fixture_frame(f); auto packet=make_core_packet(frame,1e-6);
 auto restored=reconstruct(packet);
 for(std::size_t i=0;i<192;++i) for(std::size_t d=0;d<3;++d)
  CHECK(std::bit_cast<std::uint64_t>(restored[i].values[d])==std::bit_cast<std::uint64_t>(frame.frame.observations[i].values[d]));
}
int main() {
 adapter_coverage_and_snapshot(); adapter_rejects_alias_staleness_and_wrong_direction();
 packet_roundtrip_preserves_negative_cases(); oracle_unknown_and_tampering_fail_closed();
 tolerant_core_still_recovers_exact_projection_bits();
 std::cout<<"5 bridge groups passed: baseline, provenance, reconstruction, oracle, exact floating-point replay\n";
}
