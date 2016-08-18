/**
 * 
 * 
 * 
**/
#include <iostream>
#include <map>
#include <regex>
#include "vm.hpp"

union inaddr4
{
  uint8_t  part[4];
  uint32_t whole;
  
  std::string str()
  {
    return std::to_string(part[0]) + "."
         + std::to_string(part[1]) + "."
         + std::to_string(part[2]) + "."
         + std::to_string(part[3]);
  }
};

std::map<std::string, VM*> instances;
#define VERBOSE

int main(void)
{
  instances["ircd"] = new
     VM("irc_server",
        "c0:01:0a:00:00:2a",
        "10.0.0.42",
        "../IRCd/IRCd.img",
        "./disk.img",
        "IRC SERVICE STARTED",
        "ircd.log");
  
  auto* vm = instances["ircd"];
  vm->boot(true);
  std::cout << "VM process: " << vm->pid() << std::endl;
  
  std::string output;
  std::smatch sm;
  std::regex  re(".*IRC SERVICE STARTED.*");
  
  do
  {
    output = vm->getline();
#ifdef VERBOSE
    std::cout << "<- " << output;
#endif
  }
  while (!std::regex_search(output, sm, re));
  
  vm->kill();     
}
