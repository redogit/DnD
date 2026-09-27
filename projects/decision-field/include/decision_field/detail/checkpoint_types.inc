// Included inside namespace df by the hash-bound generated successor header.
struct TransformBinding {
    std::string id, version;
    Reversibility reversibility{Reversibility::Exact};
};
template<class T> struct EngineCheckpoint {
    ObligationSet obligations;
    StateId next_state_id{1};
    EdgeId next_edge_id{1};
    InvocationId next_invocation_id{1};
    std::unordered_map<StateId,StateNode<T>> states;
    std::unordered_map<EdgeId,TransformEdge<T>> edges;
    std::unordered_map<InvocationId,InvocationRecord<T>> invocations;
    std::unordered_map<std::string,InvocationId> invocation_by_key;
    std::unordered_map<std::string,TransformBinding> transform_bindings;
    SharedResearchState research;
    std::unordered_map<StateId,std::unordered_set<std::string>> dependencies_by_state;
    std::unordered_map<std::string,std::unordered_set<StateId>> dependents_by_key;
};
inline void checkpoint_require(bool value,const char* reason) {
    if(!value) throw std::invalid_argument(reason);
}
template<class Range,class Value> bool checkpoint_has(const Range& r,const Value& v) {
    return std::find(r.begin(),r.end(),v)!=r.end();
}
template<class T> void validate_checkpoint(const EngineCheckpoint<T>& c,const DomainPolicy<T>& p) {
    auto valid_status=[](ExecutionStatus s) {return s>=ExecutionStatus::Succeeded&&s<=ExecutionStatus::Unresolved;};
    auto valid_reversal=[](Reversibility r) {return r==Reversibility::Exact||r==Reversibility::Obligation;};
    checkpoint_require(!c.obligations.id.empty()&&bool(p.fingerprint),"missing checkpoint obligation/policy");
    auto counters=[](const auto& map,std::uint64_t next) {
        checkpoint_require(next>0&&next<std::numeric_limits<std::uint64_t>::max(),"invalid next-ID counter");
        for(const auto& [id,v]:map) { (void)v;checkpoint_require(id>0&&id<next,"counter does not bound native IDs"); }
    };
    counters(c.states,c.next_state_id);counters(c.edges,c.next_edge_id);counters(c.invocations,c.next_invocation_id);
    for(const auto& [id,n]:c.states) {
        checkpoint_require(n.id==id&&n.value&&!n.fingerprint.empty(),"invalid native state identity/value");
        checkpoint_require(p.fingerprint(*n.value)==n.fingerprint,"native value fingerprint mismatch");
        if(n.direction) checkpoint_require(n.direction->sector<4&&n.direction->subdivision<4,"invalid direction address");
        if(n.probe) checkpoint_require(n.probe->parent.sector<4&&n.probe->parent.subdivision<4&&n.probe->outward_index<12,"invalid probe address");
        std::unordered_set<StateId> parents,children;
        for(auto parent:n.parent_states) {
            checkpoint_require(parents.insert(parent).second&&c.states.contains(parent),"duplicate or missing parent");
            const auto& prior=c.states.at(parent);
            checkpoint_require(prior.depth<n.depth&&checkpoint_has(prior.child_states,id),"cyclic, non-increasing or nonreciprocal lineage");
        }
        for(auto child:n.child_states) {
            checkpoint_require(children.insert(child).second&&c.states.contains(child),"duplicate or missing child");
            const auto& next=c.states.at(child);
            checkpoint_require(next.depth>n.depth&&checkpoint_has(next.parent_states,id),"nonreciprocal child lineage");
        }
        for(auto edge:n.parent_edges) checkpoint_require(c.edges.contains(edge)&&checkpoint_has(c.edges.at(edge).to_states,id),"missing producer edge");
        for(auto edge:n.child_edges) checkpoint_require(c.edges.contains(edge)&&checkpoint_has(c.edges.at(edge).from_states,id),"missing consumer edge");
    }
    for(const auto& [id,e]:c.edges) {
        checkpoint_require(e.id==id&&e.invocation_id>0&&!e.transform_id.empty()&&!e.transform_version.empty()&&
            !e.invocation_key.empty()&&valid_reversal(e.reversibility)&&valid_status(e.validation.status),"invalid edge identity/contract");
        checkpoint_require(c.invocations.contains(e.invocation_id)&&c.invocations.at(e.invocation_id).edge_id==id,"orphaned transform invocation");
        for(auto x:e.from_states) {
            checkpoint_require(c.states.contains(x),"edge has missing input");
            // Failed invocations are recorded by v0.3 without installing consumer links.
            if(e.validation.status==ExecutionStatus::Succeeded)
                checkpoint_require(checkpoint_has(c.states.at(x).child_edges,id),"missing successful consumer link");
        }
        std::vector<std::string> key_parts{e.transform_id,e.transform_version,
            e.context.configuration_fingerprint,e.context.captured_entropy_fingerprint};
        for(auto x:e.from_states) key_parts.push_back(c.states.at(x).fingerprint);
        checkpoint_require(e.invocation_key==hash_combine_strings(key_parts),"invocation key does not describe its ordered inputs/context");
        checkpoint_require(e.validation.status==ExecutionStatus::Succeeded||e.to_states.empty(),"failed edge has installed outputs");
        for(auto y:e.to_states) {
            checkpoint_require(c.states.contains(y)&&checkpoint_has(c.states.at(y).parent_edges,id),"missing output producer link");
            for(auto x:e.from_states) checkpoint_require(checkpoint_has(c.states.at(y).parent_states,x),"missing input/output lineage");
        }
    }
    checkpoint_require(c.invocations.size()==c.edges.size()&&c.invocation_by_key.size()==c.invocations.size(),"incomplete invocation/cache indexes");
    for(const auto& [id,i]:c.invocations) {
        checkpoint_require(i.id==id&&!i.key.empty()&&valid_status(i.status)&&c.edges.contains(i.edge_id),"invalid invocation");
        const auto& e=c.edges.at(i.edge_id);
        checkpoint_require(e.invocation_id==id&&e.invocation_key==i.key&&e.to_states==i.outputs&&e.validation.status==i.status,"invocation/edge mismatch");
        checkpoint_require(c.invocation_by_key.contains(i.key)&&c.invocation_by_key.at(i.key)==id,"missing cache binding");
    }
    for(const auto& [key,b]:c.transform_bindings)
        checkpoint_require(key==b.id&&!b.id.empty()&&!b.version.empty()&&valid_reversal(b.reversibility),"invalid registered transform binding");
    for(const auto& [state,keys]:c.dependencies_by_state) {
        checkpoint_require(c.states.contains(state),"dependency names missing state");
        for(const auto& key:keys) checkpoint_require(!key.empty()&&c.dependents_by_key.contains(key)&&c.dependents_by_key.at(key).contains(state),"nonreciprocal dependency");
    }
    for(const auto& [key,states]:c.dependents_by_key) {
        checkpoint_require(!key.empty(),"empty dependency key");
        for(auto state:states) checkpoint_require(c.states.contains(state)&&c.dependencies_by_state.contains(state)&&c.dependencies_by_state.at(state).contains(key),"nonreciprocal dependent");
    }
    for(const auto& [key,f]:c.research.fixes_by_id)
        checkpoint_require(key==f.id&&!key.empty()&&f.status>=FixStatus::LocalTrial&&f.status<=FixStatus::Failed,"invalid shared fix");
    for(const auto& [key,r]:c.research.residuals_by_signature)
        checkpoint_require(key==r.semantic_signature&&!key.empty()&&r.occurrence_count>0,"invalid residual identity/count");
    for(const auto& [key,r]:c.research.concessions_by_signature)
        checkpoint_require(key==r.conflict_signature&&!r.id.empty(),"invalid concession identity");
}
