/*
 * mapVM.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#ifndef MAPVM_H_
#define MAPVM_H_

#include "VM.h"
//#include "MapTask.h"
class PM;
class MapTask;

class mapVM: public VM {
public:
	mapVM();

	mapVM(int, int, PM*);

	~mapVM();

	bool allocMap(MapTask*);

	void releaseVM(std::ostream&, float);

	bool ifAvail();


private:
	MapTask* pMapTask;
};

#endif /* MAPVM_H_ */
