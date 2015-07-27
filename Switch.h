/*
 * Switch.h
 *
 *  Created on: Sep 30, 2013
 *      Author: guangyw
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include <vector>
class PM;
class Link;

class Switch {
public:
	Switch();
	Switch(int);
//	Switch(int, int, Link*, Switch*);
//	Switch(int, int);
	virtual ~Switch();

	Switch* getParent();
	void setParent(Switch*);
//	int getBandwidth();
	Link* getLink();

	void setLink(Link*);
	bool operator==(Switch);

	int getID();
//	Link* getLink(PM*);
//	Link* getLink(Switch*);

protected:
	int ID;
	Link* link;
	Switch* parent;
};

#endif /* SWITCH_H_ */
