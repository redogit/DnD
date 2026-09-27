#pragma once
#include "decision_field/native_archive.hpp"
#include "decision_field/self_architecture_domain.hpp"

namespace df::persistence {
// Complete native fixture codec: stance, marker set and orientation, not a projection.
inline NativeCodec<self_architecture::SelfState> self_state_codec() {
    using self_architecture::SelfState;
    return {"SelfState-full","v03-1",
        [](const SelfState& value){
            std::string bytes;
            for(auto s:value.stance){wire::require(s>=-1&&s<=1,"invalid native stance");bytes.push_back(static_cast<char>(s+1));}
            wire::append_u64(bytes,static_cast<std::uint64_t>(value.orientation));wire::append_u64(bytes,value.markers.size());
            for(auto m:value.markers)wire::append_u64(bytes,static_cast<std::uint64_t>(m));
            return bytes;
        },
        [](std::string_view bytes){
            SelfState out;wire::require(bytes.size()>=self_architecture::CommitmentCount+16,"truncated native SelfState");
            std::size_t pos=0;
            for(auto& s:out.stance){const auto b=static_cast<unsigned char>(bytes[pos++]);wire::require(b<=2,"invalid encoded native stance");s=static_cast<std::int8_t>(static_cast<int>(b)-1);}
            auto integer=[&](){const auto n=std::bit_cast<std::int64_t>(wire::take_u64(bytes,pos));wire::require(n>=std::numeric_limits<int>::min()&&n<=std::numeric_limits<int>::max(),"native integer out of range");return static_cast<int>(n);};
            out.orientation=integer();const auto count=wire::take_u64(bytes,pos);
            wire::require(count==(bytes.size()-pos)/8&&(bytes.size()-pos)%8==0,"native marker count mismatch");
            std::optional<int> previous;
            for(std::uint64_t i=0;i<count;++i){const auto m=integer();wire::require(!previous||*previous<m,"duplicate or unsorted native marker");previous=m;out.markers.insert(m);}
            return out;
        },
        [](const SelfState& a,const SelfState& b){return a==b;}};
}
} // namespace df::persistence
