#pragma once

#include <stdexcept>
#include <regex>

#include "cppshell/subprocess.hpp"

class VMError : public std::runtime_error {
  using runtime_error::runtime_error;
};

class VM {
  std::string name_;
  std::string macaddress_;
  std::string ip_;
  std::string hda_;
  std::string hdb_;
  std::string bootstring_;
  std::string logfile_;
  cppshell::subprocess* proc_{0};
  bool is_booted_{false};
  
public:
  void boot(bool VERB = false);
    
  inline void kill(){
    delete proc_;
  }
    
  inline std::string ip(){
    return ip_;
  }
  
  inline std::string logfile(){
    return logfile_;
  }
  
  inline void set_logfile(std::string s){
    logfile_=s;
  }

  inline void write(std::string s){
    proc_->write(s);
  }
  
  inline std::string name(){
    return name_;
  }
  
  inline int pid(){
    return proc_->pid();
  }
  
  inline std::string getline(){
    return proc_->getline();
  }
  
  inline std::string read(int n){
    return proc_->read(n);
  }
  
  bool is_booted();
  
  /**
     Read from the process stdin, collecting one byte at a time, until the whole matches re_match
     @returns the whole output.
     @note: This function is used i.e. to find a prompt~$ not ending with a newline.
   **/
  std::string read_until_match(std::regex re_match, bool verb = true);
  
  VM(std::string name, std::string mac, std::string ip, std::string hda, std::string hdb, 
     std::string bootstr, std::string logfile);
  
  inline virtual ~VM(){
    delete proc_;
  }

  // Don't think we want copies. Instead we emplace into a map.
  VM(const VM&) = delete;
  VM& operator=(const VM&) = delete;
  
  // Default move should be fine. 
  VM(VM&&) = default;
  VM& operator=(VM&&) = default;
  
};


class LoginException : public std::runtime_error {
  using runtime_error::runtime_error;
};

class requires_login {
  std::regex user_prompt_;
  std::regex pass_prompt_{"\\s*[P|p]assword:\\s*"};
  std::regex main_prompt_;

  VM* vm_{0};
  
public: 
  virtual void login(std::string username, std::string password, bool VERB = false);  
  requires_login(std::regex user_prompt,std::regex main_prompt, VM* vm);
  void read_to_prompt(bool VERB = false);
};


/*  
class has_shell {
  regex main_prompt_;
  cppshell::subprocess* proc_{0};
public:
  has_shell(std::regex main_prompt_, cppshell::subprocess*);
  std::string sh(std::string command);
};
*/

class LinuxVM : public VM, public requires_login { //public has_shell
  
public: 
  LinuxVM (std::string name, std::string mac, std::string ip, std::string hda, std::string hdb,
	   std::string bootstr, std::string logfile, 
	   std::regex user_prompt, std::regex main_prompt);
  
};

