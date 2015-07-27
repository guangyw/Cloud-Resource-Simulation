/*
 * MapTask.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include <vector>
#include "Job.h"
#include "MapTask.h"
#include "PM.h"
#include <iostream>

MapTask::MapTask():blockPM(), memRequire(1000), CPURequire(500), assigned(false), startTime(0), ID(0), finished(false), parentJob(), assignedPM(){}
MapTask::MapTask(int id, Job* pJob): blockPM(), memRequire(1000), CPURequire(500), assigned(false), startTime(0), ID(id), finished(false), parentJob(), assignedPM(){
	ID = id;
	parentJob = pJob;
}

MapTask::~MapTask() {
	// TODO Auto-generated destructor stub
}

void MapTask::addBlockPM(PM* pPM){
	blockPM.push_back(pPM);
}

std::vector<PM*> MapTask::getBlockPM() const{
	return blockPM;
}


MapTask::MapTask(int mem, int cpu){
	MapTask();
	setMem(mem);
	setCPU(cpu);
}

bool MapTask::blockInPM(PM* pPM){
	std::vector<PM*>::const_iterator itPM = blockPM.begin();
	while( itPM != blockPM.end()){
		if ((*itPM) == pPM)
			return true;
		++itPM;
	}
	return false;
}

void MapTask::setMem(int mem){memRequire = mem;}
void MapTask::setCPU(int cpu){CPURequire = cpu;}

int MapTask::getMem() const {return memRequire;}
int MapTask::getCPU() const {return CPURequire;}

bool MapTask::ifAssigned() const {return assigned;}
void MapTask::setAssigned(PM* pPM) {assignedPM = pPM; assigned = true;}

void MapTask::setStartTime(float t) {startTime = t;}
void MapTask::setID(int id) {ID = id;}
int MapTask::getID() {return ID;}

bool MapTask::ifFinished() {return finished;}
void MapTask::setFinished(std::ostream& os, float t) {
	finished = true;
	os<<"map\t"<<parentJob->getID()<<"\t"<<ID<<"\t"<<t<<std::endl;
}

void MapTask::genData(int dataSize) {assignedPM->addData(parentJob->getID(), dataSize);}
PM* MapTask::getPM() {return assignedPM;}
