#pragma once

#include "cppshell/subprocess.hpp"

struct Node
{
  Node(const std::vector<std::string>& args,
       const std::string& boot_string)
      : proc(args)
  {
    std::string s;
    do {
      s = proc.getline();
      if (s.empty()) throw 99;
    }
    while (s.find(boot_string) == std::string::npos && !s.empty());
  }
  
  uint32_t pid() {
    return proc.pid();
  }

private:
  cppshell::subprocess proc;
};
