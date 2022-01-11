#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
// Good as is?
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    // Get active PIDs
    std::vector<int> pids = LinuxParser::Pids();

    // Make processes for new pids
    for (int pid : pids){
        if(std::find(pid_list_.begin(), pid_list_.end(), pid) == pid_list_.end()) {
            Process p(pid);
            processes_.emplace_back(p);
            pid_list_.emplace_back(pid);
        }
    }

    // sort on cpu
    std::sort(processes_.begin(),processes_.end(),[ ]( Process n1, Process n2){
        return n1.CpuUtilization() > n2.CpuUtilization();
    });

    return processes_; }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }