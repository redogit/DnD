#include "decision_field/checkpoint_file.hpp"
#include "decision_field/self_state_codec.hpp"
#include <iostream>
using namespace df;using namespace df::self_architecture;using namespace df::persistence;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
std::vector<TransformDefinition<SelfState>> bindings(){
 const auto codec=self_state_codec();
 return {
 {"marker","1",Reversibility::Exact,
 [](std::span<const SelfState> in,const InvocationContext&){auto v=in[0];++v.orientation;return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{v},{"marker-note"},std::string("receipt\0exact",13)};},
 [](std::span<const SelfState> out,const InvocationContext&,std::string_view){auto v=out[0];--v.orientation;return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{v},{},{}};}},
 {"failure","1",Reversibility::Exact,
 [](std::span<const SelfState>,const InvocationContext&){return TransformExecution<SelfState>{ExecutionStatus::Failed,{}, {"preserved negative result"},"failure receipt"};},
 [](std::span<const SelfState>,const InvocationContext&,std::string_view){return TransformExecution<SelfState>{};}},
 {"sink","1",Reversibility::Exact,
 [codec](std::span<const SelfState> in,const InvocationContext&){return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{}, {"zero outputs"},codec.encode(in[0])};},
 [codec](std::span<const SelfState>,const InvocationContext&,std::string_view receipt){return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{codec.decode(receipt)},{},{}};}},
 {"generate","1",Reversibility::Exact,
 [](std::span<const SelfState>,const InvocationContext& c){auto v=architecture_self();v.orientation=std::stoi(c.configuration_fingerprint);return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{v},{"zero inputs"},"generator-receipt"};},
 [](std::span<const SelfState>,const InvocationContext&,std::string_view){return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{},{},{}};}},
 {"pair","1",Reversibility::Exact,
 [](std::span<const SelfState> in,const InvocationContext&){return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{in.begin(),in.end()},{"ordered repeated input binding"},{}};},
 [](std::span<const SelfState> out,const InvocationContext&,std::string_view){return TransformExecution<SelfState>{ExecutionStatus::Succeeded,{out.begin(),out.end()},{},{}};}}
 };
}
EngineCheckpoint<SelfState> build(){
 const auto policy=make_policy();const auto obs=obligations();DecisionFieldEngine<SelfState> engine(policy,obs);
 const auto a=architecture_self();const auto b=policy.mirror(a,lawful_axes(9),obs);const auto m=policy.overlap(a,b,obs);
 CHECK(m.shared.has_value());const auto left=engine.add_root(a,{"pole:A"});const auto right=engine.add_root(b,{"pole:B"});const auto middle=engine.add_root(*m.shared,{"shared-core"});
 const auto near=engine.explore_triad_surface(left,right,middle,1,0.02,0.05);
 CHECK(near.surface.has_value());
 const auto far=engine.explore_triad_surface(left,right,middle,3,0.02,std::nextafter(near.surface->max_radial_departure,std::numeric_limits<double>::infinity()));
 CHECK(far.surface.has_value());CHECK(far.observations.size()==30144);
 auto rich=a;rich.orientation=-83;rich.markers={-700,0,40000};const auto root=engine.add_root(rich,{"native:rich-root",std::string("origin\0full",11)});
 auto registry=bindings();for(auto t:registry)engine.register_transform(t);
 const std::array<StateId,1> in{root};const std::array<StateId,2> repeated{root,root};
 auto good=engine.execute("marker",in,{"configuration","entropy-fingerprint"});
 (void)engine.execute("failure",in);(void)engine.execute("sink",in);(void)engine.execute("generate",{},{"37","seed-fingerprint"});(void)engine.execute("pair",repeated);
 engine.mark_dependency(good.outputs[0],"stale-marker");(void)engine.invalidate_dependency("stale-marker");engine.mark_dependency(root,"resume-root");
 ResidualNote note;note.semantic_signature="deep-separation";note.counter_evidence_refs={"preserved-negative"};note.occurrence_count=4;engine.publish_residual(note);
 SharedFix fix;fix.id="cohort-fix";fix.applicable_workers={"0","3"};fix.completed_sync_rounds=4;fix.status=FixStatus::ScopedStable;fix.evidence={"supported-locally"};fix.counter_evidence={"not-universal"};engine.publish_fix(fix);
 return engine.export_checkpoint();
}
int main(int argc,char** argv){try{
 CHECK(argc>=3);const std::string mode=argv[1];const auto codec=self_state_codec();const auto policy=make_policy();
 const ArchiveIdentity identity{"process-native","self-policy","v03",codec.id,codec.version,{}};
 if(mode=="write"){
  const auto c=build();const auto bytes=pack_native(c,identity,codec,policy);save_new_checkpoint(argv[2],bytes);
  std::cout<<"{\"states\":"<<c.states.size()<<",\"edges\":"<<c.edges.size()<<",\"invocations\":"<<c.invocations.size()<<",\"bytes\":"<<bytes.size()<<",\"sha256\":\""<<archive_sha256(bytes)<<"\"}\n";return 0;
 }
 CHECK(mode=="read"&&argc==4);const auto bytes=read_checkpoint(argv[2],{},argv[3]);
 const auto c=unpack_native<SelfState>(bytes,identity,codec,policy);
 CHECK(pack_native(c,identity,codec,policy)==bytes);CHECK(c.states.size()>30000);CHECK(c.edges.size()==5);CHECK(c.invocations.size()==5);
 StateId root=0;for(const auto& [id,n]:c.states)if(checkpoint_has(n.provenance,std::string("native:rich-root")))root=id;
 CHECK(root);auto rich=architecture_self();rich.orientation=-83;rich.markers={-700,0,40000};CHECK(*c.states.at(root).value==rich);
 CHECK(c.research.residuals_by_signature.at("deep-separation").counter_evidence_refs[0]=="preserved-negative");
 CHECK(c.research.fixes_by_id.at("cohort-fix").completed_sync_rounds==4);
 CHECK((c.edges.at(5).from_states==std::vector<StateId>{root,root}));
 CHECK(std::count(c.states.at(root).child_edges.begin(),c.states.at(root).child_edges.end(),EdgeId(5))==2);
 CHECK(c.invocations.at(2).status==ExecutionStatus::Failed);CHECK(c.invocations.at(2).notes[0]=="preserved negative result");
 auto registry=bindings();auto restored=DecisionFieldEngine<SelfState>::restore_checkpoint(policy,c,registry);
 const std::array<StateId,1> in{root};auto reused=restored.execute("marker",in,{"configuration","entropy-fingerprint"});CHECK(reused.memoized_reuse);CHECK(reused.id==1);
 CHECK(restored.state(reused.outputs[0]).stale);auto sink=restored.execute("sink",in);CHECK(sink.memoized_reuse&&sink.outputs.empty());
 const auto inverse=registry[2].reverse({},c.edges.at(sink.edge_id).context,c.edges.at(sink.edge_id).recovery_receipt);CHECK(inverse.outputs.size()==1&&inverse.outputs[0]==rich);
 const auto count_before=restored.state_count();const auto affected=restored.invalidate_dependency("resume-root");CHECK(affected.size()==4);CHECK(restored.state_count()==count_before);
 const auto newroot=restored.add_root(rich);CHECK(newroot==c.next_state_id);
 const std::array<StateId,1> follow{reused.outputs[0]};auto next=restored.execute("marker",follow,{"new-configuration","entropy-fingerprint"});CHECK(!next.memoized_reuse);CHECK(next.id==c.next_invocation_id);CHECK(next.edge_id==c.next_edge_id);
 std::cout<<"{\"states\":"<<c.states.size()<<",\"edges\":"<<c.edges.size()<<",\"invocations\":"<<c.invocations.size()<<",\"bytes\":"<<bytes.size()<<",\"sha256\":\""<<archive_sha256(bytes)<<"\",\"resume\":true}\n";
 return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
