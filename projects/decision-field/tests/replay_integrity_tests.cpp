#include "decision_field/core_replay.hpp"
#include <iostream>
#include <stdexcept>
using namespace df::bridge;
#define CHECK(x) do {if(!(x))throw std::runtime_error(#x);}while(false)
ObservedFrame make_frame() {
 ObservedFrame f;
 f.context={"small","O","1","xy","metrics","axes"}; f.core={1,{},"root",0,{}};
 f.frame={{0,0},{{2,{1,0},1,1,1,0,0,0},{3,{2,1},1,1,1,0,0,0}}};
 f.sources={{{2,{1},"a",1,{}},0,0,1,{}},{{3,{1},"b",1,{}},1,0,1,{}}};
 return f;
}
int main() {
 auto f=make_frame(); auto p=make_core_packet(f);
 ObligationOracle oracle{"nonnegative","1",{"x"},[](std::span<const double> x){return std::vector<Verdict>{x[0]>=0?Verdict::Pass:Verdict::Fail};}};
 p.summary.residual_dimensions[0].maximum=42;
 CHECK(replay_core(f,p,oracle).disposition==Disposition::Repair);
 p=make_core_packet(f);
 p.summary.stable_parameters[0]=1; // overrides still reconstruct exactly, but claimed stability is false
 p.summary.residual_dimensions.erase(p.summary.residual_dimensions.begin());
 CHECK(replay_core(f,p,oracle).disposition==Disposition::Repair);
 p=make_core_packet(f);oracle.evaluate=[](std::span<const double>){return std::vector<Verdict>{static_cast<Verdict>(17)};};
 CHECK(replay_core(f,p,oracle).disposition==Disposition::Unresolved);
 std::cout<<"summary tampering and invalid oracle verdicts rejected\n";
}
