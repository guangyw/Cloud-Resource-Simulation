/*
 * PM.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "PM.h"
#include <map>
#include "RedTask.h"
#include "Switch.h"
#include "MapTask.h"
#include "mapVM.h"
#include "redVM.h"
#include "Link.h"
//class RedTask;
//class MapTask;	
//class redVM;

PM::PM(): numMap(5), numRed(3), mapVMs(), redVMs(), blocks(), distanceMap(),dataSizeMap(),availNumMap(5), availNumRed(3){}
PM::PM(int PMID):Switch(PMID), numMap(5), numRed(3), mapVMs(), redVMs(), blocks(), distanceMap(), dataSizeMap(),availNumMap(5), availNumRed(3){}

std::vector<int> PM::getBlocks() {return blocks;}

int PM::getDistance(PM* pm){
	int rhsID = pm->getID();
	int lhsID = getID();
	std::map<int,int>::const_iterator itDis = distanceMap.find(rhsID);
	int distance;

	if (itDis == distanceMap.end()){
	
		std::vector<Switch*> lhs_parents;
		std::vector<Switch*> rhs_parents;
		//Switch* pParent;
		Switch* temp = pm->getParent();
		while (temp != NULL){
			rhs_parents.push_back(temp);
			temp = temp->getParent();
		}
		temp = parent;
		while (temp!=NULL){
			lhs_parents.push_back(temp);
			temp = temp->getParent();
		}

		std::vector<Switch*>::iterator lhs_it = lhs_parents.end();
		std::vector<Switch*>::iterator rhs_it = rhs_parents.end();

		--lhs_it;
		--rhs_it;
		//verify if the first element in the parent vector is common
		if (**lhs_parents.begin() == **rhs_parents.begin()){
			distance = 2;
			distanceMap[rhsID] = distance;
		}else{
			while(**(lhs_it) == **(rhs_it)){
				--lhs_it;
				--rhs_it;
			}
			++lhs_it;
			++rhs_it;

			distance = lhs_it - lhs_parents.begin() + (rhs_it - rhs_parents.begin());
			distanceMap[rhsID] = distance;
		}
	}else{
		distance = itDis->second;
	}
	return distance;
}

void PM::addData(int jobID, int dataSize) {
	std::map<int, int>::iterator itMap = dataSizeMap.find(jobID);
	if (itMap == dataSizeMap.end()){
		dataSizeMap[jobID] = dataSize;
	}else{
		dataSizeMap[jobID] += dataSize;
	}
}

//Link* PM::getLink(){
//	return link;
//}

PM::~PM() {
	// TODO Auto-generated destructor stub
}

//blockID is the index of mapper that need this block of data
void PM::assignBlock(const int uniqueMapID){
	blocks.push_back(uniqueMapID);
}

mapVM* PM::assignMap(MapTask* pMap, float t){
	if (getNumAvailMapSlot() > 0){
		std::vector<mapVM*>::iterator itVM = mapVMs.begin();
		while(itVM != mapVMs.end()){
			if ((*itVM)->ifAvail()){
				if ((*itVM)->allocMap(pMap)){
					--availNumMap;
					pMap->setAssigned(this);
					pMap->setStartTime(t);
					return (*itVM);
				}
			}
			++itVM;
		}
	}
	return 0;
}

redVM* PM::assignRed(RedTask* pRed, float t){
	if (getNumAvailRedSlot() > 0){
		std::vector<redVM*>::iterator itVM = redVMs.begin();
		while(itVM != redVMs.end()){
			if ((*itVM)->ifAvail()){
				if ((*itVM)->allocRed(pRed)){
					--availNumRed;
					pRed->setAssigned(this);
					pRed->setStartTime(t);
					return (*itVM);
				}
			}
			++itVM;
		}
	}
	return 0;
}

//Switch* PM::getParent() const{
//	return parent;
//}
//
//void PM::setParent(Switch* pSwitch) {parent = pSwitch;}
//
//
//void PM::setLink(Link* pLink){link = pLink;}
//
//int PM::getID() const{return PMID;}

//int PM::getNumMap() const {return numMap;}
//int PM::getNumRed() const {return numRed;}

int PM::getData(int jobID) {return dataSizeMap[jobID];}

void PM::setMapVMs(std::vector<mapVM*> mVMs){
	mapVMs = mVMs;
}
void PM::setRedVMs(std::vector<redVM*> rVMs){
	redVMs = rVMs;
}

bool PM::ifMapSlotAvail(){return getNumAvailMapSlot()>0;}
bool PM::ifRedSlotAvail(){return getNumAvailRedSlot()>0;}

//int PM::getNumAvailMapSlot(){
//	int count = 0;
//	std::vector<mapVM*>::iterator itMapVM = mapVMs.begin();
//	while(itMapVM != mapVMs.end()){
//		if((*itMapVM)->ifAvail()){
//			++count;
//		}
//		++itMapVM;
//	}
//	return count;
//}
//
//int PM::getNumAvailRedSlot(){
//	int count = 0;
//	std::vector<redVM*>::iterator itRedVM = redVMs.begin();
//	while(itRedVM != redVMs.end()){
//		if((*itRedVM)->ifAvail()){
//			++count;
//		}
//		++itRedVM;
//	}
//	return count;
//}

int PM::getNumAvailMapSlot(){return availNumMap;}
int PM::getNumAvailRedSlot(){return availNumRed;}

void PM::incMapNum(){++availNumMap;}
void PM::incRedNum(){++availNumRed;}
