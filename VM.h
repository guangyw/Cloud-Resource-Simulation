/*
 * VM.h
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#ifndef VM_H_
#define VM_H_

#include <iostream>
#include <vector>

class PM;
//class Link;

class VM {
public:
	VM();
	VM(int, int, PM*);
	~VM();

	PM* getPM();

	bool ifAlloc(const int, const int);
	bool alloc(const int, const int);

	virtual void releaseVM(std::ostream&, float) = 0;

//	bool ifAvail() {return vacant;}

	int getMem() const;
	int getCPU() const;
	void setMem(int);
	void setCPU(int);


protected:
	int Mem;
	int CPU;
	PM* pPM;
	bool vacant;
};

#endif /* VM_H_ */
