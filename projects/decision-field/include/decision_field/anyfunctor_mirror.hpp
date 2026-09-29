#pragma once
#include "decision_field/native_archive.hpp"

namespace df::functions {
// A versioned, bounded receipt carried by the existing native invocation archive.
// It is not an authentication token or a proof. Rejected outputs are retained here,
// never installed as admitted engine states. The frozen v0.3 source is unchanged.
template<class T> struct AttackReceipt {
    InvocationId invocation_id{};
    EdgeId edge_id{};
    ExecutionStatus computation{ExecutionStatus::Unresolved};
    ExecutionStatus admission{ExecutionStatus::Unresolved};
    std::vector<StateId> sources, admitted;
    std::vector<T> retained_candidates;
    std::vector<std::string> violations, notes;
    std::string recovery_receipt;
    bool memoized{};
};

template<class T> class FunctionObject {
    struct Contract {
        std::string id, policy_identity;
        MirrorAxisState axes;
        ObligationSet obligations;
        DomainPolicy<T> policy;
        persistence::NativeCodec<T> codec;
    };
    struct Saved {
        std::string schema{"DF-MIRROR-RECEIPT/1"}, function, input_digest;
        ExecutionStatus computation{ExecutionStatus::Unresolved}, admission{ExecutionStatus::Unresolved};
        std::vector<std::string> candidates, violations, notes;
    };
    std::shared_ptr<const Contract> c_;
    explicit FunctionObject(std::shared_ptr<const Contract> c):c_(std::move(c)){}
    static std::string save(const Saved& s,const Contract& c){
        persistence::wire::Writer<T> w(c.codec,{});
        w(s.schema,s.function,s.input_digest,s.computation,s.admission,s.candidates,s.violations,s.notes);
        return w.data;
    }
    static Saved load(std::string_view bytes,const Contract& c){
        persistence::wire::require(bytes.size()<=8U*1024U*1024U,"mirror receipt exceeds limit");
        Saved s;persistence::wire::Reader<T> r(bytes,c.codec,{});
        r(s.schema,s.function,s.input_digest,s.computation,s.admission,s.candidates,s.violations,s.notes);
        persistence::wire::require(r.remaining()==0&&s.schema=="DF-MIRROR-RECEIPT/1"&&s.function==c.id,"mirror receipt identity mismatch");
        auto valid=[](ExecutionStatus x){return x>=ExecutionStatus::Succeeded&&x<=ExecutionStatus::Unresolved;};
        persistence::wire::require(valid(s.computation)&&valid(s.admission),"invalid mirror status");
        persistence::wire::require(s.candidates.size()<=1,"invalid mirror candidate arity");
        persistence::wire::require(save(s,c)==bytes,"noncanonical mirror receipt");
        return s;
    }
    static TransformExecution<T> generate_and_verify(const Contract& c,std::span<const T> in){
        Saved saved;saved.function=c.id;
        TransformExecution<T> result;
        try{
            if(in.size()!=1)throw std::invalid_argument("Mirror requires one source");
            saved.input_digest=persistence::archive_sha256(c.codec.encode(in[0]));
            const auto source_check=c.policy.validate(in[0],c.obligations);
            const T candidate=c.policy.mirror(in[0],c.axes,c.obligations);
            saved.computation=ExecutionStatus::Succeeded;
            const auto bytes=c.codec.encode(candidate);
            persistence::wire::require(c.codec.exact_equal(candidate,c.codec.decode(bytes)),"Mirror codec does not preserve candidate");
            saved.candidates.push_back(bytes);
            const auto check=c.policy.validate(candidate,c.obligations);
            saved.violations=source_check.violated_obligations;
            saved.violations.insert(saved.violations.end(),check.violated_obligations.begin(),check.violated_obligations.end());
            saved.notes=source_check.notes;saved.notes.insert(saved.notes.end(),check.notes.begin(),check.notes.end());
            const auto back=c.policy.mirror(candidate,c.axes,c.obligations);
            if(!c.codec.exact_equal(back,in[0]))saved.violations.push_back("mirror:not-involutive");
            const bool valid=source_check.status==ExecutionStatus::Succeeded&&check.status==ExecutionStatus::Succeeded&&saved.violations.empty();
            saved.admission=valid?ExecutionStatus::Succeeded:
                (source_check.status==ExecutionStatus::Unresolved||check.status==ExecutionStatus::Unresolved)?ExecutionStatus::Unresolved:ExecutionStatus::Rejected;
            if(valid)result.outputs.push_back(candidate);
        }catch(const std::exception& ex){
            saved.admission=ExecutionStatus::Failed;
            if(saved.computation!=ExecutionStatus::Succeeded)saved.computation=ExecutionStatus::Failed;
            saved.notes.push_back(ex.what());
        }
        result.status=saved.admission;
        result.notes=saved.notes;
        result.notes.insert(result.notes.end(),saved.violations.begin(),saved.violations.end());
        result.recovery_receipt=save(saved,c);
        return result;
    }
public:
    static FunctionObject mirror(std::string policy_identity,MirrorAxisState axes,ObligationSet obligations,
                                  DomainPolicy<T> policy,persistence::NativeCodec<T> codec){
        persistence::wire::require(!policy_identity.empty()&&!obligations.id.empty()&&policy.mirror&&policy.validate&&
            codec.encode&&codec.decode&&codec.exact_equal&&!codec.id.empty()&&!codec.version.empty(),"incomplete Mirror contract");
        // Full declarations, not only an axis version, contribute to function identity.
        persistence::wire::Writer<T> w(codec,{});
        const std::vector<std::string> labels(axes.boundary_labels.begin(),axes.boundary_labels.end());
        w(std::string("AnyFunctor/Mirror/1"),policy_identity,obligations,codec.id,codec.version,
          axes.version,labels,axes.reflected_relations,axes.preserved_relations,axes.previous_version,axes.evidence_refs);
        const auto id=std::string("AnyFunctor:Mirror:")+persistence::archive_sha256(w.data);
        return FunctionObject(std::make_shared<const Contract>(Contract{id,std::move(policy_identity),std::move(axes),
            std::move(obligations),std::move(policy),std::move(codec)}));
    }
    [[nodiscard]] const std::string& id()const{return c_->id;}
    [[nodiscard]] const ObligationSet& obligations()const{return c_->obligations;}
    [[nodiscard]] const persistence::NativeCodec<T>& codec()const{return c_->codec;}
    [[nodiscard]] TransformDefinition<T> binding()const{
        const auto c=c_;
        return {c->id,"1",Reversibility::Exact,
            [c](std::span<const T> in,const InvocationContext&){return generate_and_verify(*c,in);},
            [c](std::span<const T> out,const InvocationContext&,std::string_view bytes){
                const auto s=load(bytes,*c);
                persistence::wire::require(s.admission==ExecutionStatus::Succeeded&&out.size()==1&&s.candidates.size()==1,"Mirror has no admitted inverse");
                persistence::wire::require(c->codec.encode(out[0])==s.candidates[0],"Mirror output does not match receipt");
                T back=c->policy.mirror(out[0],c->axes,c->obligations);
                persistence::wire::require(persistence::archive_sha256(c->codec.encode(back))==s.input_digest,"Mirror inverse digest mismatch");
                return TransformExecution<T>{ExecutionStatus::Succeeded,{std::move(back)},{"Homeward exact codec check"},std::string(bytes)};
            }};
    }
    [[nodiscard]] AttackReceipt<T> receipt(const InvocationRecord<T>& record,const TransformEdge<T>& edge)const{
        const auto s=load(edge.recovery_receipt,*c_);
        persistence::wire::require(edge.transform_id==c_->id&&edge.transform_version=="1"&&record.status==s.admission&&
            record.edge_id==edge.id&&edge.invocation_id==record.id,"Mirror invocation/receipt mismatch");
        AttackReceipt<T> r;r.invocation_id=record.id;r.edge_id=record.edge_id;r.computation=s.computation;
        r.admission=s.admission;r.sources=edge.from_states;r.admitted=record.outputs;
        for(const auto& bytes:s.candidates){
            auto v=c_->codec.decode(bytes);persistence::wire::require(c_->codec.encode(v)==bytes,"noncanonical retained candidate");
            r.retained_candidates.push_back(std::move(v));
        }
        r.violations=s.violations;r.notes=s.notes;r.recovery_receipt=edge.recovery_receipt;r.memoized=record.memoized_reuse;
        persistence::wire::require(r.admission==ExecutionStatus::Succeeded||r.admitted.empty(),"rejected candidate was admitted");
        return r;
    }
};

template<class T> struct AnyInvocation {
    FunctionObject<T> function;
    std::vector<StateId> sources;
    InvocationContext context;
    std::string execution_space;
};

template<class T> AttackReceipt<T> AnyFunctor(DecisionFieldEngine<T>& engine,const AnyInvocation<T>& in){
    persistence::wire::require(in.sources.size()==1&&!in.execution_space.empty(),"Mirror invocation needs source and execution space");
    persistence::wire::Writer<T> expected(in.function.codec(),{}),actual(in.function.codec(),{});
    expected(in.function.obligations());actual(engine.invocation_obligations());
    persistence::wire::require(expected.data==actual.data,"FunctionObject obligation does not match engine");
    persistence::wire::Writer<T> key(in.function.codec(),{});
    key(std::string("AnyInvocation/Mirror/1"),in.execution_space,in.function.id(),in.context,in.sources);
    for(const auto id:in.sources){
        const auto& s=engine.state(id);
        persistence::wire::require(!s.stale,"stale source requires repair before Mirror");
        key(in.function.codec().encode(*s.value));
    }
    InvocationContext context{persistence::archive_sha256(key.data),in.context.captured_entropy_fingerprint};
    if(!engine.has_transform(in.function.id()))engine.register_transform(in.function.binding());
    const auto record=engine.execute(in.function.id(),in.sources,context);
    return in.function.receipt(record,engine.edge(record.edge_id));
}

template<class T> std::vector<T> Homeward(const DecisionFieldEngine<T>& engine,const FunctionObject<T>& f,InvocationId id){
    const auto& record=engine.invocation_record(id);const auto& edge=engine.edge(record.edge_id);
    const auto receipt=f.receipt(record,edge);
    persistence::wire::require(receipt.admission==ExecutionStatus::Succeeded,"rejected candidate cannot be used as an admitted inverse");
    std::vector<T> outputs;for(auto sid:edge.to_states)outputs.push_back(*engine.state(sid).value);
    auto back=f.binding().reverse(outputs,edge.context,edge.recovery_receipt);
    persistence::wire::require(back.status==ExecutionStatus::Succeeded&&back.outputs.size()==edge.from_states.size(),"Homeward failed");
    return back.outputs;
}
} // namespace df::functions
