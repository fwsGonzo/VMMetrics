/**
 * 
 * 
 * 
**/
#include <iostream>
#include <map>
#include <regex>
#include "vm.hpp"
#include "node.hpp"
#include "perfdata/pidstat.hpp"

#define VERBOSE

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

struct cpu_time
{
  uint64_t cpu_total;
  uint64_t cpu_guest;
};

cpu_time run_includeos_boot_test(int n)
{
  auto* vm = new
     VM("irc_server",
        "c0:01:0a:00:00:2a",
        "10.0.0.42",
        "../IRCd/IRCd.img",
        "./disk.img",
        "IRC SERVICE STARTED",
        "ircd.log");
  
  vm->boot(false);
  
  /// do the test
  perfdata::Pidstat ps(vm->pid());
  cpu_time usage {
      ps.cpu_time_total(),
      ps.guest_time_total()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  
  vm->kill();
  delete vm;
  
  return usage;
}
cpu_time run_acorn_httperf(int n)
{
  auto* vm = new
     VM("acorn",
        "c0:01:0a:00:00:2a",
        "10.0.0.42",
        "../acorn/Acorn.img",
        "./disk.img",
        "Running [ Acorn ]",
        "acorn.log");
  
  vm->boot(false);
  
  /// do the test
  printf("acorn booted\n");
  int res = system("httperf --server 10.00.42 --port 80 --num-conns 500 --hog");
  if (res < 0) perror(strerror(errno));
  
  perfdata::Pidstat ps(vm->pid());
  cpu_time usage {
      ps.cpu_time_total(),
      ps.guest_time_total()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  
  vm->kill();
  delete vm;
  
  return usage;
}

cpu_time run_nodejs_httperf(int n)
{
  Node js({"nodejs", "./node/http.js"}, "Server listening on");
  
  /// do the test
  int res = system("httperf --server 127.00.1 --port 8080 --num-conns 1000 --hog");
  if (res < 0) perror(strerror(errno));
  
  perfdata::Pidstat ps(js.pid());
  cpu_time usage {
      ps.cpu_time_total(),
      ps.guest_time_total()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  return usage;
}


cpu_time average(const std::vector<cpu_time>& usage)
{
  cpu_time avg {0, 0};
  for (auto& u : usage)
  {
    avg.cpu_total += u.cpu_total;
    avg.cpu_guest += u.cpu_guest;
  }
  avg.cpu_total /= usage.size();
  avg.cpu_guest /= usage.size();
  return avg;
}

int main(void)
{
  const int RUNS = 10;
  std::vector<cpu_time> usage;
  
  for (int i = 0; i < RUNS; i++)
  {
    //usage.push_back(run_acorn_httperf(i));
    usage.push_back(run_nodejs_httperf(i));
  }
  printf("------------------------------\n");
  printf("Over a total of %u runs\n", RUNS);
  
  cpu_time avg = average(usage);
  printf("* Average total CPU time: %lu\n", avg.cpu_total);
  printf("* Average guest CPU time: %lu\n", avg.cpu_guest);
  /*
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
  */
}
