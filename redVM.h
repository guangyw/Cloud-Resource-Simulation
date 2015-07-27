/*
 * redVM.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#ifndef REDVM_H_
#define REDVM_H_
#include "VM.h"
class RedTask;
class PM;

class redVM : public VM {
public:

	redVM();

	redVM(int, int, PM*);

	~redVM();

	bool allocRed(RedTask*);

	void releaseVM(std::ostream&, float);

	bool ifAvail();


private:
	RedTask* pRedTask;
};

#endif /* REDVM_H_ */
