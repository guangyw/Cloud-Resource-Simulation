/*
 * RedTask.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "RedTask.h"
#include "PM.h"
#include "Job.h"
#include <iostream>

RedTask::~RedTask() {
	// TODO Auto-generated destructor stub
}

RedTask::RedTask():memRequire(1000), CPURequire(500), assigned(false), startTime(0), ID(0), finished(false), parentJob(), assignedPM(){}
RedTask::RedTask(int id, Job* pJob):memRequire(1000), CPURequire(500), assigned(false), startTime(0), ID(id), finished(false), parentJob(), assignedPM(){
	parentJob = pJob;
}

RedTask::RedTask(int mem, int cpu):memRequire(1000), CPURequire(500), assigned(false), startTime(0), ID(0), finished(false), parentJob(), assignedPM(){
	setMem(mem);
	setCPU(cpu);
}

void RedTask::setMem(int mem){memRequire = mem;}
void RedTask::setCPU(int cpu){CPURequire = cpu;}

int RedTask::getMem() const {return memRequire;}
int RedTask::getCPU() const {return CPURequire;}

bool RedTask::ifAssigned() const {return assigned;}
void RedTask::setAssigned(PM* pPM){assignedPM = pPM; assigned = true;}

void RedTask::setStartTime(float t) {startTime = t;}




void RedTask::setID(int id) {ID = id;}
int RedTask::getID() {return ID;}

bool RedTask::ifFinished() {return finished;}
void RedTask::setFinished(std::ostream& os, float t) {
	finished = true;
	os<<"red\t"<<parentJob->getID()<<"\t"<<ID<<"\t"<<t<<std::endl;
}
Job* RedTask::getParentJob() {return parentJob;}