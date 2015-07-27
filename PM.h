/*
 * PM.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

//PM should be a subclass of Switch
//Here this is not implemented for simplified senerio.

#ifndef PM_H_
#define PM_H_
#include <vector>
#include <map>
#include "Switch.h"

class Link;
class mapVM;
class redVM;
class MapTask;
class RedTask;
//class Switch;

//int combine(int a, int b) {
//   int times = 1;
//   while (times <= b)
//      times *= 10;
//   return a*times + b;
//} 

class PM : public Switch{
public:
	PM();
	PM(int);
	~PM();
	int getDistance(PM*);
	std::vector<Link*> getLinks(PM*);

//	Switch* getParent() const;
//	void setParent(Switch*);

	void assignBlock(const int);
	std::vector<int> getBlocks();
	//mapVM* assignMap(MapTask*);
	mapVM* assignMap(MapTask*, float);
	redVM* assignRed(RedTask*, float);

//	void setNumMap();
//	void setNumRed();
	void setMapVMs(std::vector<mapVM*>);
	void setRedVMs(std::vector<redVM*>);

	int getNumMap() const;
	int getNumRed() const;

//	Link* getLink();
//	void setLink(Link*);
//	int getID() const;

	void addData(int, int);
	
	int getData(int);

	bool ifMapSlotAvail();
	bool ifRedSlotAvail();

	int getNumAvailMapSlot();
	int getNumAvailRedSlot();
	void incMapNum();
	void incRedNum();



private:
//	Link* link;
	int numMap;
	int numRed;
	std::vector<mapVM*> mapVMs;
	std::vector<redVM*> redVMs;

	//blocks are the data blocks for each map task
	std::vector<int> blocks;
//	Switch* parent;
	std::map<int,int> distanceMap;

//	int PMID;
	std::map<int, int> dataSizeMap;
	int availNumMap;
	int availNumRed;
};

#endif /* PM_H_ */
