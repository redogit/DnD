#pragma once
#include "decision_field/native_archive.hpp"
#include <filesystem>
#include <system_error>
#ifndef _WIN32
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace df::persistence {
#ifndef _WIN32
namespace file_detail {
[[noreturn]] inline void error(const char* message) {throw std::system_error(errno,std::generic_category(),message);}
struct Descriptor {
    int fd{-1};
    explicit Descriptor(int value):fd(value){}
    Descriptor(const Descriptor&)=delete;Descriptor& operator=(const Descriptor&)=delete;
    ~Descriptor(){if(fd>=0)(void)::close(fd);}
    void close_checked(){const int old=fd;fd=-1;if(::close(old)!=0)error("checkpoint close failed");}
};
inline void sync(int fd,const char* reason) {
    int result;do{result=::fsync(fd);}while(result!=0&&errno==EINTR);if(result!=0)error(reason);
}
inline std::string path_string(const std::filesystem::path& path) {
    const auto s=path.string();wire::require(!s.empty()&&s.find('\0')==std::string::npos,"empty/NUL checkpoint path");return s;
}
} // namespace file_detail
#endif

// External I/O boundary: not a pure transform. Never replace an existing checkpoint.
// Requires a trusted existing parent directory and a filesystem supporting hard links/fsync.
inline void save_new_checkpoint(const std::filesystem::path& path,std::string_view bytes,ArchiveLimits limits={}) {
#ifdef _WIN32
    (void)path;(void)bytes;(void)limits;throw std::runtime_error("Windows durable checkpoint I/O is not implemented");
#else
    wire::require(bytes.size()<=limits.max_archive_bytes,"checkpoint exceeds byte limit");
    const auto destination=file_detail::path_string(path);
    wire::require(!path.filename().empty()&&path.filename()!="."&&path.filename()!="..","invalid checkpoint filename");
    const auto parent=path.has_parent_path()?path.parent_path():std::filesystem::path(".");
    const auto directory=file_detail::path_string(parent);
    file_detail::Descriptor dir(::open(directory.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC));
    if(dir.fd<0)file_detail::error("cannot open checkpoint directory");
    const auto pattern=(parent/".df-checkpoint-XXXXXX").string();std::vector<char> name(pattern.begin(),pattern.end());name.push_back('\0');
    file_detail::Descriptor file(::mkstemp(name.data()));if(file.fd<0)file_detail::error("cannot create private checkpoint stage");
    struct Cleanup {const char* name;bool active{true};~Cleanup(){if(active)(void)::unlink(name);}} cleanup{name.data()};
    if(::fcntl(file.fd,F_SETFD,FD_CLOEXEC)<0)file_detail::error("cannot protect checkpoint descriptor");
    std::size_t pos=0;
    while(pos<bytes.size()) {
        const auto chunk=std::min<std::size_t>(bytes.size()-pos,1024U*1024U);
        const auto count=::write(file.fd,bytes.data()+pos,chunk);
        if(count<0&&errno==EINTR)continue;
        if(count<0)file_detail::error("checkpoint write failed");
        if(count==0)throw std::runtime_error("checkpoint write made no progress");
        pos+=static_cast<std::size_t>(count);
    }
    file_detail::sync(file.fd,"checkpoint file fsync failed");file.close_checked();
    // link is atomic create-only: EEXIST leaves any prior checkpoint untouched.
    if(::link(name.data(),destination.c_str())!=0)file_detail::error("checkpoint publish failed (existing files are never replaced)");
    if(::unlink(name.data())!=0)file_detail::error("checkpoint published but stage cleanup failed");
    cleanup.active=false;
    file_detail::sync(dir.fd,"checkpoint published but directory durability is unconfirmed");
#endif
}
inline std::string read_checkpoint(const std::filesystem::path& path,ArchiveLimits limits={},std::string_view trusted_sha256={}) {
#ifdef _WIN32
    (void)path;(void)limits;(void)trusted_sha256;throw std::runtime_error("Windows checkpoint file I/O is not implemented");
#else
    const auto name=file_detail::path_string(path);
    file_detail::Descriptor file(::open(name.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK));
    if(file.fd<0)file_detail::error("cannot open checkpoint file");
    struct stat info{};if(::fstat(file.fd,&info)!=0)file_detail::error("cannot stat checkpoint file");
    wire::require(S_ISREG(info.st_mode)&&info.st_size>=0&&static_cast<std::uintmax_t>(info.st_size)<=limits.max_archive_bytes,"checkpoint is not a bounded regular file");
    std::string bytes(static_cast<std::size_t>(info.st_size),'\0');std::size_t pos=0;
    while(pos<bytes.size()) {
        const auto count=::read(file.fd,bytes.data()+pos,std::min<std::size_t>(bytes.size()-pos,1024U*1024U));
        if(count<0&&errno==EINTR)continue;
        if(count<0)file_detail::error("checkpoint read failed");
        if(count==0)throw std::runtime_error("checkpoint truncated during read");
        pos+=static_cast<std::size_t>(count);
    }
    char extra{};ssize_t count;do{count=::read(file.fd,&extra,1);}while(count<0&&errno==EINTR);
    if(count<0)file_detail::error("checkpoint final read failed");
    wire::require(count==0,"checkpoint grew during read");
    file.close_checked();
    if(!trusted_sha256.empty())wire::require(trusted_sha256.size()==64&&archive_sha256(bytes)==trusted_sha256,"checkpoint differs from trusted digest");
    return bytes;
#endif
}
} // namespace df::persistence
