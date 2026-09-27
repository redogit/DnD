#pragma once
#include "decision_field/native_checkpoint_available.hpp"
#include <bit>
#include <climits>
#include <cstring>
#include <type_traits>
#include <openssl/evp.h>

namespace df::persistence {
// Limits apply before allocating variable-length fields. Not an exact RAM budget.
struct ArchiveLimits {
    std::size_t max_archive_bytes{128U*1024U*1024U};
    std::size_t max_string_bytes{8U*1024U*1024U};
    std::size_t max_collection_entries{4000000};
};
struct ArchiveIdentity {
    std::string run_id,policy_id,policy_version,codec_id,codec_version,parent_digest;
    friend bool operator==(const ArchiveIdentity&,const ArchiveIdentity&)=default;
};
template<class T> struct NativeCodec {
    std::string id,version;
    std::function<std::string(const T&)> encode;
    std::function<T(std::string_view)> decode;
    std::function<bool(const T&,const T&)> exact_equal;
};
inline std::string sha256_bytes(std::string_view bytes) {
    std::array<unsigned char,EVP_MAX_MD_SIZE> out{};std::size_t size=0;
    if(EVP_Q_digest(nullptr,"SHA256",nullptr,bytes.data(),bytes.size(),out.data(),&size)!=1||size!=32)
        throw std::runtime_error("OpenSSL SHA-256 failed");
    return {reinterpret_cast<const char*>(out.data()),size};
}
inline std::string archive_sha256(std::string_view bytes) {
    const auto digest=sha256_bytes(bytes);std::string hex;hex.reserve(64);
    for(char raw:digest) {const auto c=static_cast<unsigned char>(raw);hex.push_back("0123456789abcdef"[c>>4]);hex.push_back("0123456789abcdef"[c&15]);}
    return hex;
}
namespace wire {
inline void require(bool b,const char* message) {if(!b)throw std::invalid_argument(message);}
template<class> struct tag {};
// Explicit fields, not object memory. Vector order survives; unordered keys are canonicalized.
#define DF_FIELDS(TYPE,...) template<class A,class X> void fields(A& a,X& x,tag<TYPE>){a(__VA_ARGS__);}
DF_FIELDS(ArchiveIdentity,x.run_id,x.policy_id,x.policy_version,x.codec_id,x.codec_version,x.parent_digest)
DF_FIELDS(Obligation,x.id,x.description,x.mandatory)
DF_FIELDS(ObligationSet,x.id,x.items)
DF_FIELDS(DirectionAddress,x.sector,x.subdivision)
DF_FIELDS(ProbeAddress,x.parent,x.outward_index)
DF_FIELDS(ValidationReceipt,x.status,x.satisfied_obligations,x.violated_obligations,x.notes)
DF_FIELDS(InvocationContext,x.configuration_fingerprint,x.captured_entropy_fingerprint)
DF_FIELDS(TransformBinding,x.id,x.version,x.reversibility)
DF_FIELDS(FixTrialMetrics,x.mandatory_pass,x.unresolved_residual,x.complexity,x.recovery_cost,x.decay_risk,x.transferability)
DF_FIELDS(SharedFix,x.id,x.problem_signature,x.applicable_workers,x.introduced_round,x.completed_sync_rounds,x.status,x.evidence,x.counter_evidence,x.round_metrics)
DF_FIELDS(ResidualNote,x.semantic_signature,x.smallest_known_difference,x.relevant_obligations,x.representative_examples,x.evidence_refs,x.counter_evidence_refs,x.occurrence_count)
DF_FIELDS(ConcessionRecord,x.id,x.conflict_signature,x.applicable_workers,x.evidence,x.remaining_disagreements)
DF_FIELDS(SharedResearchState,x.fixes_by_id,x.residuals_by_signature,x.concessions_by_signature,x.counterexamples,x.evidence)
#undef DF_FIELDS

template<class A,class X,class T> void fields(A& a,X& x,tag<StateNode<T>>) {
    a(x.id,x.value,x.fingerprint,x.parent_edges,x.child_edges,x.parent_states,x.child_states,x.stale,x.direction,x.probe,x.depth,x.provenance);
}
template<class A,class X,class T> void fields(A& a,X& x,tag<TransformEdge<T>>) {
    a(x.id,x.invocation_id,x.transform_id,x.transform_version,x.reversibility,x.from_states,x.to_states,x.invocation_key,x.context,x.validation,x.recovery_receipt);
}
template<class A,class X,class T> void fields(A& a,X& x,tag<InvocationRecord<T>>) {
    a(x.id,x.key,x.edge_id,x.status,x.outputs,x.notes,x.memoized_reuse);
}
template<class A,class X,class T> void fields(A& a,X& x,tag<EngineCheckpoint<T>>) {
    a(x.obligations,x.next_state_id,x.next_edge_id,x.next_invocation_id,x.states,x.edges,x.invocations,x.invocation_by_key,
      x.transform_bindings,x.research,x.dependencies_by_state,x.dependents_by_key);
}
template<class> inline constexpr bool is_vector=false;
template<class V,class A> inline constexpr bool is_vector<std::vector<V,A>> = true;
template<class> inline constexpr bool is_map=false;
template<class K,class V,class H,class E,class A> inline constexpr bool is_map<std::unordered_map<K,V,H,E,A>> = true;
template<class> inline constexpr bool is_set=false;
template<class K,class H,class E,class A> inline constexpr bool is_set<std::unordered_set<K,H,E,A>> = true;
template<class> inline constexpr bool is_optional=false;
template<class V> inline constexpr bool is_optional<std::optional<V>> = true;

inline void append_u64(std::string& out,std::uint64_t v) {
    for(unsigned j=0;j<8;++j) out.push_back(static_cast<char>((v>>(8*j))&255U));
}
inline std::uint64_t take_u64(std::string_view in,std::size_t& pos) {
    require(pos<=in.size()&&in.size()-pos>=8,"truncated integer");std::uint64_t v=0;
    for(unsigned j=0;j<8;++j) v|=std::uint64_t(static_cast<unsigned char>(in[pos++]))<<(8*j);
    return v;
}
template<class T> class Writer {
    const NativeCodec<T>& codec_;ArchiveLimits limits_;std::size_t entries_{};
    void room(std::size_t n) {require(n<=limits_.max_archive_bytes&&data.size()<=limits_.max_archive_bytes-n,"archive byte limit exceeded");}
    void count(std::size_t n) {
        require(n<=limits_.max_collection_entries&&entries_<=limits_.max_collection_entries-n,"archive collection limit exceeded");
        entries_+=n;integer(n);
    }
    void integer(std::uint64_t n) {room(8);append_u64(data,n);}
public:
    std::string data;
    Writer(const NativeCodec<T>& codec,ArchiveLimits limits):codec_(codec),limits_(limits){}
    template<class... V> void operator()(const V&... v) {(one(v),...);}
    template<class V> void one(const V& v) {
        using U=std::remove_cvref_t<V>;
        if constexpr(std::is_same_v<U,bool>) {room(1);data.push_back(v?'\1':'\0');}
        else if constexpr(std::is_enum_v<U>) one(static_cast<std::underlying_type_t<U>>(v));
        else if constexpr(std::is_integral_v<U>) integer(static_cast<std::uint64_t>(v));
        else if constexpr(std::is_same_v<U,double>) {
            static_assert(sizeof(double)==8&&std::numeric_limits<double>::is_iec559);
            integer(std::bit_cast<std::uint64_t>(v));
        } else if constexpr(std::is_same_v<U,std::string>) {
            require(v.size()<=limits_.max_string_bytes,"archive string limit exceeded");integer(v.size());room(v.size());data+=v;
        } else if constexpr(std::is_same_v<U,std::shared_ptr<const T>>) {
            require(bool(v),"null native value");const auto bytes=codec_.encode(*v);const auto back=codec_.decode(bytes);
            require(codec_.exact_equal(*v,back)&&codec_.encode(back)==bytes,"native codec is not exactly reversible/canonical");one(bytes);
        } else if constexpr(is_optional<U>) {one(v.has_value());if(v)one(*v);}
        else if constexpr(is_vector<U>) {count(v.size());for(const auto& item:v)one(item);}
        else if constexpr(is_map<U>) {
            count(v.size());std::vector<typename U::key_type> keys;keys.reserve(v.size());
            for(const auto& [key,item]:v){(void)item;keys.push_back(key);}std::sort(keys.begin(),keys.end());
            for(const auto& key:keys){one(key);one(v.at(key));}
        } else if constexpr(is_set<U>) {
            count(v.size());std::vector<typename U::value_type> values(v.begin(),v.end());std::sort(values.begin(),values.end());
            for(const auto& item:values)one(item);
        } else fields(*this,v,tag<U>{});
    }
};
template<class T> class Reader {
    const NativeCodec<T>& codec_;ArchiveLimits limits_;std::size_t entries_{};
    std::size_t count() {
        const auto n=integer();require(n<=limits_.max_collection_entries&&entries_<=limits_.max_collection_entries-n,"archive collection limit exceeded");
        require(n<=remaining(),"collection count exceeds remaining bytes");entries_+=static_cast<std::size_t>(n);return static_cast<std::size_t>(n);
    }
    std::uint64_t integer(){return take_u64(data,pos);}
public:
    std::string_view data;std::size_t pos{};
    Reader(std::string_view bytes,const NativeCodec<T>& codec,ArchiveLimits limits):codec_(codec),limits_(limits),data(bytes){}
    std::size_t remaining()const{return data.size()-pos;}
    template<class... V> void operator()(V&... v){(one(v),...);}
    template<class V> void one(V& v) {
        using U=std::remove_cvref_t<V>;
        if constexpr(std::is_same_v<U,bool>){require(remaining()>=1,"truncated bool");const auto b=static_cast<unsigned char>(data[pos++]);require(b<=1,"invalid bool");v=b!=0;}
        else if constexpr(std::is_enum_v<U>){std::underlying_type_t<U> n{};one(n);v=static_cast<U>(n);}
        else if constexpr(std::is_integral_v<U>){
            const auto n=integer();
            if constexpr(std::is_signed_v<U>){const auto s=std::bit_cast<std::int64_t>(n);require(s>=std::numeric_limits<U>::min()&&s<=std::numeric_limits<U>::max(),"signed integer out of range");v=static_cast<U>(s);}
            else {require(n<=std::numeric_limits<U>::max(),"unsigned integer out of range");v=static_cast<U>(n);}
        } else if constexpr(std::is_same_v<U,double>){v=std::bit_cast<double>(integer());}
        else if constexpr(std::is_same_v<U,std::string>){const auto n=integer();require(n<=limits_.max_string_bytes&&n<=remaining(),"invalid string length");v.assign(data.substr(pos,static_cast<std::size_t>(n)));pos+=static_cast<std::size_t>(n);}
        else if constexpr(std::is_same_v<U,std::shared_ptr<const T>>){std::string bytes;one(bytes);T value=codec_.decode(bytes);require(codec_.encode(value)==bytes,"noncanonical native value");v=std::make_shared<const T>(std::move(value));}
        else if constexpr(is_optional<U>){bool has{};one(has);if(has){typename U::value_type item{};one(item);v=std::move(item);}else v.reset();}
        else if constexpr(is_vector<U>){const auto n=count();v.clear();for(std::size_t i=0;i<n;++i){typename U::value_type item{};one(item);v.push_back(std::move(item));}}
        else if constexpr(is_map<U>){
            const auto n=count();v.clear();std::optional<typename U::key_type> previous;
            for(std::size_t i=0;i<n;++i){typename U::key_type key{};typename U::mapped_type item{};one(key);
                require(!previous||*previous<key,"duplicate or noncanonical map key");previous=key;one(item);v.emplace(std::move(key),std::move(item));}
        } else if constexpr(is_set<U>){
            const auto n=count();v.clear();std::optional<typename U::value_type> previous;
            for(std::size_t i=0;i<n;++i){typename U::value_type item{};one(item);require(!previous||*previous<item,"duplicate or noncanonical set value");previous=item;v.insert(std::move(item));}
        } else fields(*this,v,tag<U>{});
    }
};
template<class T> void validate_identity(const ArchiveIdentity& i,const NativeCodec<T>& codec) {
    require(!i.run_id.empty()&&!i.policy_id.empty()&&!i.policy_version.empty()&&!codec.id.empty()&&!codec.version.empty()&&
        i.codec_id==codec.id&&i.codec_version==codec.version&&codec.encode&&codec.decode&&codec.exact_equal,"missing/mismatched archive identity or native codec");
    require(i.parent_digest.empty()||(i.parent_digest.size()==64&&std::all_of(i.parent_digest.begin(),i.parent_digest.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');})),"invalid predecessor digest");
}
} // namespace wire

template<class T> std::string pack_native(const EngineCheckpoint<T>& checkpoint,const ArchiveIdentity& identity,
    const NativeCodec<T>& codec,const DomainPolicy<T>& policy,ArchiveLimits limits={}) {
    wire::validate_identity(identity,codec);validate_checkpoint(checkpoint,policy);
    wire::Writer<T> writer(codec,limits);writer(identity,checkpoint);
    wire::require(limits.max_archive_bytes>=56&&writer.data.size()<=limits.max_archive_bytes-56,"archive envelope exceeds byte limit");
    std::string out="DFNAT001";wire::append_u64(out,1);wire::append_u64(out,writer.data.size());out+=writer.data;out+=sha256_bytes(out);
    return out;
}
template<class T> EngineCheckpoint<T> unpack_native(std::string_view bytes,const ArchiveIdentity& expected,
    const NativeCodec<T>& codec,const DomainPolicy<T>& policy,ArchiveLimits limits={}) {
    wire::validate_identity(expected,codec);
    wire::require(bytes.size()>=56&&bytes.size()<=limits.max_archive_bytes,"invalid archive size");
    wire::require(bytes.substr(0,8)=="DFNAT001","unknown native archive profile");
    std::size_t pos=8;wire::require(wire::take_u64(bytes,pos)==1,"unsupported native archive version");
    const auto size=wire::take_u64(bytes,pos);wire::require(size==bytes.size()-56,"truncated or trailing archive bytes");
    const auto signed_part=bytes.substr(0,bytes.size()-32);
    wire::require(sha256_bytes(signed_part)==bytes.substr(bytes.size()-32),"native archive checksum mismatch");
    wire::Reader<T> reader(bytes.substr(24,static_cast<std::size_t>(size)),codec,limits);
    ArchiveIdentity found;reader(found);wire::require(found==expected,"archive scope/policy/codec/predecessor mismatch");
    EngineCheckpoint<T> checkpoint;reader(checkpoint);wire::require(reader.remaining()==0,"trailing native checkpoint fields");
    validate_checkpoint(checkpoint,policy);return checkpoint;
}
} // namespace df::persistence
