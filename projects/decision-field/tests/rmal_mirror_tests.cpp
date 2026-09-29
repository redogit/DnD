#if !__has_include("decision_field/rmal_mirror_host.hpp")
#include <iostream>
int main(){std::cerr<<"FAIL: executable RMAL Mirror host absent\n";return 1;}
#else
#include "decision_field/rmal_mirror_host.hpp"
#include "decision_field/self_state_codec.hpp"
#include <iostream>
using namespace df;using namespace df::self_architecture;using namespace df::functions;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
RmalStatus run(RmalVm* vm,const std::string& text){
 RmalStatus s{};auto* p=rmal_parse_source(text.c_str(),"mirror-test.rmal",&s);if(!p)return s;
 auto* b=rmal_compile(p,&s);if(!b){rmal_program_free(p);return s;}
 RmalValue result{};s=rmal_vm_run(vm,b,true,&result);rmal_value_free(&result);rmal_bytecode_free(b);rmal_program_free(p);return s;
}
int main(){
 const auto p=make_policy();const auto o=obligations();const auto codec=persistence::self_state_codec();
 DecisionFieldEngine<SelfState> e(p,o);auto source=architecture_self();source.markers={-8,4};source.orientation=19;
 const auto root=e.add_root(source,{"RMAL exact occurrence"});CHECK(root==1);
 const auto good=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),o,p,codec);
 const auto bad=FunctionObject<SelfState>::mirror("self-policy@1",absolute_axes(),o,p,codec);
 RmalMirrorHost<SelfState> host(e,{"rmal-context:A",""},"rmal-space@1");host.add(1,good);host.add(2,bad);
 auto* vm=rmal_vm_create();CHECK(vm);
 CHECK(!run(vm,"AnyFunctor(\"mirror\",1,1);").ok);CHECK(e.export_checkpoint().invocations.empty());
 CHECK(host.bind(vm).ok);CHECK(!host.bind(vm).ok);
 const auto digest=persistence::archive_sha256(codec.encode(source));
 const auto program=std::string("MODULE MirrorFixture\nlet first = AnyFunctor(\"mirror\",1,1);\n")+
  "assert AnyFunctor(\"status\",1,first) == \"SUCCEEDED\";\n"+
  "let reflected = AnyFunctor(\"output\",1,first);\n"+
  "assert AnyFunctor(\"homeward\",1,first) == \""+digest+"\";\n"+
  "assert AnyFunctor(\"mirror\",1,1) == first;\n"+
  "let twice = AnyFunctor(\"mirror\",1,reflected);\n"+
  "let refused = AnyFunctor(\"mirror\",2,1);\n"+
  "assert AnyFunctor(\"status\",2,refused) == \"REJECTED\";\n";
 const auto s=run(vm,program);if(!s.ok)std::cerr<<s.line<<":"<<s.column<<" "<<s.message<<"\n";CHECK(s.ok);
 CHECK(host.last_recovered().size()==1&&host.last_recovered()[0]==source);
 CHECK(e.export_checkpoint().invocations.size()==3);CHECK(e.state_count()==3);
 const auto reflected=*e.state(2).value;CHECK(reflected==p.mirror(source,lawful_axes(),o));CHECK(*e.state(3).value==source);
 CHECK(!run(vm,"AnyFunctor(\"output\",2,3);").ok);
 CHECK(!run(vm,"AnyFunctor(\"status\",2,1);").ok);
 CHECK(!run(vm,"AnyFunctor(\"homeward\",2,3);").ok);
 CHECK(!run(vm,"AnyFunctor(\"mirror\",77,1);").ok);
 CHECK(!run(vm,"AnyFunctor(\"mirror\",1,-1);").ok);
 CHECK(!run(vm,"AnyFunctor(\"mirror\",1,true);").ok);
 CHECK(!run(vm,"AnyFunctor(\"publish\",1,1);").ok);
 CHECK(!run(vm,"fn AnyFunctor(a,b,c){return 1;} AnyFunctor(\"mirror\",1,1);").ok);
 CHECK(e.export_checkpoint().invocations.size()==3);
 CHECK(run(vm,program).ok);CHECK(e.export_checkpoint().invocations.size()==3);
 bool found=false;for(size_t i=0;i<rmal_vm_trace_count(vm);++i){const auto* t=rmal_vm_trace_at(vm,i);if(t->detail&&std::string(t->detail)=="AnyFunctor"&&t->line>0)found=true;}CHECK(found);
 rmal_vm_free(vm);
 std::cout<<"PASS real RMAL VM -> AnyFunctor -> Mirror/status/output/Homeward: admission, replay, rejections, immutable host binding and source trace\n";
}
#endif
