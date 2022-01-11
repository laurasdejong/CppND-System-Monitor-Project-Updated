#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float mem_total = GetValue(kProcDirectory + kMeminfoFilename,"MemTotal:");
  float mem_free = GetValue(kProcDirectory + kMeminfoFilename,"MemFree:");
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (mem_total){ //prevent /0
    return (mem_total-mem_free)/mem_total;
  }
  return 0;
}

long LinuxParser::UpTime() {
  long up_time = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
  }
  return up_time/sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies()+IdleJiffies();
  // active/(active+idle)
  }

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  int place_min = 14; //https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  int place_max = 17;
  string dir = kProcDirectory+to_string(pid)+kStatFilename;

  return GetSumRange(dir,place_min,place_max); }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // = 1+2+3+6+7+8
  return 1; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // = 4+5
  return 1; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    string temp;
    while (linestream >> temp) {
      cpu.emplace_back(temp);
      }
  }
  return cpu;
}

int LinuxParser::TotalProcesses() {
  return GetValue(kProcDirectory+kStatFilename,"processes");
}

int LinuxParser::RunningProcesses() {
  float running_process_float = GetValue(kProcDirectory + kStatFilename,"procs_running");
  return (int)running_process_float;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string path = kProcDirectory+to_string(pid)+kCmdlineFilename;
  std::ifstream stream(path);

  // Get whole line
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return string();
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string path = kProcDirectory+to_string(pid)+kStatusFilename;
  float ram_kb = GetValue(path,"VmSize:");
  return to_string(ram_kb/1000);
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string path = kProcDirectory+to_string(pid)+kStatusFilename;
  float uid = GetValue(path,"Uid:");
  return to_string(uid);
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, key;
  float value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (value == pid) {
          //ToDo remove doubledots
          // get 'key' which located before value
          return key;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  int place = 22; //start time https://man7.org/linux/man-pages/man5/proc.5.html
  string dir = kProcDirectory+to_string(pid)+kStatFilename;

  return GetSumRange(dir,place,place)/sysconf(_SC_CLK_TCK);
}

float LinuxParser::GetValue(string dir,string process_name){
  string line, key;
  float value;

  // search dir
  std::ifstream filestream(dir);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == process_name) {
          // Get number behind 'key'
          return value;
        }
      }
    }
  }
  return 0;
}

long LinuxParser::GetSumRange(string dir ,int place_min, int place_max){
  float value;
  string line;
  int i = 0;
  long sum =0;

  //search dir
  std::ifstream filestream(dir);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        i++;
        if (i >= place_min){
          sum +=value;
          if (i>= place_max){
            return sum;
          }
        }
      }
    }
  }
   return 0;
}
