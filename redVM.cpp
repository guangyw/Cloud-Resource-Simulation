/*
 * redVM.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "redVM.h"
#include "RedTask.h"
#include "PM.h"

redVM::redVM(int mem, int cpu, PM* pm): VM(mem, cpu, pm), pRedTask() {

}

redVM::~redVM() {
}

redVM::redVM():pRedTask(){};

bool redVM::allocRed(RedTask* pRed){
	if (pRed->getMem() >= Mem && pRed->getCPU() >= CPU){
		pRedTask = pRed;
		vacant = false;
		return true;
	} 
	return false;
}

void redVM::releaseVM(std::ostream& os, float t){
//		delete pRedTask;
	std::cout<< "redVM finished executing map task: "<< pRedTask->getID()<<std::endl; 
	pRedTask -> setFinished(os, t);
	pRedTask = 0;
	pPM->incRedNum();
	vacant = true;
}


bool redVM::ifAvail() {return vacant;}
