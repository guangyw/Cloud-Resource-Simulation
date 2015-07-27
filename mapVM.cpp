/*
 * mapVM.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "mapVM.h"
#include "MapTask.h"
//#include "VM.h"
//#include "MapTask.h"
#include "PM.h"
#include <iostream>
class VM;

mapVM::mapVM():pMapTask(){};

mapVM::mapVM(int mem, int cpu, PM* pm): VM(mem, cpu, pm), pMapTask() {

}

mapVM::~mapVM() {
}

bool mapVM::allocMap(MapTask* pMap){
	if (pMap->getMem() >= Mem && pMap->getCPU() >= CPU){
		pMapTask = pMap;
		vacant = false;
		return true;
	}
	return false;
}

void mapVM::releaseVM(std::ostream& os, float t){
//		delete pMapTask;
	std::cout<< "mapVM finished executing map task: "<< pMapTask->getID()<<std::endl; 
	pMapTask -> setFinished(os, t);
	pMapTask = 0;
	pPM->incMapNum();
	vacant = true;
}

bool mapVM::ifAvail() {return vacant;}