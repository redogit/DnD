#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace df::functions {

enum class LocalStage { Receive = 0, Contextualize = 1, Represent = 2, Integrate = 3 };

struct LocalRelation {
    std::string from;
    std::string to;
    std::set<std::string> obligations;
    std::string evidence;
};

struct LocalDiscovery {
    std::string id;
    std::set<std::string> obligations;
    std::vector<LocalRelation> relations;
};

struct LocalPlane {
    std::set<std::string> active_obligations;
    std::map<std::pair<std::string,std::string>, LocalRelation> admitted;
    std::vector<LocalDiscovery> history;
};

struct LocalIntegrationReceipt {
    LocalStage stage{LocalStage::Receive};
    bool admitted{};
    bool reused{};
    std::size_t examined{};
    std::vector<std::string> violations;
    std::vector<LocalRelation> admitted_relations;
};

// Bounded AnyFunctor increment: it knows only the supplied local plane.  It does
// not manufacture global state, erase history, or collapse distinct obligations.
class LocalPlaneAnyFunctor {
    static bool subset(const std::set<std::string>& a,const std::set<std::string>& b) {
        return std::includes(b.begin(),b.end(),a.begin(),a.end());
    }
public:
    [[nodiscard]] LocalIntegrationReceipt integrate(LocalPlane& plane,const LocalDiscovery& discovery) const {
        LocalIntegrationReceipt r;
        if(discovery.id.empty()) {
            r.violations.push_back("receive:missing-discovery-id");
            return r;
        }

        r.stage=LocalStage::Contextualize;
        if(discovery.obligations.empty()) {
            r.violations.push_back("context:no-obligations");
            return r;
        }
        for(const auto& o:discovery.obligations)
            if(!plane.active_obligations.contains(o))
                r.violations.push_back("context:obligation-outside-local-plane:"+o);
        if(!r.violations.empty()) return r;

        r.stage=LocalStage::Represent;
        for(const auto& rel:discovery.relations) {
            ++r.examined;
            if(rel.from.empty()||rel.to.empty()||rel.evidence.empty()) {
                r.violations.push_back("represent:incomplete-relation");
                continue;
            }
            if(!subset(rel.obligations,discovery.obligations))
                r.violations.push_back("represent:relation-obligation-not-declared");
            if(rel.obligations.empty())
                r.violations.push_back("represent:relation-without-obligation");
        }
        if(!r.violations.empty()) return r;

        r.stage=LocalStage::Integrate;
        // Admission is multi-obligation: every obligation declared by the
        // discovery must be supported by at least one represented relation.
        std::set<std::string> covered;
        for(const auto& rel:discovery.relations)
            covered.insert(rel.obligations.begin(),rel.obligations.end());
        if(!subset(discovery.obligations,covered)) {
            for(const auto& o:discovery.obligations)
                if(!covered.contains(o)) r.violations.push_back("integrate:uncovered-obligation:"+o);
            return r;
        }

        for(const auto& rel:discovery.relations) {
            const auto key=std::pair{rel.from,rel.to};
            const auto it=plane.admitted.find(key);
            if(it!=plane.admitted.end()) {
                // Same local path may accumulate obligation coverage, but
                // contradictory evidence is retained as a failure, not overwritten.
                if(it->second.evidence!=rel.evidence) {
                    r.violations.push_back("integrate:evidence-conflict:"+rel.from+"->"+rel.to);
                    return r;
                }
            }
        }

        for(const auto& rel:discovery.relations) {
            const auto key=std::pair{rel.from,rel.to};
            auto [it,inserted]=plane.admitted.emplace(key,rel);
            if(!inserted) {
                const auto before=it->second.obligations.size();
                it->second.obligations.insert(rel.obligations.begin(),rel.obligations.end());
                r.reused=true;
                if(it->second.obligations.size()!=before) r.admitted_relations.push_back(it->second);
            } else r.admitted_relations.push_back(rel);
        }
        plane.history.push_back(discovery);
        r.admitted=true;
        return r;
    }

    [[nodiscard]] std::size_t local_distance(const LocalPlane& plane,
        const std::string& from,const std::string& to,const std::string& obligation) const {
        if(from==to) return 0;
        if(auto it=plane.admitted.find({from,to});
           it!=plane.admitted.end()&&it->second.obligations.contains(obligation)) return 1;

        std::set<std::string> seen{from};
        std::vector<std::string> frontier{from};
        std::size_t distance=0;
        while(!frontier.empty()) {
            ++distance;
            std::vector<std::string> next;
            for(const auto& node:frontier) for(const auto& [key,rel]:plane.admitted) {
                if(key.first!=node||!rel.obligations.contains(obligation)) continue;
                if(key.second==to) return distance;
                if(seen.insert(key.second).second) next.push_back(key.second);
            }
            frontier=std::move(next);
        }
        throw std::out_of_range("no admitted local path for obligation");
    }
};

} // namespace df::functions
