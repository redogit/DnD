#include "rmal/rmal.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

static std::string readall(const std::string& p){
  std::ifstream f(p);
  if(!f) throw std::runtime_error("cannot open "+p);
  std::ostringstream s; s<<f.rdbuf(); return s.str();
}

int main(int argc,char** argv){
  try {
    if(argc<2){
      std::cerr<<"rmalc <check|compile|manifest|run|audit|trace|selfcheck|language-spec|version> [file]\n";
      return 2;
    }
    std::string cmd=argv[1];
    if(cmd=="version"){
      std::cout<<"RMALC 3.0-dev (C++20)\n";
      return 0;
    }
    if(cmd=="language-spec"){
      std::string format="summary";
      if(argc>=4 && std::string(argv[2])=="--format") format=argv[3];
      if(format=="json"){
        std::cout<<R"JSON({
  "schema":"rmal/language-contract/v3",
  "language":"RMAL",
  "full_name":"Ryan McMillan April Language",
  "compiler":"RMALC",
  "implementation":"C++20",
  "current_pipeline":["lexer","parser","AST","RMALBC1","VM","trace","audit"],
  "status_classes":["EXECUTABLE_CURRENT","PARSED_CARRIER_CURRENT","SPECIFIED_TARGET","HISTORICAL_OR_RECOVERED"],
  "boundaries":["SURFACE != SEMANTICS","PARSE != TRUTH","PARSED != EXECUTED","GENERATE != VERIFY != ADMIT","COMPILED != SCIENTIFICALLY_VALID","TRACE != PROOF"]
})JSON"<<"\n";
      } else {
        std::cout
          <<"RMAL — Ryan McMillan April Language\n"
          <<"RMALC — Ryan McMillan April Language Compiler\n"
          <<"implementation: C++20\n"
          <<"current: source -> lexer -> parser -> AST -> RMALBC1 -> VM -> trace/audit\n"
          <<"target: semantic object -> RMAL IR -> RMAL-SIR2 -> RMALKDVMLLL -> RMALBC1 -> RMALOBJ1 -> RMALEXE1\n"
          <<"classes: EXECUTABLE_CURRENT | PARSED_CARRIER_CURRENT | SPECIFIED_TARGET | HISTORICAL_OR_RECOVERED\n"
          <<"boundary: PARSED != EXECUTED; COMPILED != SCIENTIFICALLY_VALID; TRACE != PROOF\n";
      }
      return 0;
    }
    if(cmd=="selfcheck"){
      std::string report;
      bool ok=rmal::selfcheck(&report);
      std::cout<<report;
      return ok?0:1;
    }
    if(argc<3) throw std::runtime_error("source file required");
    auto src=readall(argv[2]);
    auto p=rmal::parse_source(src);
    auto bc=rmal::Compiler().compile(p);
    if(cmd=="check"){
      std::cout<<"CHECK PASS module="<<p.module<<" statements="<<p.statements.size()<<"\n";
      return 0;
    }
    if(cmd=="compile"){ std::cout<<rmal::disassemble(bc); return 0; }
    if(cmd=="manifest"){ std::cout<<rmal::manifest(bc); return 0; }
    if(cmd=="audit"){ std::cout<<rmal::audit(p,bc); return 0; }
    if(cmd=="run"||cmd=="trace"){
      rmal::VM vm; vm.run(bc,cmd=="trace");
      if(cmd=="trace")
        for(auto& e:vm.trace()) std::cout<<"TRACE pc="<<e.pc<<" op="<<e.op<<"\n";
      return 0;
    }
    throw std::runtime_error("unknown command: "+cmd);
  } catch(const std::exception& e){
    std::cerr<<"RMALC ERROR: "<<e.what()<<"\n";
    return 1;
  }
}
