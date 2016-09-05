#include "vm.hpp"

#include <iostream>
#include <cstdio>

using namespace std;
using namespace cppshell;

void VM::boot(bool VERB){
  //cout << "Booting " << name << endl;
  proc_ = new subprocess{
    //"numactl", "--physcpubind=4","--membind=0",
    /*
    "qemu-system-x86_64", "-enable-kvm",
    "-cpu","host","-m","1024","-smp","1",
    "-hda", hda_, "-hdb", hdb_,
    "-device","virtio-net,netdev=net0,mac="+macaddress_,
    "-netdev","tap,id=net0,script=./qemu-ifup",
    "-nographic"};
    */
    /*
    "qemu-system-x86_64", "-enable-kvm",
    "-hda", hda_, "-nographic"};
    */
    "telnet", "irc.homelien.no", "6667"};
  
  cout << "booting..." << endl;
  string s;
  do {
      cout << "getline: ";
      s = proc_->getline();
      printf("after getline\n");
      cout << s << endl;
      if (s.empty()) throw 99;
      
      cout << s << endl;
      if (VERB) cout << s;
  }
  while (s.find(bootstring_) == string::npos && !s.empty());
  is_booted_ = true;
}

string VM::read_until_match(regex re_match, bool verb){
  vector<string> lines;
  string input{};
  string latest{};
  do {
    latest = proc_->read(1);
    if (latest == "\n") {
      lines.push_back(input + latest);
      if (verb)
	cout << "<matching> " << input << endl;
      input = "";
      continue;
    }
    input += latest;
    
  } while (! regex_match(input, re_match));
  
  return input;
}


VM::VM(string aname,string amac, string anip, string ahda, string ahdb, 
       string abootstr, string alogfile) : 
    name_{aname},macaddress_{amac},ip_{anip},hda_{ahda},hdb_{ahdb},
				 bootstring_{abootstr},logfile_{alogfile}{}



bool VM::is_booted(){
  // Should probably check the status of proc...
  return is_booted_;
}


requires_login::requires_login(std::regex user_prompt,std::regex main_prompt,VM* vm) :
  user_prompt_(user_prompt), main_prompt_(main_prompt), vm_(vm)  {}

void requires_login::login(string user, string pass, bool VERB)
{
  if (VERB) cout << "Logging in " << endl;
  
  string input = "";
  vm_->read_until_match(user_prompt_, VERB);
  
  if (VERB) cout << "At the login prompt" << endl; 
  
  vm_->write(user+"\n");
  
  vm_->getline();
  vm_->read_until_match(pass_prompt_);
  
  if (VERB) cout << "At pasword prompt" << endl;
  vm_->write(pass+"\n");
  vm_->getline();  
  
  if (VERB) cout << "Should be in now. Reading to prompt" << endl;
  
  string res = vm_->read_until_match(main_prompt_, VERB);
  
  if (VERB) cout << res;
}

void requires_login::read_to_prompt(bool VERB) {
  string res = vm_->read_until_match(main_prompt_, VERB);
  if (VERB) cout << res;
}
