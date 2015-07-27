/*
 * Link.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#ifndef LINK_H_
#define LINK_H_
//
//#include "PM.h"
//#include "Switch.h"
#include <list>
class PM;
class Switch;
class Trans;

class Link {
public:
	Link();
//	Link(PM*, Switch*, const int);
//	Link(PM*, Switch*);
	Link(Switch*, Switch*, const int);
	Link(Switch*, Switch*);


	int getBandwidth() const;
	//void occupy(float);
	//void release();
	bool ifBusy();
	//void reduceBusyTime(float);
	//void incrBusyTime(float);
	//float getBusyTime();
	//void addLink(PM*, Switch*);
	~Link();
	int getAvailBand();
	//void releaseAll();
	void addTrans(float);
	void update(float);


private:
//	PM* pPM;
	Switch* son;
	Switch* pSwitch;
	int bandwidth;
	//float busyTime;
	//int numActiveTrans;
	std::list<Trans*> trans;
};

#endif /* LINK_H_ */
