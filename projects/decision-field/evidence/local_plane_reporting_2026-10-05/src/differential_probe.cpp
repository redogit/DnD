// Diagnostic only: uses the frozen LocalPlane implementation unchanged.
// This does not implement the user's recurrent controller or its wiring rule.
#include "decision_field/local_plane.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace df::functions;

int main() {
    LocalPlane plane("differential-probe", {"occurrence", "admission", "residual"});
    const std::vector<std::string> all{"occurrence", "admission", "residual"};
    bool first_row = true;
    auto emit = [&](const std::string& name, const std::string& context,
                    const std::string& route, const std::vector<std::string>& satisfied,
                    bool admitted) {
        const auto rb = plane.revision();
        const auto cb = plane.route_count();
        const auto eb = plane.residuals().size();
        if (!first_row) std::cout << ",\n";
        first_row = false;
        try {
            const auto t = plane.observe(context, "representation", route, satisfied, admitted);
            std::cout << "{\"case\":\"" << name << "\",\"occurrence\":" << t.occurrence
                      << ",\"revision_before\":" << rb << ",\"revision_after\":" << plane.revision()
                      << ",\"routes_before\":" << cb << ",\"routes_after\":" << plane.route_count()
                      << ",\"residuals_before\":" << eb << ",\"residuals_after\":" << plane.residuals().size()
                      << ",\"integrated\":" << (t.integrated ? "true" : "false")
                      << ",\"reused\":" << (t.reused ? "true" : "false") << ",\"stages\":[";
            for (std::size_t i = 0; i < t.stages.size(); ++i) {
                if (i) std::cout << ',';
                std::cout << static_cast<unsigned>(t.stages[i]);
            }
            std::cout << "]";
            if (t.residual_index) {
                const auto& r = plane.residuals().at(*t.residual_index);
                std::cout << ",\"residual_occurrence\":" << r.occurrence
                          << ",\"reason\":\"" << r.reason << "\"";
            }
            const auto preserved = plane.resolve("context:A", "representation");
            std::cout << ",\"route_A_preserved\":"
                      << (preserved && *preserved == "route:A" ? "true" : "false") << '}';
        } catch (const std::invalid_argument& e) {
            // The outer diagnostic journal explicitly retains this failed attempt.
            std::cout << "{\"case\":\"" << name << "\",\"exception\":\"" << e.what()
                      << "\",\"revision_before\":" << rb << ",\"revision_after\":" << plane.revision()
                      << ",\"routes_before\":" << cb << ",\"routes_after\":" << plane.route_count()
                      << ",\"residuals_before\":" << eb << ",\"residuals_after\":" << plane.residuals().size()
                      << ",\"failure_preserved_by\":\"outer-diagnostic-only\"}";
        }
    };
    std::cout << "[\n";
    emit("integrate", "context:A", "route:A", all, true);
    emit("reuse", "context:A", "route:A", all, true);
    emit("conflict", "context:A", "route:B", all, true);
    emit("missing_obligation", "context:B", "route:A", {"occurrence", "admission"}, true);
    emit("rejected_execution", "context:C", "route:A", all, false);
    emit("new_context", "context:D", "route:A", all, true);
    emit("invalid_obligation_id", "context:E", "route:A", {""}, true);
    emit("after_invalid", "context:A", "route:A", all, true);
    std::cout << "\n]\n";
}
