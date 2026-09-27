#pragma once
#include "decision_field/component_adapter.hpp"
#include <functional>
#include <unordered_set>

namespace df::bridge {
struct CoordinateOverride {std::size_t dimension{}; double value{};};
struct EncodedRow {StateKey state_id{}; std::vector<CoordinateOverride> overrides;};
struct CorePacket {
    RunContext context;
    component::SimplerCore summary;
    std::size_t dimensions{}, source_count{};
    double tolerance{};
    std::vector<EncodedRow> rows;
};
struct DecodedRow {StateKey state_id{}; std::vector<double> values;};
enum class Verdict {Pass,Fail,Unknown};
enum class Disposition {Keep,Repair,Unresolved};
enum class ReplayMode {ExactProjection,ObligationProjection};
struct ObligationOracle {
    std::string id,version;
    std::vector<std::string> obligation_ids;
    std::function<std::vector<Verdict>(std::span<const double>)> evaluate;
};
struct CaseReceipt {
    StateKey state_id{};
    bool exact_projection_match{};
    std::vector<Verdict> original,reconstructed;
};
struct ReplayReport {
    Disposition disposition{Disposition::Unresolved};
    RunContext context;
    std::string oracle_id,oracle_version;
    std::size_t rows_checked{},preserved_negative_checks{};
    bool all_obligations_pass{false};
    std::vector<CaseReceipt> cases;
    std::vector<std::string> findings;
};

inline CorePacket make_core_packet(const ObservedFrame& source,double tolerance=0.0) {
    detail::validate_frame(source);
    detail::require(std::isfinite(tolerance) && tolerance>=0,"invalid core tolerance");
    CorePacket packet;
    packet.context=source.context; packet.dimensions=source.frame.core.size();
    packet.source_count=source.frame.observations.size(); packet.tolerance=tolerance;
    packet.summary.stable_parameters.resize(packet.dimensions);
    if(packet.source_count==0) return packet; // no evidence; replay must remain unresolved
    for(std::size_t d=0;d<packet.dimensions;++d) {
        double lo=source.frame.observations.front().values[d],hi=lo;
        for(const auto& o:source.frame.observations) {lo=std::min(lo,o.values[d]);hi=std::max(hi,o.values[d]);}
        if(hi-lo<=tolerance) {
            // Use an observed value, not an invented average. Every non-identical bit
            // pattern (including signed zero) still gets its own exact override.
            packet.summary.stable_parameters[d]=source.frame.observations.front().values[d];
        } else packet.summary.residual_dimensions.push_back({d,lo,hi});
    }
    for(const auto& o:source.frame.observations) {
        EncodedRow row{o.state_id,{}};
        for(std::size_t d=0;d<packet.dimensions;++d) {
            const auto& shared=packet.summary.stable_parameters[d];
            if(!shared || std::bit_cast<std::uint64_t>(*shared)!=std::bit_cast<std::uint64_t>(o.values[d]))
                row.overrides.push_back({d,o.values[d]});
        }
        packet.rows.push_back(std::move(row));
    }
    return packet;
}

// The decoder has NO access to the source frame or original values.
inline std::vector<DecodedRow> reconstruct(const CorePacket& packet) {
    detail::validate_context(packet.context);
    detail::require(packet.dimensions>0 && packet.summary.stable_parameters.size()==packet.dimensions &&
                    packet.source_count==packet.rows.size(),"malformed recovery packet");
    detail::require(std::isfinite(packet.tolerance) && packet.tolerance>=0,"invalid packet tolerance");
    std::unordered_set<StateKey> ids;
    std::vector<DecodedRow> out;
    for(const auto& row:packet.rows) {
        detail::require(row.state_id!=0 && ids.insert(row.state_id).second,"duplicate/missing recovery identity");
        auto values=packet.summary.stable_parameters;
        std::vector<bool> overridden(packet.dimensions,false);
        for(const auto& v:row.overrides) {
            detail::require(v.dimension<packet.dimensions && !overridden[v.dimension] && std::isfinite(v.value),
                            "invalid or duplicate coordinate override");
            overridden[v.dimension]=true; values[v.dimension]=v.value;
        }
        DecodedRow decoded{row.state_id,{}};
        for(const auto& v:values) {
            detail::require(v.has_value() && std::isfinite(*v),"recovery packet lost a required coordinate");
            decoded.values.push_back(*v);
        }
        out.push_back(std::move(decoded));
    }
    // The summary is itself a claim. Exact row recovery does not excuse a false
    // claim of shared stability or an altered range descriptor.
    if (!out.empty()) {
        std::vector<const component::CoreResidualDimension*> ranges(packet.dimensions,nullptr);
        for (const auto& r : packet.summary.residual_dimensions) {
            detail::require(r.dimension<packet.dimensions && !ranges[r.dimension] &&
                            std::isfinite(r.minimum) && std::isfinite(r.maximum) && r.minimum<=r.maximum,
                            "invalid residual range descriptor");
            ranges[r.dimension]=&r;
        }
        for (std::size_t d=0; d<packet.dimensions; ++d) {
            double lo=out.front().values[d],hi=lo;
            for (const auto& row : out) {lo=std::min(lo,row.values[d]);hi=std::max(hi,row.values[d]);}
            const auto& shared=packet.summary.stable_parameters[d];
            if (shared) {
                detail::require(!ranges[d] && hi-lo<=packet.tolerance && *shared>=lo && *shared<=hi,
                                "claimed shared coordinate is not supported by all recovered rows");
            } else {
                detail::require(ranges[d] && ranges[d]->minimum==lo && ranges[d]->maximum==hi,
                                "residual range does not describe recovered rows");
            }
        }
    }
    return out;
}

inline ReplayReport replay_core(const ObservedFrame& original,const CorePacket& packet,
                                const ObligationOracle& oracle,ReplayMode mode=ReplayMode::ExactProjection) {
    ReplayReport report;
    report.context=original.context; report.oracle_id=oracle.id; report.oracle_version=oracle.version;
    try {detail::validate_frame(original);} catch(const std::exception& e) {report.findings.push_back(e.what());return report;}
    if(packet.context!=original.context) {report.findings.push_back("scope changed: no evidence transfer");return report;}
    std::unordered_set<std::string> obligation_ids;
    if(oracle.id.empty() || oracle.version.empty() || !oracle.evaluate || oracle.obligation_ids.empty()) {
        report.findings.push_back("missing obligation oracle");return report;
    }
    for(const auto& id:oracle.obligation_ids) if(id.empty() || !obligation_ids.insert(id).second) {
        report.findings.push_back("invalid oracle obligation identities");return report;
    }
    std::vector<DecodedRow> decoded;
    try {decoded=reconstruct(packet);} catch(const std::exception& e) {
        report.disposition=Disposition::Repair;report.findings.push_back(e.what());return report;
    }
    if(decoded.size()!=original.frame.observations.size()) {
        report.disposition=Disposition::Repair;report.findings.push_back("source coverage changed");return report;
    }
    bool mismatch=false,unknown=false,all_pass=true;
    for(std::size_t i=0;i<decoded.size();++i) {
        const auto& prior=original.frame.observations[i]; const auto& row=decoded[i];
        if(prior.state_id!=row.state_id || prior.values.size()!=row.values.size()) {
            mismatch=true;report.findings.push_back("source identity or dimensionality changed");continue;
        }
        CaseReceipt receipt{row.state_id,detail::same_bits(prior.values,row.values),{}, {}};
        if(mode==ReplayMode::ExactProjection && !receipt.exact_projection_match) mismatch=true;
        try {
            receipt.original=oracle.evaluate(prior.values);
            receipt.reconstructed=oracle.evaluate(row.values);
            const auto a=oracle.evaluate(prior.values),b=oracle.evaluate(row.values);
            if(receipt.original.size()!=oracle.obligation_ids.size() || receipt.reconstructed.size()!=oracle.obligation_ids.size() ||
               a!=receipt.original || b!=receipt.reconstructed) {
                unknown=true;all_pass=false;report.findings.push_back("incomplete or unstable oracle result");
            } else {
                for(std::size_t j=0;j<receipt.original.size();++j) {
                    const auto x=receipt.original[j],y=receipt.reconstructed[j];
                    const auto valid=[](Verdict v) {return v==Verdict::Pass || v==Verdict::Fail || v==Verdict::Unknown;};
                    if(!valid(x) || !valid(y) || x==Verdict::Unknown || y==Verdict::Unknown) unknown=true;
                    else if(x!=y) mismatch=true;
                    if(x==Verdict::Fail && y==Verdict::Fail) ++report.preserved_negative_checks;
                    if(x!=Verdict::Pass || y!=Verdict::Pass) all_pass=false;
                }
            }
        } catch(const std::exception& e) {unknown=true;all_pass=false;report.findings.push_back(e.what());}
        ++report.rows_checked;
        report.cases.push_back(std::move(receipt));
    }
    report.all_obligations_pass=all_pass && !decoded.empty() && !unknown && !mismatch;
    report.disposition=mismatch?Disposition::Repair:(unknown || decoded.empty()?Disposition::Unresolved:Disposition::Keep);
    if(mismatch) report.findings.push_back("candidate changed an obligated result, identity or required exact projection");
    if(report.disposition==Disposition::Keep) report.findings.push_back("replay equivalent within this finite frozen scope; not universal admission");
    return report;
}
} // namespace df::bridge
