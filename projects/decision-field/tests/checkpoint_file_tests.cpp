#include "decision_field/checkpoint_file.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/stat.h>
using namespace df::persistence;
#define CHECK(x) do{if(!(x))throw std::runtime_error(#x);}while(false)
template<class F> void rejects(F f){bool threw=false;try{f();}catch(const std::exception&){threw=true;}CHECK(threw);}
int main(int argc,char** argv){
 CHECK(argc==2);const std::filesystem::path folder=argv[1];std::filesystem::create_directories(folder);
 const auto file=folder/"checkpoint.bin";const std::string data("all\0bytes",9);
 CHECK(!std::filesystem::exists(file));
 save_new_checkpoint(file,data);CHECK(read_checkpoint(file)==data);
 rejects([&]{save_new_checkpoint(file,"replacement");});CHECK(read_checkpoint(file)==data);
 CHECK(read_checkpoint(file,{},archive_sha256(data))==data);
 rejects([&]{(void)read_checkpoint(file,{},std::string(64,'0'));});
 ArchiveLimits small;small.max_archive_bytes=8;rejects([&]{(void)read_checkpoint(file,small);});
 rejects([&]{(void)read_checkpoint(folder/"missing");});
 rejects([&]{(void)read_checkpoint(folder);});
 const auto sym=folder/"symlink";std::filesystem::create_symlink(file,sym);
 rejects([&]{(void)read_checkpoint(sym);});rejects([&]{save_new_checkpoint(sym,"replacement");});
 CHECK(read_checkpoint(file)==data);
 struct stat meta{};CHECK(::stat(file.c_str(),&meta)==0);CHECK((meta.st_mode&0777)==0600);
 // Removal is test-fixture cleanup, not production retention policy.
 std::filesystem::remove(sym);std::filesystem::remove(file);std::filesystem::remove(folder);
 std::cout<<"create-only publication, exact bytes, trusted digest, private permissions, symlink/size guards: PASS\n";
}
