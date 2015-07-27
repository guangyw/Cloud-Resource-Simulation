/*
 * Job.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#ifndef JOB_H_
#define JOB_H_

#include <vector>
//#include "MapTask.h"
//#include "RedTask.h"
class MapTask;
class RedTask;


class Job {
public:
	Job();
	Job(int, int, int, int);
	virtual ~Job();
	void setMapTasks(std::vector<MapTask*>);
	std::vector<MapTask*> getMapTasks();

	void setRedTasks(std::vector<RedTask*>);
	std::vector<RedTask*> getRedTasks();

	//void finishJob();
	bool ifFinished();

	void skip();
	int getSkip() const;

	//bool operator< (Job* pJob);

	MapTask* getUnassignedMap();
	RedTask* getUnassignedRed();

	std::vector<MapTask*> getAssignedMaps();

	int getNumRunningMapTasks();

	int getNumRunningRedTasks();

	int getNumRunningTasks();

	int getNumAssignedTasks();

	int getNumFinishedMapTasks();

	int getNumFinishedRedTasks();

	bool ifFinishedMap();

	bool ifFinishedRed();

	void reduceNumMapTask();
	void reduceNumRedTask();

	int getSubmitTime();
	int getMapBytes();
	int getShuffleBytes();

	int getNumMapTasks();
	int getNumRedTasks();

	int getID();

	int getTotalMapNum();
	int getTotalRedNum();
	int getTotalTaskNum();
	float getProgress();
	bool ifFinishedAssign();
	bool ifFinishedAssignMap();
	bool ifFinishedAssignRed();
	std::vector<MapTask*> getUnlaunchedMaps();
	void launchMap(MapTask*);


private:
	int jobID;
	std::vector<MapTask*> mapTasks;
	std::vector<RedTask*> redTasks;
	bool finished;
	
	//This attribute is for fair scheduler
	int skipTimes;
	int numMapTask;
	int numRedTask;
	int submitTime;
	int mapInputBytes;
	int shuffleBytes;
	int totalMapNum;
	int totalRedNum;
	//std::vector<MapTask*> unlaunchedMaps;
	//std::vector<RedTask*> unlaunchedReds;

};

#endif /* JOB_H_ */
