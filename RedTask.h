/*
 * RedTask.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */
#include <iostream>
#ifndef REDTASK_H_
#define REDTASK_H_
class PM;
class Job;

class RedTask {
public:
	
	virtual ~RedTask();

	RedTask();
	RedTask(int, Job*);
	RedTask(int, int);
	void setMem(int mem);
	void setCPU(int cpu);
	int getMem() const;
	int getCPU() const;

	bool ifAssigned() const;
	void setAssigned(PM*);

	void setStartTime(float);

	void setID(int);
	int getID();

	bool ifFinished();
	void setFinished(std::ostream&, float);
	Job* getParentJob();

private:
	//PM that stores the needed data
	int memRequire;
	int CPURequire;
	bool assigned;
	float startTime;
	int ID;
	bool finished;
	Job* parentJob;
	PM* assignedPM;
};

#endif /* REDTASK_H_ */
