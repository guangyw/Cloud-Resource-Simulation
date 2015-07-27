/*
 * Job.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "Job.h"
#include "MapTask.h"
#include "RedTask.h"


Job::Job() : jobID(0), mapTasks(), redTasks(), finished(false), skipTimes(0), numMapTask(0), numRedTask(0), submitTime(0), mapInputBytes(0), shuffleBytes(0), totalMapNum(0), totalRedNum(0){};
Job::Job(int jobID, int sTime, int mBytes, int sBytes)
	: jobID(jobID), mapTasks(), redTasks(), finished(false), skipTimes(0), numMapTask(0), numRedTask(0), submitTime(sTime), mapInputBytes(mBytes), shuffleBytes(sBytes),totalMapNum(0), totalRedNum(0){};
void Job::setMapTasks(std::vector<MapTask*> mapTasks){this->mapTasks = mapTasks; numMapTask = mapTasks.size(); totalMapNum = numMapTask;}
std::vector<MapTask*> Job::getMapTasks(){return mapTasks;}

void Job::setRedTasks(std::vector<RedTask*> redTasks){this->redTasks = redTasks; numRedTask = redTasks.size(); totalRedNum = numRedTask;}
std::vector<RedTask*> Job::getRedTasks(){return redTasks;}

//void Job::finishJob(){finished = true;}
bool Job::ifFinished() {return (getTotalTaskNum() - getNumFinishedMapTasks() - getNumFinishedRedTasks())<=0;}

void Job::skip() {++skipTimes;}
int Job::getSkip() const {return skipTimes;}

Job::~Job() {
	// TODO Auto-generated destructor stub
}

//bool Job::operator< (Job* pJob){
//	return (getNumRunningTasks() < pJob->getNumRunningTasks());
//}

MapTask* Job::getUnassignedMap() {
	std::vector<MapTask*>::iterator itMaps = mapTasks.begin();
	while (itMaps != mapTasks.end()){
		if (!(*itMaps) -> ifAssigned())
			return (*itMaps);
		++itMaps;
	}
	return 0;
}

RedTask* Job::getUnassignedRed() {
	std::vector<RedTask*>::iterator itReds = redTasks.begin();
	while (itReds != redTasks.end()){
		if (!(*itReds) -> ifAssigned())
			return (*itReds);
		++itReds;
	}
	return 0;
}

std::vector<MapTask*> Job::getAssignedMaps(){
	std::vector<MapTask*> temp;
	std::vector<MapTask*>::iterator itMap = mapTasks.begin();
	while (itMap != mapTasks.end()){
		if((*itMap)->ifAssigned() && (!(*itMap)->ifFinished()))
			temp.push_back(*itMap);
		++itMap;
	}
	return temp;
}

//numMapTasks and numRedTasks are kept in track of for performance optimization

//int Job::getNumAssignedTasks(){
//	int count = 0;
//	std::vector<MapTask*>::iterator itMap = mapTasks.begin();
//	std::vector<RedTask*>::iterator itRed = redTasks.begin();
//	while(itMap != mapTasks.end()){
//		if ((*itMap)->ifAssigned())
//			++count;
//		++itMap;
//	}
//
//	while(itRed != redTasks.end()){
//		if ((*itRed)->ifAssigned())
//			++count;
//		++itRed;
//	}
//	return count;
//}

int Job::getNumAssignedTasks(){
	return totalMapNum + totalRedNum - numMapTask - numRedTask;
}

//bool Job::ifFinishedAssignMap(){
//	int count = 0 ;
//	std::vector<MapTask*>::iterator itMap = mapTasks.begin();
//	while(itMap != mapTasks.end()){
//		if ((*itMap)->ifAssigned())
//			++count;
//		++itMap;
//	}
//	return count == totalMapNum;
//}

bool Job::ifFinishedAssignMap(){
	return numMapTask ==0;
}

/*bool Job::ifFinishedAssignRed(){
	int count = 0 ;
	std::vector<RedTask*>::iterator itRed = redTasks.begin();
	while(itRed != redTasks.end()){
		if ((*itRed)->ifAssigned())
			++count;
		++itRed;
	}
	return count == totalRedNum;
}*/

bool Job::ifFinishedAssignRed(){
	return numRedTask==0;
}



//int Job::getNumRunningMapTasks(){
//	int count = 0;
//	std::vector<MapTask*>::const_iterator itMap = mapTasks.begin();
//	while (itMap != mapTasks.end()){
//		if ((*itMap)->ifAssigned()){
//			++count;
//		}
//		++itMap;
//	}
//	return count;
//}

int Job::getNumRunningMapTasks(){
	return totalMapNum - numMapTask - getNumFinishedMapTasks();
}

//int Job::getNumRunningRedTasks(){
//	int count = 0;
//	std::vector<RedTask*>::const_iterator itRed = redTasks.begin();
//	while (itRed != redTasks.end()){
//		if ((*itRed)->ifAssigned()){
//			++count;
//		}
//		++itRed;
//	}
//	return count;
//}

int Job::getNumRunningRedTasks(){
	return totalRedNum - numRedTask - getNumFinishedRedTasks();
}

int Job::getNumRunningTasks(){
	int count = 0;
	count += getNumRunningMapTasks();
	count += getNumRunningRedTasks();
	return count;
}

int Job::getNumFinishedMapTasks(){
	int count = 0;
	std::vector<MapTask*>::const_iterator itMap = mapTasks.begin();
	while (itMap != mapTasks.end()){
		if ((*itMap)->ifFinished()){
			++count;
		}
		++itMap;
	}
	return count;
}

int Job::getNumFinishedRedTasks(){
	int count = 0;
	std::vector<RedTask*>::const_iterator itRed = redTasks.begin();
	while (itRed != redTasks.end()){
		if ((*itRed)->ifFinished()){
			++count;
		}
		++itRed;
	}
	return count;
}


bool Job::ifFinishedMap() {return getNumFinishedMapTasks() == totalMapNum;}

bool Job::ifFinishedRed() {return getNumFinishedRedTasks() == totalRedNum;}
bool Job::ifFinishedAssign(){return (ifFinishedMap()&& ifFinishedRed());}

void Job::reduceNumMapTask() {--numMapTask;}
void Job::reduceNumRedTask() {--numRedTask;}

int Job::getSubmitTime() {return submitTime;}
int Job::getMapBytes() {return mapInputBytes;}
int Job::getShuffleBytes() {return shuffleBytes;}

int Job::getNumMapTasks() {return mapTasks.size();}
int Job::getNumRedTasks() {return redTasks.size();}

int Job::getID() {return jobID;}


int Job::getTotalMapNum(){return totalMapNum;}
int Job::getTotalRedNum(){return totalRedNum;}
int Job::getTotalTaskNum(){return totalMapNum + totalRedNum;}

float Job::getProgress() {return static_cast<float>(getTotalTaskNum() - numMapTask - numRedTask)/static_cast<float>(getTotalTaskNum());}

std::vector<MapTask*> Job::getUnlaunchedMaps(){
	std::vector<MapTask*> tempMaps;
	std::vector<MapTask*>::iterator itMap = mapTasks.begin();
	while(itMap != mapTasks.end()){
		if (!(*itMap)->ifAssigned())
			tempMaps.push_back((*itMap));
		++itMap;
	}
	return tempMaps;
}

//void Job::launchMap(MapTask* pMap){
//	unlaunchedMaps
//}
//

