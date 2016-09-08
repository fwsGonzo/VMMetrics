#pragma once

#include "cppshell/subprocess.hpp"
#include <vector>

struct HttPerf
{
  HttPerf(const std::string& host, const std::string& port, const std::string num_conns)
      : proc({"httperf", "--server", host, "--port", port, "--num-conns", num_conns, "--hog"})
  {
    std::string output;
    std::string s;
    do {
      s = proc.getline();
      if (s.empty()) throw 99;
      
      output.append(s);
    }
    while (s.find("Errors: ") == std::string::npos && !s.empty());
    
    const std::string RRS = "Request rate: ";
    auto it = output.find(RRS);
    if (it == std::string::npos) return;
    // go past size of request rate string
    it += RRS.size();
    auto nxt = output.find(" req/s", it);
    if (nxt == std::string::npos) return;
    
    auto req_string = output.substr(it, nxt - it);
    this->RPS = atof(req_string.c_str());
  }
  
  uint32_t pid() {
    return proc.pid();
  }
  
  float get_rps() const {
    return RPS;
  }

private:
  cppshell::subprocess proc;
  float RPS = 0.0f;
};
