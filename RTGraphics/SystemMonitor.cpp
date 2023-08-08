

#include "SystemMonitor.h"


//Returns false if unknown 
bool SystemMonitor::isPotatoComputer() {
    int ram = juce::SystemStats::getMemorySizeInMegabytes();

    //Less than 6gb ram is potato 
    if (ram > 0 && ram < 6500)
    {
        return true;
    }

    int mh = juce::SystemStats::getCpuSpeedInMegahertz();

    //Less than 2 Ghz is potato 
    if (mh > 0 && mh < 2400)
    {
        return true;
    }

    return false;
}


//platform specific code

/*
    Windows 
*/
#if TARGET_WINDOWS


#include <Windows.h>


struct SystemMonitor::DataPimpl {
    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors = 0;
    HANDLE self;
};


void SystemMonitor::initCpuMonitor() {
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    pimpl.reset(new DataPimpl());
    pimpl->numProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&pimpl->lastCPU, &ftime, sizeof(FILETIME));

    pimpl->self = GetCurrentProcess();
    GetProcessTimes(pimpl->self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&pimpl->lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&pimpl->lastUserCPU, &fuser, sizeof(FILETIME));
}

double SystemMonitor::getCpuUsage() {
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(pimpl->self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - pimpl->lastSysCPU.QuadPart) +
        (user.QuadPart - pimpl->lastUserCPU.QuadPart);
    percent /= (now.QuadPart - pimpl->lastCPU.QuadPart);
    percent /= pimpl->numProcessors;
    pimpl->lastCPU = now;
    pimpl->lastUserCPU = user;
    pimpl->lastSysCPU = sys;

    return percent;
}


#elif TARGET_MAC

/*
    Mac 
*/

#error "mac implimentation not implemented yet"

struct SystemMonitor::DataPimpl {

};

void SystemMonitor::initCpuMonitor() {

}
double SystemMonitor::getCpuUsage() {
    return 0;
}
#else 
#error "unrecognized platform"

#endif 


std::unique_ptr<SystemMonitor::DataPimpl> SystemMonitor::pimpl = std::make_unique<SystemMonitor::DataPimpl>();