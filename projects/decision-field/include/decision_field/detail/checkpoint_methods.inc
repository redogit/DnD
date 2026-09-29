    // Quiescent snapshot. Executable callbacks are represented by versioned bindings only.
    [[nodiscard]] EngineCheckpoint<T> export_checkpoint() const {
        EngineCheckpoint<T> c;
        c.obligations=obligations_; c.next_state_id=next_state_id_;
        c.next_edge_id=next_edge_id_;c.next_invocation_id=next_invocation_id_;
        c.states=states_;c.edges=edges_;c.invocations=invocations_;c.invocation_by_key=invocation_by_key_;
        c.research=research_;c.dependencies_by_state=dependencies_by_state_;c.dependents_by_key=dependents_by_key_;
        for(const auto& [key,t]:transforms_) c.transform_bindings.emplace(key,TransformBinding{t.id,t.version,t.reversibility});
        validate_checkpoint(c,policy_);
        return c;
    }
    [[nodiscard]] static DecisionFieldEngine restore_checkpoint(DomainPolicy<T> policy,
        const EngineCheckpoint<T>& c,std::span<const TransformDefinition<T>> registry) {
        DecisionFieldEngine out(std::move(policy),c.obligations);
        validate_checkpoint(c,out.policy_);
        for(const auto& [id,binding]:c.transform_bindings) {
            const TransformDefinition<T>* found=nullptr;
            for(const auto& t:registry) if(t.id==id&&t.version==binding.version) {
                checkpoint_require(found==nullptr,"duplicate executable registry binding");found=&t;
            }
            checkpoint_require(found&&found->reversibility==binding.reversibility&&found->forward&&found->reverse,
                               "required executable transform version is unavailable");
            out.register_transform(*found);
        }
        out.next_state_id_=c.next_state_id;out.next_edge_id_=c.next_edge_id;out.next_invocation_id_=c.next_invocation_id;
        out.states_=c.states;out.edges_=c.edges;out.invocations_=c.invocations;out.invocation_by_key_=c.invocation_by_key;
        out.research_=c.research;out.dependencies_by_state_=c.dependencies_by_state;out.dependents_by_key_=c.dependents_by_key;
        return out;
    }
