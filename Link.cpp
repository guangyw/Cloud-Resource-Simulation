/*
 * Link.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "Link.h"
#include "Switch.h"
#include "PM.h"
#include "Trans.h"

Link::Link():son(), pSwitch(), bandwidth(1000), trans(){};

Link::Link(Switch* pPM, Switch* pSwitch)
	:son(pPM), pSwitch(pSwitch),bandwidth(1000), trans(){}

Link::Link(Switch* pPM, Switch* pSwitch, const int bandwidth)
	:son(pPM), pSwitch(pSwitch), bandwidth(bandwidth), trans(){}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

int Link::getBandwidth() const{return bandwidth;}

//void Link::occupy(float t){
//	++numActiveTrans;
//	busyTime = t;
//}
//void Link::release(){
//	--numActiveTrans;
//}
bool Link::ifBusy(){return trans.size() > 0;}

//void Link::reduceBusyTime(float t){
//	busyTime -= t;
//	if (busyTime < 0){
//		release();
//		busyTime = 0;
//	}
//}
//
//void Link::releaseAll() {numActiveTrans = 0;}

int Link::getAvailBand() {return bandwidth / (trans.size() + 1);}

//void Link::incrBusyTime(float t) {
//	if(!ifBusy())
//		occupy(t);
//	busyTime += t;
//}

//float Link::getBusyTime(){return busyTime;}

void Link::addTrans(float dataSize) {
	int band = getAvailBand();
	Trans* tx = new Trans(dataSize, band);
	trans.push_back(tx);
	std::list<Trans*>::iterator it = trans.begin();
	while(it != trans.end()){
		(*it)->updateBand(band);
	}
	//float busyT = dataSize / (float)band ;
	//incrBusyTime(busyT);
	//++numActiveTrans;
}

void Link::update(float tick){
	std::list<Trans*>::iterator it = trans.begin();
	while(it != trans.end()){
		(*it)->update(tick);
		if ((*it)->ifActive())
			it = trans.erase(it);
		else
			++it;
	}
}