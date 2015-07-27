/*
 * Switch.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: guangyw
 */

#include "Switch.h"
#include "Link.h"

Switch::Switch():ID(0), link(), parent(NULL){};
Switch::Switch(int id):ID(id), link(), parent(NULL){};

//Switch::Switch(int id, int band):ID(id),bandwidth(band), link(), parent(NULL){}

Switch::~Switch() {
	// TODO Auto-generated destructor stub
}

//Switch::Switch(int id, int bandwidth, std::vector<Link*>& links, Switch* s):ID(id), bandwidth(bandwidth), links(), parent(s){
//	std::vector<Link*>::const_iterator it = links.begin();
//	while(it != links.end()){
//		this->links.push_back(*it);
//		++it;
//	}
//}

void Switch::setLink(Link* pLink){
	link = pLink;
}

Switch* Switch::getParent(){ return parent;}
void Switch::setParent(Switch* pSwitch){this->parent = pSwitch;}
//int Switch::getBandwidth(){return bandwidth;}
//std::vector<Link*>& Switch::getLinks(){return links;}
Link* Switch::getLink(){return link;}

//Link* Switch::getLink(PM* pm){
//	Link* temp = pm->getLink();
//	std::vector<Link*>::const_iterator it = links.begin();
//	while(it != links.end()){
//		if (*it == temp)
//	}
//}

bool Switch::operator==(Switch pS){
	return ID == pS.getID();
}

int Switch::getID() {return ID;}