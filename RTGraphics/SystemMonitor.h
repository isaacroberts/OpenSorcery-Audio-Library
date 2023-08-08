#pragma once

//#include <RTCore/RTCore.h>
#include <JuceHeader.h>


#if ! RT_TYPEDEF_INCLUDED

#error "wtf"

#endif 

class SystemMonitor {
public:

	//Returns false if unknown 
	static bool isPotatoComputer();

	//Returns false if unknown 
	static bool isCpuFull() {
        return getCpuUsage() > .9;
	}

	static void initCpuMonitor();
	static double getCpuUsage();

	//Platform specific variables
	class DataPimpl;
	static std::unique_ptr<DataPimpl> pimpl;

};