#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

// https://stackoverflow.com/questions/7880784/what-is-rss-and-vsz-in-linux-memory-management
// https://www.tutorialspoint.com/how-to-get-memory-usage-under-linux-in-cplusplus

void mem_usage() {
    double vm_usage = 0.0;
    double resident_set = 0.0;
    ifstream stat_stream("/proc/self/stat",ios_base::in); //get info from proc
    //create some variables to get info
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;
    unsigned long vsize;
    long rss;
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
    >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
    >> utime >> stime >> cutime >> cstime >> priority >> nice
    >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care
    stat_stream.close();
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configured
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
    cout << "Virtual Memory: " << vm_usage << " kb" << "\nResident set size: " << resident_set << " kb" << endl;
}

const int malloc_size = 1024 * 1024 * 1024; // 1GB

int main() {
    mem_usage();
    char *ptr = (char *)malloc(1024 * 1024 * 1024);
    cout << "after malloc 1GB" << endl;
    mem_usage();
    memset(ptr, 0, 1024 * 1024 * 1024);
    cout << "after memset 1GB" << endl;
    mem_usage();
}
