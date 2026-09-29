#include "decision_field/rmal_mirror_host.hpp"
#include "decision_field/checkpoint_file.hpp"
#include "decision_field/self_state_codec.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace df;using namespace df::self_architecture;using namespace df::functions;using namespace df::persistence;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
SelfState input(){auto v=architecture_self();v.orientation=19;v.markers={-8,4};return v;}
void program(DecisionFieldEngine<SelfState>& engine,const std::string& source){
 const auto p=make_policy();const auto o=obligations();const auto codec=self_state_codec();
 const auto good=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),o,p,codec);
 const auto bad=FunctionObject<SelfState>::mirror("self-policy@1",absolute_axes(),o,p,codec);
 RmalMirrorHost<SelfState> host(engine,{archive_sha256(source),""},"rmal-space@1");host.add(1,good);host.add(2,bad);
 std::unique_ptr<RmalVm,decltype(&rmal_vm_free)> vm(rmal_vm_create(),rmal_vm_free);CHECK(vm&&host.bind(vm.get()).ok);
 RmalStatus s{};std::unique_ptr<RmalProgram,decltype(&rmal_program_free)> parsed(rmal_parse_source(source.c_str(),"mirror_homeward.rmal",&s),rmal_program_free);
 if(!s.ok)throw std::runtime_error(s.message);CHECK(parsed);
 std::unique_ptr<RmalBytecode,decltype(&rmal_bytecode_free)> bytecode(rmal_compile(parsed.get(),&s),rmal_bytecode_free);
 if(!s.ok)throw std::runtime_error(s.message);CHECK(bytecode);
 RmalValue result{};s=rmal_vm_run(vm.get(),bytecode.get(),true,&result);rmal_value_free(&result);
 if(!s.ok)throw std::runtime_error(s.message);
 CHECK(host.last_recovered().size()==1&&host.last_recovered()[0]==input());
 CHECK(engine.state_count()==3&&*engine.state(3).value==input());
 CHECK(engine.export_checkpoint().invocations.size()==3);
 const auto& failed=engine.invocation_record(3);const auto rejected=bad.receipt(failed,engine.edge(failed.edge_id));
 CHECK(rejected.admission==ExecutionStatus::Rejected&&rejected.retained_candidates.size()==1);
 CHECK(rejected.retained_candidates[0]==p.mirror(input(),absolute_axes(),o));
 bool traced=false;for(size_t i=0;i<rmal_vm_trace_count(vm.get());++i){const auto* t=rmal_vm_trace_at(vm.get(),i);if(t->detail&&std::string(t->detail)=="AnyFunctor"&&t->line>0)traced=true;}CHECK(traced);
}
int main(int argc,char** argv){try{
 CHECK(argc==4||argc==5);const std::string mode=argv[1];std::ifstream f(argv[3]);CHECK(f);std::ostringstream read;read<<f.rdbuf();const auto source=read.str();
 const auto codec=self_state_codec();const auto p=make_policy();const auto o=obligations();
 const ArchiveIdentity identity{"rmal-mirror-homeward/1:"+archive_sha256(source),"self-policy","1",codec.id,codec.version,{}};
 std::string bytes;
 if(mode=="write"){
  CHECK(argc==4);DecisionFieldEngine<SelfState> engine(p,o);CHECK(engine.add_root(input(),{"RMAL source occurrence:fixture@1"})==1);
  program(engine,source);bytes=pack_native(engine.export_checkpoint(),identity,codec,p);save_new_checkpoint(argv[2],bytes);
 }else{
  CHECK(mode=="read"&&argc==5);bytes=read_checkpoint(argv[2],{},argv[4]);const auto c=unpack_native<SelfState>(bytes,identity,codec,p);
  CHECK(pack_native(c,identity,codec,p)==bytes);
  const auto good=FunctionObject<SelfState>::mirror("self-policy@1",lawful_axes(),o,p,codec);
  const auto bad=FunctionObject<SelfState>::mirror("self-policy@1",absolute_axes(),o,p,codec);
  const std::array<TransformDefinition<SelfState>,2> registry{good.binding(),bad.binding()};
  auto restored=DecisionFieldEngine<SelfState>::restore_checkpoint(p,c,registry);
  program(restored,source);
  const auto replay=AnyFunctor(restored,AnyInvocation<SelfState>{good,{1},{archive_sha256(source),""},"rmal-space@1"});CHECK(replay.memoized&&replay.invocation_id==1);
  CHECK(pack_native(restored.export_checkpoint(),identity,codec,p)==bytes);
 }
 std::cout<<"{\"states\":3,\"edges\":3,\"invocations\":3,\"bytes\":"<<bytes.size()<<",\"sha256\":\""<<archive_sha256(bytes)<<"\",\"homeward_sha256\":\""<<archive_sha256(codec.encode(input()))<<"\",\"rejected_candidates_retained\":1,\"rmal_vm_executed\":true}";
 // Keep output a single compact, parseable receipt; actual RMAL VM ran above.
 return 0;
}catch(const std::exception& ex){std::cerr<<ex.what()<<'\n';return 2;}}
