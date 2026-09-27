#include "decision_field/native_archive.hpp"
#include "decision_field/self_state_codec.hpp"
#include <iostream>
#include <stdexcept>
using namespace df;using namespace df::self_architecture;using namespace df::persistence;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
template<class F> void rejects(F f){bool threw=false;try{f();}catch(const std::exception&){threw=true;}CHECK(threw);}
int main(){
 const auto policy=make_policy();const auto codec=self_state_codec();
 const ArchiveIdentity id{"archive-run","self-policy","v03",codec.id,codec.version,{}};
 DecisionFieldEngine<SelfState> engine(policy,obligations());
 auto value=architecture_self();value.orientation=-83;value.markers={-700,0,40000};
 const auto root=engine.add_root(value,{std::string("origin\0exact",12)});
 (void)engine.build_directional_field(root);
 engine.mark_dependency(root,"source");
 (void)engine.invalidate_dependency("source");
 auto original=engine.export_checkpoint();
 original.research.counterexamples={"do not erase",std::string("NUL\0evidence",12)};
 original.research.evidence={"not an admission"};
 SharedFix fix;fix.id="f";fix.completed_sync_rounds=4;fix.evidence={"kept"};fix.counter_evidence={"also kept"};
 fix.applicable_workers={"b","a"};fix.round_metrics.push_back({true,1,2,3,-0.0,5});
 original.research.fixes_by_id.emplace("f",fix);
 ResidualNote note;note.semantic_signature="why-deep";note.smallest_known_difference="orientation";note.occurrence_count=9;
 note.evidence_refs={"yes"};note.counter_evidence_refs={"no"};original.research.residuals_by_signature.emplace("why-deep",note);
 original.research.concessions_by_signature.emplace("conflict",ConcessionRecord{"c","conflict",{"b","a"},{"e"},{"different"}});
 const auto packed=pack_native(original,id,codec,policy);
 const auto decoded=unpack_native<SelfState>(packed,id,codec,policy);
 CHECK(decoded.states.size()==original.states.size());CHECK(*decoded.states.at(root).value==value);
 CHECK(decoded.states.at(root).provenance==original.states.at(root).provenance);
 CHECK(decoded.states.at(root).stale);
 CHECK(decoded.research.counterexamples==original.research.counterexamples);
 CHECK(std::bit_cast<std::uint64_t>(decoded.research.fixes_by_id.at("f").round_metrics[0].decay_risk)==std::bit_cast<std::uint64_t>(-0.0));
 CHECK(pack_native(decoded,id,codec,policy)==packed);
 auto reordered=original;reordered.states.rehash(10007);reordered.dependencies_by_state.rehash(3007);
 CHECK(pack_native(reordered,id,codec,policy)==packed);
 auto wrong=id;wrong.policy_version="different";rejects([&]{(void)unpack_native<SelfState>(packed,wrong,codec,policy);});
 wrong=id;wrong.codec_version="different";rejects([&]{(void)unpack_native<SelfState>(packed,wrong,codec,policy);});
 for(const auto index:{std::size_t(0),std::size_t(8),std::size_t(25),packed.size()-1}) {
  auto damaged=packed;damaged[index]=static_cast<char>(damaged[index]^1);
  rejects([&]{(void)unpack_native<SelfState>(damaged,id,codec,policy);});
 }
 for(const auto n:{std::size_t(0),std::size_t(12),packed.size()-1})
  rejects([&]{(void)unpack_native<SelfState>(std::string_view(packed).substr(0,n),id,codec,policy);});
 rejects([&]{(void)unpack_native<SelfState>(packed+"extra",id,codec,policy);});
 ArchiveLimits limits;limits.max_archive_bytes=packed.size()-1;
 rejects([&]{(void)unpack_native<SelfState>(packed,id,codec,policy,limits);});
 limits={};limits.max_collection_entries=10;
 rejects([&]{(void)unpack_native<SelfState>(packed,id,codec,policy,limits);});
 limits={};limits.max_string_bytes=2;
 rejects([&]{(void)unpack_native<SelfState>(packed,id,codec,policy,limits);});
 auto broken=original;broken.states.at(root).fingerprint="wrong";
 rejects([&]{(void)pack_native(broken,id,codec,policy);});
 auto lossy=codec;lossy.encode=[](const SelfState&){return std::string("lost");};lossy.decode=[](std::string_view){return architecture_self();};
 rejects([&]{(void)pack_native(original,id,lossy,policy);});
 // Bypass producer validation only inside this test to exercise the untrusted reader.
 auto forge=[&](const EngineCheckpoint<SelfState>& candidate){
  wire::Writer<SelfState> writer(codec,{});writer(id,candidate);
  std::string bytes="DFNAT001";wire::append_u64(bytes,1);wire::append_u64(bytes,writer.data.size());bytes+=writer.data;bytes+=sha256_bytes(bytes);return bytes;
 };
 broken=original;broken.states.at(root).child_states.clear();
 rejects([&]{(void)unpack_native<SelfState>(forge(broken),id,codec,policy);});
 broken=original;broken.next_state_id=1;
 rejects([&]{(void)unpack_native<SelfState>(forge(broken),id,codec,policy);});
 broken=original;broken.states.at(root).probe=ProbeAddress{{9,0},0};
 rejects([&]{(void)unpack_native<SelfState>(forge(broken),id,codec,policy);});
 broken=original;broken.dependents_by_key.clear();
 rejects([&]{(void)unpack_native<SelfState>(forge(broken),id,codec,policy);});
 {wire::Reader<SelfState> reader(std::string_view("\2",1),codec,{});bool b{};rejects([&]{reader(b);});}
 // Bounded, deterministic malformed-body probing; no authority attaches to a re-signed body.
 std::size_t accepted=0,rejected=0;std::uint64_t seed=0xD1EC7;
 for(unsigned attempt=0;attempt<256;++attempt){
  seed=seed*6364136223846793005ULL+1;
  auto mutant=packed;const auto offset=24+static_cast<std::size_t>(seed%(mutant.size()-56));
  mutant[offset]=static_cast<char>(static_cast<unsigned char>(mutant[offset])^static_cast<unsigned char>((seed>>32)|1U));
  mutant.replace(mutant.size()-32,32,sha256_bytes(std::string_view(mutant).substr(0,mutant.size()-32)));
  try {const auto changed=unpack_native<SelfState>(mutant,id,codec,policy);validate_checkpoint(changed,policy);++accepted;}
  catch(const std::exception&){++rejected;}
 }
 CHECK(accepted+rejected==256);CHECK(rejected>0);
 std::cout<<"re-signed mutation probes accepted_as_structurally_valid="<<accepted<<" rejected="<<rejected<<" (not authentication)\n";
 std::cout<<"native archive: "<<decoded.states.size()<<" exact states; "<<packed.size()<<" bytes; sha256="<<archive_sha256(packed)<<"\n";
}
