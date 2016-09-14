/**
 * 
 * 
 * 
**/
#include <cassert>
#include <iostream>
#include <map>
#include <regex>
#include "vm.hpp"
#include "node.hpp"
#include "httperf.hpp"
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
  float    RPS;
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
      ps.guest_time_total(),
      0.0f
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
  perfdata::Pidstat ps1(vm->pid());
  
  /// do the test
  HttPerf perf("10.00.42", "80", "500");
  
  perfdata::Pidstat ps2(vm->pid());
  cpu_time usage {
      ps2.cpu_time_total() - ps1.cpu_time_total(),
      ps2.guest_time_total() - ps1.guest_time_total(),
      perf.get_rps()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  printf("%u: requests/sec:   %.2f req/s\n", n+1, usage.RPS);
  
  vm->kill();
  delete vm;
  
  return usage;
}
cpu_time run_ubuntu_httperf(int n, uint64_t pid, const std::string& ip, const std::string& port)
{
  perfdata::Pidstat ps1(pid);
  
  /// do the test
  HttPerf perf(ip, port, "500");
  
  perfdata::Pidstat ps2(pid);
  cpu_time usage {
      ps2.cpu_time_total() - ps1.cpu_time_total(),
      ps2.guest_time_total() - ps1.guest_time_total(),
      perf.get_rps()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  printf("%u: requests/sec:   %.2f req/s\n", n+1, usage.RPS);
  
  return usage;
}

cpu_time run_nodejs_httperf(int n)
{
  Node js({"nodejs", "./node/http.js"}, "Server listening on");
  perfdata::Pidstat ps1(js.pid());
  
  /// do the test
  HttPerf perf("127.00.1", "8080", "1000");
  
  perfdata::Pidstat ps2(js.pid());
  cpu_time usage {
      ps2.cpu_time_total() - ps1.cpu_time_total(),
      ps2.guest_time_total() - ps1.guest_time_total(),
      perf.get_rps()
  };
  /// -----------
  printf("%u: total CPU time: %lu\n", n+1, usage.cpu_total);
  printf("%u: guest CPU time: %lu\n", n+1, usage.cpu_guest);
  printf("%u: requests/sec:   %.2f req/s\n", n+1, usage.RPS);
  return usage;
}

cpu_time run_nodejs_json(int n)
{
  Node js({"nodejs", "./node/http.js"}, "Server listening on");
  perfdata::Pidstat ps1(js.pid());
  
  /// do the test
  const int N = 10;
  for (int i = 0; i < N; i++)
  {
    int res = system(
      "curl -S -s -H 'Content-Type:application/json' "
      "-H 'Accept: application/json' "
      "--data-binary @generated.json "
      "http://localhost:8080/post1 > /dev/null");
    assert(res == 0);
  }

  /// measurements
  perfdata::Pidstat ps2(js.pid());
  cpu_time usage {
      (ps2.cpu_time_total() - ps1.cpu_time_total()) / N,
      (ps2.guest_time_total() - ps1.guest_time_total()) / N,
      0.0f
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
    avg.RPS += u.RPS;
  }
  avg.cpu_total /= usage.size();
  avg.cpu_guest /= usage.size();
  avg.RPS /= usage.size();
  return avg;
}

int main(void)
{
  const int RUNS = 100;
  std::vector<cpu_time> usage;
  
  for (int i = 0; i < RUNS; i++)
  {
    //usage.push_back(run_includeos_boot_test(i));
    
    //usage.push_back(run_acorn_httperf(i));
    //usage.push_back(run_nodejs_httperf(i));
    
    /// apache on virt-manager:
    //usage.push_back(run_ubuntu_httperf(i, 27259, "192.168.122.189", "80"));
    /// includeos on virt-manager:
    //usage.push_back(run_ubuntu_httperf(i, 16148, "10.00.42", "80"));
    /// nodejs on virt-manager:
    usage.push_back(run_ubuntu_httperf(i, 27259, "192.168.122.189", "8080"));
    
    //usage.push_back(run_acorn_json(i));
    //usage.push_back(run_nodejs_json(i));
    
    cpu_time avg = average(usage);
    printf("* Average total CPU time: %lu\n", avg.cpu_total);
    printf("* Average guest CPU time: %lu\n", avg.cpu_guest);
    printf("* Average requests/sec:   %.2f req/s\n", avg.RPS);
  }
  printf("------------------------------\n");
  printf("Over a total of %u runs\n", RUNS);
  
  cpu_time avg = average(usage);
  printf("* Average total CPU time: %lu\n", avg.cpu_total);
  printf("* Average guest CPU time: %lu\n", avg.cpu_guest);
  printf("* Average requests/sec:   %.2f req/s\n", avg.RPS);
}
