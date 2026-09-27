#include "decision_field/component_analysis.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace df::component;
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while(false)
int main() {
    ComponentFrame f{{0,0}, {{1,{1,6},1,1,1,0,0,0}, {2,{9,-1},0,0,0,10,0,0}}};
    auto b=make_directional_basis(2,{1,0},{0,1},{});
    auto r=analyze(f,b,AnalysisConfig{1,0,2,0});
    // A negative/zero-quality case cannot be erased from the common-core claim.
    CHECK(!r.simpler_core.stable_parameters[0]);
    CHECK(r.simpler_core.residual_dimensions[0].maximum==9);
    f.observations[1]={2,{9,-6},1,1,1,0,0,0};
    r=analyze(f,b,AnalysisConfig{1,0,2,0});
    // A component cap limits the ranking, not the preservation scope.
    CHECK(!r.simpler_core.stable_parameters[0]);
    bool rejected=false;
    f.observations[0].values[0]=std::numeric_limits<double>::quiet_NaN();
    try { (void)analyze(f,b); } catch(const std::invalid_argument&) { rejected=true; }
    CHECK(rejected);
    f.observations[0].values[0]=1;
    rejected=false;
    try { (void)analyze(f,b,AnalysisConfig{8,2,2,-1}); } catch(const std::invalid_argument&) { rejected=true; }
    CHECK(rejected);
    const auto big=std::numeric_limits<double>::max();
    ComponentFrame same{{big,1},{{10,{big,1},1,1,1,0,0,0},{11,{big,1},1,1,1,0,0,0}}};
    const auto bounded=analyze(same,b);
    CHECK(std::isfinite(bounded.simpler_core.stable_parameters[0].value()));
    CHECK(bounded.simpler_core.stable_parameters[0].value()==big);
    std::cout<<"complete-frame coverage and invalid-input checks passed\n";
}
