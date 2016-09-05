/**
 * 
 * 
 * 
**/
#include <iostream>
#include <map>
#include <regex>
#include "vm.hpp"
#include "perfdata/pidstat.hpp"

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
  vm->boot(false);
  std::cout << "VM process: " << vm->pid() << std::endl;
  
  /// do the test
  perfdata::Pidstat ps(vm->pid());
  
  printf("total CPU time: %lu\n", ps.cpu_time_total());
  printf("guest CPU time: %lu\n", ps.guest_time_total());
  /// -----------
  
  std::string output;
  std::smatch sm;
  std::regex  re(".*IncludeOS IRC Service.*");
  
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
