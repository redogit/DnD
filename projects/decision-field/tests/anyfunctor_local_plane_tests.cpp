#if !__has_include("decision_field/anyfunctor_local_plane.hpp")
#include <iostream>
int main(){std::cerr<<"FAIL: AnyFunctor local-plane adapter absent\n";return 1;}
#else
#include "decision_field/anyfunctor_local_plane.hpp"
#include "decision_field/self_state_codec.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;
using namespace df::self_architecture;
using namespace df::functions;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)

int main(){
    const auto p=make_policy();
    const auto o=obligations();
    const auto codec=persistence::self_state_codec();
    CHECK(o.items.size()>=2);

    DecisionFieldEngine<SelfState> e(p,o);
    auto value=architecture_self();
    value.markers={-8,13};
    value.orientation=2;
    const auto root=e.add_root(value,{"exact source occurrence"});
    const auto twin=e.add_root(value,{"equal value, distinct occurrence"});

    const auto mirror=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),o,p,codec);
    auto plane=LocalPlaneFor("fixture-local-plane@1",mirror);

    const AnyInvocation<SelfState> root_in{mirror,{root},{"context:A",""},"fixture-space@1"};
    const auto first=LocalAnyFunctor(plane,e,root_in,"mirror/self");
    CHECK(first.execution.computation==ExecutionStatus::Succeeded);
    CHECK(first.execution.admission==ExecutionStatus::Succeeded);
    CHECK(!first.execution.memoized);
    CHECK(first.transition.novel);
    CHECK(first.transition.integrated);
    CHECK(!first.transition.reused);
    CHECK(plane.revision()==1);
    CHECK(plane.route_count()==1);
    CHECK(plane.resolve(LocalContextSignature(root_in.execution_space,root_in.context.configuration_fingerprint),"mirror/self")==
          std::optional<std::string>{mirror.id()});

    const auto again=LocalAnyFunctor(plane,e,root_in,"mirror/self");
    CHECK(again.execution.memoized);
    CHECK(!again.transition.novel);
    CHECK(!again.transition.integrated);
    CHECK(again.transition.reused);
    CHECK(plane.revision()==1);
    CHECK(plane.route_count()==1);

    const AnyInvocation<SelfState> twin_in{mirror,{twin},{"context:A",""},"fixture-space@1"};
    const auto twin_run=LocalAnyFunctor(plane,e,twin_in,"mirror/self");
    CHECK(!twin_run.execution.memoized);
    CHECK(twin_run.transition.reused);
    CHECK(twin_run.execution.sources==std::vector<StateId>{twin});
    CHECK(twin_run.execution.invocation_id!=first.execution.invocation_id);
    CHECK(plane.route_count()==1);

    const auto bad=FunctionObject<SelfState>::mirror("self-policy@1",absolute_axes(),o,p,codec);
    const auto denied=LocalAnyFunctor(
        plane,e,AnyInvocation<SelfState>{bad,{root},{"context:A",""},"fixture-space@1"},"mirror/absolute");
    CHECK(denied.execution.computation==ExecutionStatus::Succeeded);
    CHECK(denied.execution.admission==ExecutionStatus::Rejected);
    CHECK(!denied.transition.integrated);
    CHECK(denied.transition.residual_index.has_value());
    CHECK(plane.residuals().back().reason=="execution-not-admitted");
    CHECK(!plane.resolve(LocalContextSignature("fixture-space@1","context:A"),"mirror/absolute").has_value());

    std::cout<<"PASS AnyFunctor local plane: admitted route integration, reuse without occurrence collapse, and rejected residual retention\n";
}
#endif
