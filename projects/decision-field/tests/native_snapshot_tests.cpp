#include "decision_field/native_checkpoint_available.hpp"
#include "decision_field/self_architecture_domain.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;
using namespace df::self_architecture;
#define CHECK(x) do { if(!(x)) throw std::runtime_error(#x); } while(false)
template<class F> void rejects(F f) { bool threw=false; try { f(); } catch(const std::exception&) {threw=true;} CHECK(threw); }
TransformDefinition<SelfState> marker_transform() {
    return {"marker","1",Reversibility::Exact,
        [](std::span<const SelfState> in,const InvocationContext&) {
            auto s=in[0]; ++s.orientation;
            return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{s},{"forward"},std::string("receipt\0kept",12)};
        },
        [](std::span<const SelfState> out,const InvocationContext&,std::string_view) {
            auto s=out[0]; --s.orientation;
            return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{s},{}, {}};
        }};
}
int main() {
    const auto policy=make_policy(); const auto obs=obligations();
    DecisionFieldEngine<SelfState> engine(policy,obs);
    auto a=architecture_self(); a.markers={-9,7,99}; a.orientation=3;
    auto root=engine.add_root(a,{"origin",std::string("native\0source",13)});
    const std::array<StateId,1> inputs{root};
    std::array<TransformDefinition<SelfState>,2> bindings{marker_transform(),
        TransformDefinition<SelfState>{"failure","1",Reversibility::Exact,
        [](std::span<const SelfState>,const InvocationContext&) {
            return TransformExecution<SelfState>{ExecutionStatus::Failed,{}, {"negative evidence"},"failed-receipt"};},
        [](std::span<const SelfState>,const InvocationContext&,std::string_view) { return TransformExecution<SelfState>{}; }}};
    for(auto t:bindings)engine.register_transform(t);
    auto good=engine.execute("marker",inputs,{"configuration","entropy"});
    auto bad=engine.execute("failure",inputs);
    auto field=engine.build_directional_field(good.outputs[0]);
    engine.mark_dependency(good.outputs[0],"axis:1");
    SharedFix fix; fix.id="fix";fix.applicable_workers={"0","3"};fix.completed_sync_rounds=4;fix.status=FixStatus::ScopedStable;
    fix.round_metrics.push_back({true,1,2,3,4,5}); engine.publish_fix(fix);
    ResidualNote note;note.semantic_signature="depth";note.counter_evidence_refs={"negative"};engine.publish_residual(note);
    auto frozen=engine.export_checkpoint();
    CHECK(frozen.states.size()==engine.state_count()); CHECK(frozen.edges.size()==2);
    CHECK(frozen.invocations.at(bad.id).notes[0]=="negative evidence");
    auto resumed=DecisionFieldEngine<SelfState>::restore_checkpoint(policy,frozen,bindings);
    CHECK(*resumed.state(root).value==a);CHECK(resumed.state(root).provenance==engine.state(root).provenance);
    CHECK(resumed.research_state().fixes_by_id.at("fix").completed_sync_rounds==4);
    CHECK(resumed.research_state().residuals_by_signature.at("depth").counter_evidence_refs[0]=="negative");
    auto hit=resumed.execute("marker",inputs,{"configuration","entropy"});CHECK(hit.memoized_reuse);CHECK(hit.id==good.id);
    auto invalidated=resumed.invalidate_dependency("axis:1");CHECK(invalidated.size()==209);
    CHECK(resumed.state(field.outward[15][11].state_id).stale);
    CHECK(resumed.state_count()==engine.state_count());CHECK(resumed.add_root(a)==frozen.next_state_id);
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,frozen,{});});
    auto wrong=bindings;wrong[0].version="2";
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,frozen,wrong);});
    auto corrupt=frozen;corrupt.states.at(root).child_states.clear();
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,corrupt,bindings);});
    corrupt=frozen;corrupt.next_state_id=root;
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,corrupt,bindings);});
    corrupt=frozen;corrupt.invocation_by_key.clear();
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,corrupt,bindings);});
    corrupt=frozen;corrupt.dependencies_by_state.clear();
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,corrupt,bindings);});
    corrupt=frozen;
    const auto original_key=corrupt.invocations.at(good.id).key;
    corrupt.invocations.at(good.id).key="forged-key";
    corrupt.edges.at(good.edge_id).invocation_key="forged-key";
    corrupt.invocation_by_key.erase(original_key);corrupt.invocation_by_key.emplace("forged-key",good.id);
    rejects([&]{(void)DecisionFieldEngine<SelfState>::restore_checkpoint(policy,corrupt,bindings);});
    std::cout<<"native snapshot: "<<frozen.states.size()<<" states, "<<frozen.edges.size()<<" edges; negative/cache/dependency/ID resume preserved\n";
}
