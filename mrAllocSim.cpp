//============================================================================
// Name        : mrAllocSim.cpp
// Author      : Guangyan Wang
// Version     :
// Copyright   : Your copyright notice
//============================================================================

#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <sstream>
#include <math.h>
#include <list>
#include <assert.h>
#include "Job.h"
#include "Link.h"
#include "PM.h"
#include <map>
#include "Switch.h"
#include <algorithm>
#include "MapTask.h"
#include "RedTask.h"
#include "VM.h"
#include "mapVM.h"
#include "redVM.h"
#include "Trans.h"


class Job;

using namespace std;

const int numPMs = 100;
//const int numJobs = 30;
const int numSwitches = 10;
const int numBlocks = 3;
const int defaultBlockSize = 128 * 1024;
//const int replicaLevel = 3;

//Since the root level of a cluster is seldomly congested, we use one number to represent the transmission delay.
const float TxRootLevelBlock = 0.5;

//the value for delay scheduling
const int maxSkipTimes = 5;

//mimimal time that take into account
const float timeScale = 0.2;

//Simulated time 
float simuTime = 0;

//int hops[numPMs][numPMs];
//int assignment[][];

std::vector<Job*> jobs;
std::list<Job*> remainningJobs;
//std::map<PM,Link> cluster;
std::vector<PM*> PMs;
std::vector<Switch*> Switches;
std::vector<Link*> links;

//This is used to keep track of the time that a VM should be released.
map<float, mapVM*> mapTimeMap;
map<float, redVM*> redTimeMap;

//MapID and execution time of the map task. To simplify calculation, in a homogeneous cluster, execution time of map is set to be the same since the default block size is the same.
//to do : initialize mapMap and redMap 
map<int, float> mapMap;
//const float mapTime = 40;
map<int, float> redMap;

const float base_tx_time = 0.5;

//const int computingSpeed = 1000;

//default computing time for 64mb data input
const float comTime = 5; 

const int numMapVM = 5;
const int numRedVM = 3;

const int maxRedTask = 50;

map<int, float> mapRate;

const int maxBand = 1024;

//A poisson generate to simulate the output generation by map tasks
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator;
//std::poisson_distribution<int> poisson();

//default map output generating rate
//const mapGenRate = 


//void linkThem(PM*, Switch*);
void linkThem(Switch*, Switch*);
void destruct();
std::vector<MapTask*> initMapTasks(int, Job*);
std::vector<RedTask*> initRedTasks(Job*, int);
void scheduleJob(Job*);
void scheduleMaps(vector<MapTask*>);
//void scheduleRed(vector<RedTask*>);
bool scheduleRed(PM* , Job*, float);

int getMapAssignCost(MapTask*, PM*);
float getExpectedAssignCost(MapTask*);

int getRedAssignCost(RedTask*, PM*);
float getExpectedAssignCost(RedTask*);

float getProbOfAssign(MapTask*, PM*);

bool scheduleAMap(Job*, PM*, float, ostream&);
bool fairScheduleAMap(Job* , PM* , float, ostream&);
bool randomScheduleRed(PM* pPM, Job* pJob, float t);
float getProbOfAssign(RedTask*, PM*);

float getTxTime(PM*, PM*);

void updateLink(float, ostream&, float);

void sortJobsByProg();

void sortJobsByAssignedNum();

//Let unique id be jobID + 000 + Map/Red id
int getUniqueID(int jobID, int taskID) {
//   int times = 1;
 //  while (times <= taskID +3)
 //     times *= 10;
	int times = 1000;
	return jobID*times + taskID;
} 


//job are launched together. 
void initJobs(){
	cout<<"initializing Jobs"<<endl;

	//in stream from trace file
	std::ifstream infile("FB.txt");
	std::ofstream of("FBp.txt");
	if (!infile.is_open()){
		cout<< "cannot find file "<<endl;
	}
	//Number of map tasks for a job is determined by the data size to be processed
	//and the default block size
	int jobID = 1;
	//random job size between 200 and 2000
//	for (int i = 0; i < numJobs; ++i){
//		jobID = i + 1;
//		Job* ptrJob = new Job(jobID);
//		int numMapTask;
//		int jobSize = rand() % 1800 + 200;
//		if (jobSize % defaultBlockSize == 0)
//			numMapTask = jobSize/defaultBlockSize;
//		else
//			numMapTask = jobSize/defaultBlockSize + 1;

//		ptrJob->setMapTasks(initMapTasks(numMapTask, ptrJob));
//		ptrJob->setRedTasks(initRedTasks(ptrJob));

	//	jobs.push_back(ptrJob);
//	}
	long submitTime, submitGap, mapSize, shuffleSize, redSize;
	int numMapTask;
	long jobSize;
	jobs.reserve(200);
	string line;
	while (std::getline(infile, line)){
		std::istringstream iss(line);
		iss>>submitTime>>submitGap>>mapSize>>shuffleSize>>redSize;
	//while (infile>>submitTime>>submitGap>>mapSize>>shuffleSize>>redSize){
		cout<<submitTime<<" "<<submitGap<<" "<<mapSize<<" "<<shuffleSize<<endl;
		if (shuffleSize != 0){
			Job* ptrJob = new Job(jobID, submitTime, mapSize, shuffleSize);
			jobSize = mapSize;
			if (jobSize > 128*1024*100)
				numMapTask = 100;
			else{
				if (jobSize % defaultBlockSize == 0)
					numMapTask = jobSize/defaultBlockSize;
				else
					numMapTask = jobSize/defaultBlockSize + 1;
			}

			ptrJob->setMapTasks(initMapTasks(numMapTask, ptrJob));
			ptrJob->setRedTasks(initRedTasks(ptrJob, numMapTask));
			jobs.push_back(ptrJob);
			remainningJobs.push_back(ptrJob);
			++jobID;
			of<<mapSize<<"\t"<<redSize<<endl;
		}
	}
}



void initHomoCluster(){
	//Initialize the cluster topology
	//XML file to define cluster topology?
	//Three level cluster topology. Switches are the second level. The links from switches to the root switch is not shown.
	//Since study shows the congestion happens mainly in the lower level.
	cout<<"initializing Cluster"<<endl;

	//This is the one root switch
	Switch* root = new Switch(0);
	PMs.reserve(numPMs);
	PM* pmTemp;
	for (int i = 0; i < numPMs; ++i){
		pmTemp = new PM(i+1);
		vector<mapVM*> tempMapVMs;
		vector<redVM*> tempRedVMs;
		//each PM have 5 map VM and 3 red VM
		for(int i = 0; i < numMapVM; ++i){
			tempMapVMs.push_back(new mapVM(1000,500,pmTemp));
		}
		for (int i= 0; i< numRedVM; ++i){
			tempRedVMs.push_back(new redVM(1000,500,pmTemp));
		}
		pmTemp->setMapVMs(tempMapVMs);
		pmTemp->setRedVMs(tempRedVMs);


		PMs.push_back(pmTemp);
	}

		//Creat Level one switch
	Switches.reserve(numSwitches + numSwitches/2);
	for (int i = 0; i < numSwitches; ++i){
		Switch* pTemp = new Switch(i+1);
		Switches.push_back(pTemp);
//		pTemp->setParent(root);
	}

	//Link PMs with level 1 switches
	std::vector<PM*>::iterator itPM = PMs.begin();
	while(itPM != PMs.end()){
		int randSwitchIdx = rand() % numSwitches;
		linkThem(*itPM, Switches[randSwitchIdx]);
		++itPM;
	}

	//Create Level two Switch
	vector<Switch*> tempSws;
	for (int i = 0; i< numSwitches/2; ++i){
		Switch* pSw = new Switch(numSwitches + i +1);
		pSw->setParent(root);
		tempSws.push_back(pSw);
	}

	//link level one switches to level two switch
	vector<Switch*>::iterator itSw = Switches.begin();
	while (itSw != Switches.end()){
		int randSwIdx = rand() % (numSwitches/2);
		linkThem(*itSw, tempSws[randSwIdx]);
		++itSw;
	}

	itSw = tempSws.begin();
	while (itSw != tempSws.end()){
		Switches.push_back(*itSw);
		++itSw;
	}
}

void linkThem(Switch* cSw, Switch* pSw) {
	Link* pLink = new Link(cSw, pSw);
	cSw->setLink(pLink);
	cSw->setParent(pSw);
	pSw->setLink(pLink);
	links.push_back(pLink);
}


//int getNumOfRemainingJob(){
//	vector<Job*>::iterator itJob;
//	itJob = jobs.end();
//	int remainingJobCount = jobs.size();
//	//check the remaining number of jobs.
//	while (--itJob != jobs.begin()) {
//		if ((*itJob)->ifFinished()){
//			cout<<"Job "<<(*itJob)->getID() <<" has finished"<<endl;
//			--remainingJobCount;
//		}
//	}
//	return remainingJobCount;
//}

void updateRemainningJob(ostream& os, float t){
	list<Job*>::iterator itJob = remainningJobs.begin();
	//check the remaining number of jobs.
	while (itJob != remainningJobs.end()) {
		if ((*itJob)->ifFinished()){
			cout<<"Job "<<(*itJob)->getID() <<" has finished"<<endl;
//			os<<(*itJob)->getID()<<"\t"<<t<<endl;
			itJob = remainningJobs.erase(itJob);
		}else{
			++itJob;
		}
	}
}

void genData(){
		//Every 1 sec, running map tasks generate output to the PM
	int dataSize;
	float mRate;
//	vector<Job*>::iterator itJob = jobs.end();
	list<Job*>::iterator itJob = remainningJobs.end();
	while (--itJob != remainningJobs.begin()){
		vector<MapTask*> maps = (*itJob)->getAssignedMaps();
		if (maps.size() > 0){
			//mapRate = static_cast<float>((*itJob)->getShuffleBytes())/static_cast<float>((*itJob)->getTotalMapNum()*comTime);

			vector<MapTask*>::iterator itMap = maps.begin();
			while (itMap != maps.end()){
				mRate = mapRate[getUniqueID((*itJob)->getID(), (*itMap)->getID())];
				poisson_distribution<int> poisson(mRate);
//				if ((*itMap)->ifAssigned() && !((*itMap)->ifFinished())){
					dataSize = poisson(generator);
					(*itMap) ->genData(dataSize);
					++itMap;
				//}
			}
		}
		/*else{                 //neeeded
			break;
		}*/
	}
//				++itJob;
}

//void chooseRedToSchedule(PM* pPM){
//	vector<Job*>::iterator itJob = jobs.end();
//	--itJob;
//	while (itJob != jobs.begin()){
//		if ((*itJob)->ifFinishedMap()){
//			//Schedule a reduce here;
//			if (scheduleRed(pPM, *itJob, simuTime)){
//				(*itJob)->reduceNumRedTask();
//				break;
//			}
//		}
//		--itJob;
//	}
//}


//In coupling scheduler, when a job is to be scheduled, only one task is scheduled.
int main() {
	ofstream logFile;
	logFile.open("log.txt");
	ofstream localTask;
	localTask.open("localTask.tsv");
	ofstream utilization;
	utilization.open("utilization.tsv");
	ofstream linkUtilization;
	linkUtilization.open("linkUtilization.tsv");

	initHomoCluster();
	initJobs();
	//sort job according to the scheduling sequence or use choose job
	list<Job*>::iterator itJob;
	vector<PM*>::iterator itPM = PMs.begin();
	//map output generator time interval
	float previousTime = simuTime;

	cout<<"total number of Jobs "<<jobs.size()<<endl;
	cout<<"Current time: "<<simuTime<<endl;

	int remainingJobCount = remainningJobs.size();
//	float mapRate;

	while(remainingJobCount > 0){
		cout<<"At time: "<<simuTime<<" remaining Job" <<remainingJobCount<<endl;
		//Try to find if existing jobs have local data on this PM
		//Implemented by fair scheduler
		//itJob = jobs.begin();
		//while (itJob != jobs.end()){
		//	if (scheduleAMap(*itJob, *itPM, simuTime)){
		//		(*itJob)->reduceNumMapTask();
		//		break;
		//	}else{
		//		++itJob;
		//	}
		//}

		sortJobsByAssignedNum();                //needed for selective scheduling
		itJob = remainningJobs.begin();
		while (itJob != remainningJobs.end()){
			if (!((*itJob)->ifFinishedAssignMap())){
				if ((*itPM)->ifMapSlotAvail()){
					if (scheduleAMap(*itJob, *itPM, simuTime, localTask)){
//					if (fairScheduleAMap(*itJob, *itPM, simuTime, localTask)){
//						Job* tempJob = *itJob;			//delete for our method
						(*itJob)->reduceNumMapTask();   
//						remainningJobs.erase(itJob);	//delete for our method
//						remainningJobs.push_back(tempJob); //delete for our method
						break;
					}else{
//						Job* tempJob = *itJob;			//delete for our method
//						remainningJobs.erase(itJob);	//delete for our method
//						remainningJobs.push_back(tempJob); //delete for our method
						break;//delete for our method
					}

				}else{
					break;
				}
			}
			++itJob;
		}

		//Schedule a reduce task from Jobs that are finished assignning map
		sortJobsByProg();         //needed in selective scheduling
		if((*itPM)->ifRedSlotAvail()){
			if (simuTime > 1.1){
				itJob = remainningJobs.end();
				while (--itJob != remainningJobs.begin()){
					if ((*itJob)->ifFinishedAssignMap()&& (!((*itJob)->ifFinishedAssignRed()))){
						//Schedule a reduce here;
						if (scheduleRed(*itPM, *itJob, simuTime)){               //needed in our method
	//					if (randomScheduleRed(*itPM, *itJob, simuTime)){         //fairschedule
							(*itJob)->reduceNumRedTask();
							break;
						}
					}
				}
			}
		}

	/*	if (simuTime > 1.1){
			chooseRedToSchedule(*itPM);
		}
*/
		//if(simuTime - previousTime > 1){
		//	itJob = jobs.end();
		//	--itJob;
		//	int dataSize;
		//	mapRate = static_cast<float>((*itJob)->getShuffleBytes())/static_cast<float>((*itJob)->getNumMapTasks());
		//	poisson_distribution<int> poisson(mapRate);
		//	vector<MapTask*>::iterator itMap = (*itJob)->getMapTasks().begin();
		//	while (itMap != ((*itJob)->getMapTasks().end())){
		//		if ((*itMap)->ifAssigned() && !((*itMap)->ifFinished())){
		//			dataSize = poisson(generator);
		//			(*itMap) ->genData(dataSize);
		//		}
		//		++itMap;
		//	}
		//	previousTime = simuTime;
		//}

		//Everytime update link status
		updateLink(timeScale, linkUtilization, simuTime);

		//suppose the interval between each heartbeat is 1s
		simuTime += timeScale;
		
		utilization<<simuTime<<"\t"<<static_cast<float>(mapTimeMap.size())/(numMapVM * numPMs)<<"\t"<<static_cast<float>(redTimeMap.size())/(numRedVM * numPMs)<<endl;
		if (mapTimeMap.size()>0){
			map<float, mapVM*>::iterator itMapTime = mapTimeMap.begin();
			if (itMapTime->first < simuTime ){
				mapVM* pVM;
				pVM = itMapTime->second;
				pVM->releaseVM(logFile, simuTime);
				itMapTime = mapTimeMap.erase(itMapTime);
			}
		}

		if (redTimeMap.size()>0){
			map<float, redVM*>::iterator itRedTime = redTimeMap.begin();
			redVM* pVM;
			while(itRedTime != redTimeMap.end()){ 
				if (itRedTime->first < simuTime ){
					pVM = itRedTime->second;
					pVM->releaseVM(logFile, simuTime);
					itRedTime = redTimeMap.erase(itRedTime);
				}else
					break;
			}
		}


		++itPM;
		if (itPM == PMs.end()){
			itPM = PMs.begin();
		}
		
		updateRemainningJob(logFile, simuTime);
		remainingJobCount = remainningJobs.size();
		if(simuTime - previousTime >= 1){
			genData();
			previousTime = simuTime;
		}
	}
	logFile.close();
	utilization.close();
	linkUtilization.close();
	localTask.close();
	return 0;
}

void updateLink(float t, ostream& os, float simuTime){
	int count = 0;
	vector<Link*>::iterator itLink = links.begin();
	while (itLink != links.end()){
		(*itLink)->update(t);
		if((*itLink)->ifBusy()){
			++count;
		}
		++itLink;
	}
	cout<<"Number of busy links "<<count<<endl;
	os<<simuTime<<"\t"<<static_cast<float>(count)/links.size()<<endl;
}

//schedule a reduce job
//To do: Should make the sum of probability of assign of all reduce tasks to be 1;
bool scheduleRed(PM* pPM, Job* pJob, float t){
//	if (pPM->getNumAvailRedSlot() >0 ){
	vector<RedTask*> reds = pJob->getRedTasks();
	vector<RedTask*>::iterator itRed = reds.begin();
	float tempProb;
	float maxProb = 0;
	RedTask* tempRed = 0;
	while(itRed != reds.end()){
		if(!(*itRed)->ifAssigned()){
			tempProb = getProbOfAssign(*itRed, pPM);
			if (tempProb > maxProb){
				tempRed = *itRed;
				maxProb = tempProb;
			}
		}
		++itRed;
	}
		
	float prob = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
	if (maxProb > prob) {
		//if (getUniqueID(pJob->getID(), tempRed->getID()) == 65001){
		//	cout <<"1"<<endl;
		//}
		cout<<"Scheduling red task "<<tempRed->getID()<<" of Job: "<<pJob->getID()<<" on PM: "<<pPM->getID()<<endl;

		//transmission of shuffle data
		float inc;
		int dataSize;
		std::vector<PM*> blockPMs;
		Job* pJob = tempRed->getParentJob();
		std::vector<MapTask*> maps = pJob -> getMapTasks();
		std::vector<MapTask*>::iterator itMap = maps.begin();
		while(itMap != maps.end()){
			blockPMs.push_back((*itMap)->getPM());
			++itMap;
		}
		std::vector<PM*>::iterator itBlockPM = blockPMs.begin();
		while (itBlockPM != blockPMs.end()){
			dataSize = (*itBlockPM)->getData(pJob->getID());  
			Link* pLink = (*itBlockPM)->getLink();
			//inc = static_cast<float>(dataSize) * base_tx_time/static_cast<float>((defaultBlockSize * pJob->getTotalRedNum()));
//				inc = static_cast<float>(dataSize)/defaultBlockSize * base_tx_time/pJob->getNumRedTasks();
			//pLink->incrBusyTime(inc);
			pLink->addTrans(dataSize/pJob->getTotalRedNum());
			++itBlockPM;
		}

//			float incTime = static_cast<float>(pJob->getShuffleBytes())/(pJob->getNumRedTasks()*defaultBlockSize) * base_tx_time;
		//float incTime = base_tx_time * static_cast<float>(pJob->getShuffleBytes())/static_cast<float>(pJob->getTotalRedNum()*defaultBlockSize);
		//pPM->getLink()->incrBusyTime(incTime);
		pPM->getLink()->addTrans(dataSize/pJob->getTotalRedNum());

		//get the delay
		float delay = 0;
		float maxDelay = 0;
		itBlockPM = blockPMs.begin();
		while(itBlockPM != blockPMs.end()){
			delay = getTxTime(*itBlockPM, pPM);
			if ( delay > maxDelay){
				maxDelay = delay; 
			}
			++itBlockPM;
		}


		redVM* tempVM = pPM->assignRed(tempRed, t);
		float tt = t + redMap[getUniqueID(pJob->getID(),(tempRed)->getID())];
		tt += maxDelay;
		redTimeMap[tt] = tempVM;
		return true;
	}
//	}
	return false;
}

//random schedule red
bool randomScheduleRed(PM* pPM, Job* pJob, float t){
//transmission of shuffle data
	float inc;
	int dataSize;
	std::vector<PM*> blockPMs;
	RedTask* tempRed = pJob->getUnassignedRed();
	std::vector<MapTask*> maps = pJob -> getMapTasks();
	std::vector<MapTask*>::iterator itMap = maps.begin();
	while(itMap != maps.end()){
		blockPMs.push_back((*itMap)->getPM());
		++itMap;
	}
	std::vector<PM*>::iterator itBlockPM = blockPMs.begin();
	while (itBlockPM != blockPMs.end()){
		dataSize = (*itBlockPM)->getData(pJob->getID());  
		Link* pLink = (*itBlockPM)->getLink();
		//inc = static_cast<float>(dataSize) * base_tx_time/static_cast<float>((defaultBlockSize * pJob->getTotalRedNum()));
		pLink->addTrans(dataSize/pJob->getTotalRedNum());
		//pLink->incrBusyTime(inc);
		++itBlockPM;
	}

	//float incTime = base_tx_time * static_cast<float>(pJob->getShuffleBytes())/static_cast<float>(pJob->getTotalRedNum()*defaultBlockSize);
	pPM->getLink()->addTrans(dataSize/pJob->getTotalRedNum());
	//pPM->getLink()->incrBusyTime(incTime);


	//get the delay
	float delay = 0;
	float maxDelay = 0;
	itBlockPM = blockPMs.begin();
	while(itBlockPM != blockPMs.end()){
		delay = getTxTime(*itBlockPM, pPM);
		if ( delay > maxDelay){
			maxDelay = delay; 
		}
		++itBlockPM;
	}


	redVM* tempVM = pPM->assignRed(tempRed, t);
	float tt = t + redMap[getUniqueID(pJob->getID(),(tempRed)->getID())];
	tt += maxDelay;
	redTimeMap[tt] = tempVM;
	return true;
}

bool scheduleAMap(Job* pJob, PM* pPM, float t, ostream& os){
	//To do:: add reduce block
	//std::vector<int>::iterator itBlocks = pPM->getBlocks().begin();
	vector<MapTask*> maps = pJob->getMapTasks();
	if (pPM->getNumAvailMapSlot() >0){
		vector<MapTask*>::iterator itMap = maps.begin();
		while (itMap != maps.end()){
			if (!(*itMap)->ifAssigned()){
			//try to shedule local task first
				if((*itMap)->blockInPM(pPM)){
					mapVM* tempVM = pPM -> assignMap(*itMap, t);
					if (tempVM != 0){
						float tt = t + mapMap[getUniqueID(pJob->getID(),(*itMap)->getID())];
						mapTimeMap[tt] = tempVM;
						mapRate[getUniqueID(pJob->getID(),(*itMap)->getID())] = static_cast<float>(pJob->getShuffleBytes()) /((tt -t)*pJob->getTotalMapNum());
						cout<<"Assign a local task of "<<pJob->getID() << " to PM "<<pPM->getID()<<endl;
						os<<pJob->getID()<<"\t"<<(*itMap)->getID()<<"\tlocal"<<endl;
						return true;
					}
				}
			}
			++itMap;
		}
		cout<<"No local task for PM "<<pPM->getID()<<endl;
		itMap = maps.begin();
		float prob;			
//		while ((*itMap)->ifAssigned())
//				++itMap;
		prob = static_cast<float>(rand())/static_cast<float>(RAND_MAX);

		float tempProb;
		float maxProb = 0;
		MapTask* tempMap = maps[0];
		//find the map task with highest probability
		while(itMap != maps.end()){
			if (!(*itMap)->ifAssigned()){
				tempProb = getProbOfAssign(*itMap, pPM);
				if (tempProb > maxProb){
					tempMap = *itMap;
					maxProb = tempProb;
				}
			}
			++itMap;
		}

		if (maxProb > prob) {
			cout<<"Scheduling map task "<<tempMap->getID()<<" of Job: "<<pJob->getID()<<" on PM: "<<pPM->getID()<<endl;
			PM* pTemp;
			int distance;
			int minDistance = 100;
			std::vector<PM*> BlockPMs = tempMap->getBlockPM();
			for(int i = 0; i < numBlocks; ++i){
				distance = pPM->getDistance(BlockPMs[i]);
				if (distance < minDistance){
					pTemp = BlockPMs[i];
					minDistance = distance;
				}
			}
			mapVM* tempVM = pPM->assignMap(tempMap, t);
			if (tempVM != 0){
				float tTemp = getTxTime(pPM, pPM);
				float tt = t + mapMap[getUniqueID(pJob->getID(),(tempMap)->getID())] + tTemp;
				mapTimeMap[tt] = tempVM;
				mapRate[getUniqueID(pJob->getID(),(tempMap)->getID())] = static_cast<float>(pJob->getShuffleBytes()) /((tt -t)*pJob->getTotalMapNum());
				float mapSize = static_cast<float>(pJob->getMapBytes())/pJob->getTotalMapNum();
				//float inc = base_tx_time * mapSize / defaultBlockSize;
				//pPM->getLink()->incrBusyTime(inc);
				//pPM->getLink()->incrBusyTime(base_tx_time);
				pPM->getLink()->addTrans(mapSize);
				if (minDistance > 2)
					os<<pJob->getID()<<"\t"<<tempMap->getID()<<"\tremote"<<endl;
				else
					os<<pJob->getID()<<"\t"<<tempMap->getID()<<"\tlocalRack"<<endl;
				//pTemp->getLink()->incrBusyTime(0.488);
				return true;
			}
		}else{
			cout<<"Not assignning "<<tempMap->getID()<<" of Job: "<<pJob->getID()<<" on PM: "<<pPM->getID()<< "is a better option" <<endl;
		}
	}
	return false;
}

bool fairScheduleAMap(Job* pJob, PM* pPM, float t, ostream& os){
	if (pPM->getNumAvailMapSlot()>0){
		if(!pJob->ifFinishedAssignMap()){
			vector<MapTask*> maps = pJob->getUnlaunchedMaps();
			vector<MapTask*>::iterator itMap = maps.begin();
			while (itMap != maps.end()){
				if((*itMap)->blockInPM(pPM)){
					mapVM* tempVM = pPM -> assignMap(*itMap, t);
					if (tempVM != 0){
						float tt = t + mapMap[getUniqueID(pJob->getID(),(*itMap)->getID())];
						mapTimeMap[tt] = tempVM;
						mapRate[getUniqueID(pJob->getID(),(*itMap)->getID())] = static_cast<float>(pJob->getShuffleBytes()) /((tt -t)*pJob->getTotalMapNum());
						os<<pJob->getID()<<"\t"<<(*itMap)->getID()<<"\tlocal"<<endl;
						return true;
					}
				}
				++itMap;
			}
			if(pJob->getSkip() < maxSkipTimes){
				pJob->skip();				
				return false;
			}
			else{
				itMap = maps.begin();
				PM* pTemp;
				int distance;
				int minDistance = 100;
				std::vector<PM*> BlockPMs = (*itMap)->getBlockPM();
				for(int i = 0; i < numBlocks; ++i){
					distance = pPM->getDistance(BlockPMs[i]);
					if (distance < minDistance){
						pTemp = BlockPMs[i];
						minDistance = distance;
					}
				}
				mapVM* tempVM = pPM -> assignMap(*itMap, t);
				if (tempVM != 0){
					float tTemp = getTxTime(pPM, pPM);
					float tt = t + mapMap[getUniqueID(pJob->getID(),(*itMap)->getID())] + tTemp;
					mapTimeMap[tt] = tempVM;
					mapRate[getUniqueID(pJob->getID(),(*itMap)->getID())] = static_cast<float>(pJob->getShuffleBytes()) /((tt -t)*pJob->getTotalMapNum());
					float mapSize = static_cast<float>(pJob->getMapBytes())/pJob->getTotalMapNum();
					//float inc = base_tx_time * mapSize / defaultBlockSize;
					//pPM->getLink()->incrBusyTime(inc);
					pPM->getLink()->addTrans(mapSize);
					if (minDistance > 2)
						os<<pJob->getID()<<"\t"<<(*itMap)->getID()<<"\tremote"<<endl;
					else
						os<<pJob->getID()<<"\t"<<(*itMap)->getID()<<"\tlocalRack"<<endl;

				return true;
				}
			}
		}
	}
	return false;
}



float getProbOfAssign(RedTask* pRed, PM* pPM) {
	float ratio = static_cast<float>(getRedAssignCost(pRed, pPM))/getExpectedAssignCost(pRed);
	float prob = 1 - exp(-1/ratio);
	return prob;
}

int getRedAssignCost(RedTask* pRed, PM* pPM){
	int distance;
	int cost = 0;
	int dataSize;
//	PM* pTemp;
	std::vector<PM*> blockPMs;
	Job* pJob = pRed->getParentJob();
	std::vector<MapTask*> maps = pJob -> getMapTasks();
	std::vector<MapTask*>::iterator itMap = maps.begin();
	while(itMap != maps.end()){
		blockPMs.push_back((*itMap)->getPM());
		++itMap;
	}
	std::vector<PM*>::iterator itBlockPM = blockPMs.begin();
	while (itBlockPM != blockPMs.end()){
		distance = pPM->getDistance((*itBlockPM));
		dataSize = (*itBlockPM)->getData(pJob->getID());  
		cost += distance * dataSize;
		++itBlockPM;
	}
	return cost;
}

float getExpectedAssignCost(RedTask* pRed){
	float expectCost = 0;
	int tempCost = 0;
	std::vector<PM*>::const_iterator itPM = PMs.begin();
	while(itPM != PMs.end()){
		expectCost += static_cast<float>(getRedAssignCost(pRed, *itPM))/static_cast<float>(numPMs);
		++itPM;
	}
	return expectCost;
}


//Naively in increasing order of number of running tasks
bool sortByProgress(Job* i, Job* j) {return i->getProgress()< j->getProgress();}
bool sortByAssignedNum(Job* i, Job*j) {return i->getNumAssignedTasks() < j->getNumAssignedTasks();}

void sortJobsByProg(){
//	std::sort(jobs.begin(), jobs.end(), sortByProgress);
	remainningJobs.sort(sortByProgress);
}

void sortJobsByAssignedNum(){
	remainningJobs.sort(sortByAssignedNum);
}

void linkSwitchToParent(Switch* pSwitch, Switch* pParentSwitch){
	pSwitch -> setParent (pParentSwitch);
}

std::vector<MapTask*> initMapTasks(int numMapTask, Job* pJob){
	//random distribute data blocks
	//It's not the default implementation that data blocks are put on machines in diffrent clusters
	float mapSize = static_cast<float>(pJob->getMapBytes())/numMapTask;
	float ctime = mapSize/defaultBlockSize * comTime;
	std::vector<MapTask*> tempVec;
	tempVec.reserve(numMapTask);
	for(int i = 0; i< numMapTask; ++i){
		MapTask* ptrMap = new MapTask(i+1, pJob);
		for (int j = 0; j < numBlocks; ++j){
			int randPMIdx = rand() % numPMs;
			ptrMap->addBlockPM(PMs[randPMIdx]);
			PMs[randPMIdx]->assignBlock(getUniqueID(pJob->getID(), i+1));
		}
		tempVec.push_back(ptrMap);
		mapMap[getUniqueID(pJob->getID(), ptrMap->getID())] = ctime;
	}
	return tempVec;
}

std::vector<RedTask*> initRedTasks(Job* pJob, int numMapTask){
	//random number of reduce tasks between 3 and 10
	int numRedTask = rand() % numMapTask + 3;
	if (numRedTask > 50)
		numRedTask = 50;
	float redSize = static_cast<float>(pJob->getShuffleBytes()) / static_cast<float>(numRedTask);
	float time = redSize/defaultBlockSize * comTime;
	std::vector<RedTask*> tempVec;
	tempVec.reserve(numRedTask);
	for (int i = 0; i < numRedTask; ++i){
		RedTask* ptrRed = new RedTask(i+1, pJob);
		tempVec.push_back(ptrRed);
		redMap[getUniqueID(pJob->getID(), ptrRed->getID())] = time;
	}
	return tempVec;
}

int getMapAssignCost(MapTask* pMap, PM* pPM){
	int distance;
	float factDist;
	float minFactDist = 10000;
	int cost;
	PM* pTemp;
	float minRate = 1;
	std::vector<PM*> BlockPMs = pMap->getBlockPM();
	for(int i = 0; i < numBlocks; ++i){
		distance = pPM->getDistance(BlockPMs[i]);
		int tempDis = distance;
		Link* lPtrLink = pPM->getLink();
		Link* rPtrLink = pPM->getLink();
		//while (tempDis > 0){
		float lRate = static_cast<float>(lPtrLink->getAvailBand()) / static_cast<float>(maxBand);
		float rRate = static_cast<float>(rPtrLink->getAvailBand()) /static_cast<float>(maxBand);
		if (lRate < minRate)
			minRate = lRate;
		if (rRate < minRate)
			minRate = rRate;
		//	tempDis -= 2;
		//}

		factDist = static_cast<float>(distance) / minRate;
		if (factDist < minFactDist){
			pTemp = BlockPMs[i];
			minFactDist = factDist;
		}
	}
	//The cost of assigning pMap to pPM is the cost to transfer the data block from the nearest node to pPM
	cost = defaultBlockSize * minFactDist;
	return cost;
}

float getExpectedAssignCost(MapTask* pMap){
	float expectCost = 0;
	int tempCost = 0;
	std::vector<PM*>::const_iterator itPM = PMs.begin();
	while(itPM != PMs.end()){
		expectCost += static_cast<float>(getMapAssignCost(pMap, *itPM))/static_cast<float>(numPMs);
		++itPM;
	}
	return expectCost;
}


float getProbOfAssign(MapTask* pMap, PM* pPM){
	float ratio = static_cast<float>(getMapAssignCost(pMap, pPM))/getExpectedAssignCost(pMap);
	float prob = 1 - exp(-1/ratio);
	return prob;
}


//float getTxTime(PM* lpPM, PM* rpPM){
//	float tx = 0;
//	int dis = lpPM->getDistance(rpPM);
//	int hops = dis/2;
//	//This is simplified senerio where the cluster is just three level
//	if (hops > 0){
//		Link* lPtrLink = lpPM->getLink();
//		Link* rPtrLink = rpPM->getLink();
////		tx += static_cast<float>(defaultBlockSize) / static_cast<float>(lPtrLink->getBandwidth());
////		tx += static_cast<float>(defaultBlockSize) / static_cast<float>(rPtrLink->getBandwidth());
//		tx += 2* base_tx_time;
//		if (lPtrLink->ifBusy())
//			tx += lPtrLink->getBusyTime();
//		if (rPtrLink->ifBusy())
//			tx += rPtrLink->getBusyTime();
//
//		
//	}
//	if (hops == 2){
//		tx += 2*TxRootLevelBlock;
//	}
//
	/*
	//this should be implemented
	while (hops > 0){	
		Link* lPtrLink = lpPM->getLink();
		Link* rPtrLink = rpPM->getLink();
		tx += static_cast<float>(defaultBlockSize) / static_cast<float>(lPtrLink->getBandwidth);
		tx += static_cast<float>(defaultBlockSize) / static_cast<float>(rPtrLink->getBandwidth);
		Switch* lPtrTemp = lpPM->getParent();
		Switch* rPtrTemp = rpPM->getParent();		
		--hops;
	}
	*/
//
//	return tx;		
//}

//Coupling scheduler:: This is for choosing which job to schedule next 
int computeMismatch(Job* pJob){
	return 0;
}

//void linkThem(PM* pPM, Switch* pSwitch){
//	Link* pLink = new Link(pPM, pSwitch);
//	pPM->setLink(pLink);
//	pPM->setParent(pSwitch);
//	pSwitch->setLink(pLink);
//	links.push_back(pLink);
//}
//



//to be implemented: algorithm for choosing job in coupling scheduler
Job* chooseJob(){
	return jobs[0];
}


void destruct(){
	//delete everything
}



//This function is for launching job separetely at different time.
//void initJob(int jobID){
	//Number of map tasks for a job is determined by the data size to be processed
	//and the default block size
	//random job size between 200 and 2000
//	Job* ptrJob = new Job(jobID);
//	int numMapTask;
//	int jobSize = rand() % 1800 + 200;
//	if (jobSize % defaultBlockSize == 0)
//		numMapTask = jobSize/defaultBlockSize;
//	else
//		numMapTask = jobSize/defaultBlockSize + 1;

//	ptrJob->setMapTasks(initMapTasks(numMapTask, ptrJob));
//	ptrJob->setRedTasks(initRedTasks(ptrJob));
//	jobReds[jobID] = initRedTasks();
//}


//fairScheduler
//bool fairScheduleAMap(vector<MapTask*> maps, PM* pPM, float t){
//	if (pPM->getNumAvailMapSlot() >0){
//		vector<MapTask*>::iterator itMap = maps.begin();
//		while (itMap != maps.end()){
//			if((*itMap)->blockInPM(pPM)){
//				if (!(*itMap)->ifAssigned()){
//					pPM -> assignMap(*itMap, t);
//					return true;
//				}
//			}
//			++itMap;
//		}
//	}
//	return false;
//}