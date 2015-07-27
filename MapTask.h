/*
 * MapTask.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */
#include <iostream>
#ifndef MAPTASK_H_
#define MAPTASK_H_
#include <vector>
//#include "Job.h"
//#include "PM.h"

class Job;
class PM;

class MapTask {
public:
	MapTask();
	MapTask(int, Job*);

	~MapTask();

	MapTask(int, int);

	void addBlockPM(PM*);
	std::vector<PM*> getBlockPM() const;

	bool blockInPM(PM*);

	void setMem(int );
	void setCPU(int );
	int getMem() const;
	int getCPU() const;

	bool ifAssigned() const;
	void setAssigned(PM* );

	void setStartTime(float);

	void setID(int);
	int getID();

	bool ifFinished();
	void setFinished(std::ostream&, float);

	void genData(int);
	PM* getPM();

private:
	//PM that stores the needed data
	std::vector<PM*> blockPM;
	int memRequire;
	int CPURequire;
	bool assigned;
	float startTime;
	int ID;
	bool finished;
	Job* parentJob;
	PM* assignedPM;
};

#endif /* MAPTASK_H_ */
