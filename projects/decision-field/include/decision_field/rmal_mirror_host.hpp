#pragma once
#include "decision_field/anyfunctor_mirror.hpp"
#include "rmal/rmal.h"
#include <cstdlib>
#include <cstring>
#include <limits>
#include <map>

namespace df::functions {
// Host-owned typed bindings. No dynamic loading or script-side registration.
// Host must outlive its VM. Engine and host are quiescent/single-threaded.
// The single symbol AnyFunctor routes invocation and read-only receipt queries.
template<class T> class RmalMirrorHost {
    DecisionFieldEngine<T>& engine_;
    InvocationContext context_;
    std::string execution_space_;
    std::map<std::int64_t,FunctionObject<T>> functions_;
    std::vector<T> recovered_;
    bool bound_{};
    static RmalStatus error(const char* message)noexcept{
        RmalStatus s{};std::snprintf(s.message,sizeof s.message,"%s",message);return s;
    }
    static void text(RmalValue* result,const std::string& value){
        auto* p=static_cast<char*>(std::malloc(value.size()+1));if(!p)throw std::bad_alloc();
        std::memcpy(p,value.c_str(),value.size()+1);result->kind=RMAL_VALUE_STRING;result->as.string=p;
    }
    static void integer(RmalValue* result,std::uint64_t value){
        persistence::wire::require(value<=static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()),"RMAL handle exceeds integer range");
        result->kind=RMAL_VALUE_INT;result->as.integer=static_cast<std::int64_t>(value);
    }
    static const char* status_name(ExecutionStatus status){
        switch(status){case ExecutionStatus::Succeeded:return "SUCCEEDED";case ExecutionStatus::Rejected:return "REJECTED";
        case ExecutionStatus::Failed:return "FAILED";case ExecutionStatus::Unresolved:return "UNRESOLVED";}
        throw std::invalid_argument("invalid admission status");
    }
    static RmalStatus invoke(void* context,const RmalValue* args,std::size_t count,RmalValue* result)noexcept{
        try{
            if(!context||!result||count!=3||!args||args[0].kind!=RMAL_VALUE_STRING||!args[0].as.string||
               args[1].kind!=RMAL_VALUE_INT||args[2].kind!=RMAL_VALUE_INT||args[1].as.integer<=0||args[2].as.integer<=0)
                return error("AnyFunctor requires (operation string, positive function handle, positive source/receipt handle)");
            auto& host=*static_cast<RmalMirrorHost*>(context);
            const auto it=host.functions_.find(args[1].as.integer);
            if(it==host.functions_.end())return error("unbound FunctionObject handle");
            const auto& function=it->second;const auto id=static_cast<std::uint64_t>(args[2].as.integer);
            const std::string_view op=args[0].as.string;
            if(op=="mirror"){
                const auto receipt=AnyFunctor(host.engine_,AnyInvocation<T>{function,{id},host.context_,host.execution_space_});
                integer(result,receipt.invocation_id);
            }else if(op=="status"||op=="output"||op=="homeward"){
                const auto& record=host.engine_.invocation_record(id);
                const auto receipt=function.receipt(record,host.engine_.edge(record.edge_id));
                if(op=="status")text(result,status_name(receipt.admission));
                else if(op=="output"){
                    persistence::wire::require(receipt.admission==ExecutionStatus::Succeeded&&receipt.admitted.size()==1,"receipt has no admitted output");
                    integer(result,receipt.admitted[0]);
                }else{
                    auto recovered=Homeward(host.engine_,function,id);
                    persistence::wire::require(recovered.size()==1,"Mirror Homeward requires one reconstructed value");
                    const auto digest=persistence::archive_sha256(function.codec().encode(recovered[0]));
                    host.recovered_=std::move(recovered);text(result,digest);
                }
            }else return error("unsupported AnyFunctor operation");
            RmalStatus s{};s.ok=true;return s;
        }catch(const std::exception& ex){return error(ex.what());}
        catch(...){return error("native Mirror bridge failed");}
    }
public:
    RmalMirrorHost(DecisionFieldEngine<T>& engine,InvocationContext context,std::string space)
      :engine_(engine),context_(std::move(context)),execution_space_(std::move(space)){
        persistence::wire::require(!execution_space_.empty(),"execution space is required");
    }
    RmalMirrorHost(const RmalMirrorHost&)=delete;RmalMirrorHost& operator=(const RmalMirrorHost&)=delete;
    void add(std::int64_t handle,FunctionObject<T> function){
        persistence::wire::require(!bound_&&handle>0&&functions_.size()<64,"function registration closed or out of bounds");
        persistence::wire::require(functions_.emplace(handle,std::move(function)).second,"duplicate FunctionObject handle");
    }
    RmalStatus bind(RmalVm* vm){
        if(bound_)return error("Mirror host already bound");
        auto status=rmal_vm_bind_native(vm,"AnyFunctor",3,&invoke,this);if(status.ok)bound_=true;return status;
    }
    [[nodiscard]] const std::vector<T>& last_recovered()const noexcept{return recovered_;}
};
} // namespace df::functions
