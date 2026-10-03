#if !__has_include("decision_field/anyfunctor_mirror.hpp")
#include <iostream>
int main(){std::cerr<<"FAIL: first-class AnyFunctor Mirror implementation absent\n";return 1;}
#else
#include "decision_field/anyfunctor_mirror.hpp"
#include "decision_field/self_state_codec.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;
using namespace df::self_architecture;
using namespace df::functions;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
template<class F>void rejects(F f){bool did=false;try{f();}catch(const std::exception&){did=true;}CHECK(did);}
int main(){
    const auto p=make_policy();const auto o=obligations();const auto codec=persistence::self_state_codec();
    DecisionFieldEngine<SelfState> e(p,o);
    auto value=architecture_self();value.markers={-8,13};value.orientation=2;
    const auto root=e.add_root(value,{"exact source occurrence"});
    const auto twin=e.add_root(value,{"equal value, distinct occurrence"});
    const auto mirror=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),o,p,codec);
    AnyInvocation<SelfState> in{mirror,{root},{"context:A",""},"fixture-space@1"};
    const auto a=AnyFunctor(e,in);
    CHECK(a.computation==ExecutionStatus::Succeeded);CHECK(a.admission==ExecutionStatus::Succeeded);
    CHECK(a.admitted.size()==1);CHECK(a.retained_candidates.size()==1);CHECK(!a.memoized);
    CHECK(a.sources==std::vector<StateId>{root});
    CHECK(e.edge(a.edge_id).from_states==a.sources);
    CHECK(e.edge(a.edge_id).recovery_receipt==a.recovery_receipt);
    const auto expected=p.mirror(value,lawful_axes(),o);
    CHECK(*e.state(a.admitted[0]).value==expected);CHECK(a.retained_candidates[0]==expected);
    CHECK(Homeward(e,mirror,a.invocation_id)[0]==value);
    const auto count=e.state_count();
    const auto again=AnyFunctor(e,in);CHECK(again.memoized);CHECK(again.invocation_id==a.invocation_id);
    CHECK(e.state_count()==count);
    in.context.configuration_fingerprint="context:B";
    const auto other_context=AnyFunctor(e,in);CHECK(!other_context.memoized);CHECK(other_context.invocation_id!=a.invocation_id);
    in.sources={twin};in.context.configuration_fingerprint="context:A";
    const auto other_occurrence=AnyFunctor(e,in);CHECK(other_occurrence.invocation_id!=a.invocation_id);CHECK(other_occurrence.sources[0]==twin);
    in.sources=a.admitted;
    const auto twice=AnyFunctor(e,in);CHECK(twice.admission==ExecutionStatus::Succeeded);CHECK(*e.state(twice.admitted[0]).value==value);
    const auto bad=FunctionObject<SelfState>::mirror("self-policy@1",absolute_axes(),o,p,codec);
    const auto before=e.state_count();
    const auto denied=AnyFunctor(e,AnyInvocation<SelfState>{bad,{root},{"context:A",""},"fixture-space@1"});
    CHECK(denied.computation==ExecutionStatus::Succeeded);CHECK(denied.admission==ExecutionStatus::Rejected);
    CHECK(denied.admitted.empty());CHECK(denied.retained_candidates.size()==1);CHECK(e.state_count()==before);
    CHECK(denied.retained_candidates[0]==p.mirror(value,absolute_axes(),o));
    CHECK(!denied.violations.empty());CHECK(e.edge(denied.edge_id).from_states[0]==root);
    rejects([&]{(void)Homeward(e,bad,denied.invocation_id);});
    auto changed_axes=lawful_axes();changed_axes.evidence_refs.push_back("new axes provenance");
    const auto revision=FunctionObject<SelfState>::mirror("self-policy@1",changed_axes,o,p,codec);
    CHECK(revision.id()!=mirror.id());
    auto noninvolutive=p;noninvolutive.mirror=[](const SelfState& x,const MirrorAxisState&,const ObligationSet&){auto y=x;++y.orientation;return y;};
    const auto broken=FunctionObject<SelfState>::mirror("non-involution@1",lawful_axes(),o,noninvolutive,codec);
    const auto failed=AnyFunctor(e,AnyInvocation<SelfState>{broken,{root},{},"fixture-space@1"});
    CHECK(failed.admitted.empty());CHECK(failed.admission==ExecutionStatus::Rejected);CHECK(failed.retained_candidates.size()==1);
    const auto frozen=e.export_checkpoint();
    const std::array<TransformDefinition<SelfState>,3> registry{mirror.binding(),bad.binding(),broken.binding()};
    auto restored=DecisionFieldEngine<SelfState>::restore_checkpoint(p,frozen,registry);
    in={mirror,{root},{"context:A",""},"fixture-space@1"};
    const auto replay=AnyFunctor(restored,in);CHECK(replay.memoized);CHECK(replay.invocation_id==a.invocation_id);
    CHECK(Homeward(restored,mirror,a.invocation_id)[0]==value);
    const auto rejected_replay=AnyFunctor(restored,AnyInvocation<SelfState>{bad,{root},{"context:A",""},"fixture-space@1"});
    CHECK(rejected_replay.memoized);CHECK(rejected_replay.admission==ExecutionStatus::Rejected);
    CHECK(rejected_replay.retained_candidates==denied.retained_candidates);
    CHECK(restored.state_count()==e.state_count());
    auto stale=restored;stale.mark_dependency(root,"axis");(void)stale.invalidate_dependency("axis");
    rejects([&]{(void)AnyFunctor(stale,in);});
    auto wrong=o;wrong.id="different obligation";
    const auto wrong_fn=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),wrong,p,codec);
    rejects([&]{(void)AnyFunctor(restored,AnyInvocation<SelfState>{wrong_fn,{root},{},"fixture-space@1"});});
    std::cout<<"PASS AnyFunctor Mirror: shared invocation, separate admission, exact rejected candidate retention, occurrence/context keys, involution, checkpoint replay and Homeward\n";
}
#endif
