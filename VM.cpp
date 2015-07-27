/*
 * VM.cpp
 *
 *  Created on: Sep 26, 2013
 *      Author: guangyw
 */

#include "VM.h"
#include "PM.h"

VM::VM():Mem(1000), CPU(500), pPM(), vacant(true){}

VM::VM(int mem, int cpu, PM* pm)
	: Mem(mem), CPU(cpu), pPM(pm), vacant(true){
	}

VM::~VM() {
	// TODO Auto-generated destructor stub
}

PM* VM::getPM() {return pPM;}

bool VM::ifAlloc(const int mem, const int cpu){
	if(vacant)
		return (Mem > mem) && (CPU > cpu);
	else
		return false;
}

bool VM::alloc(const int mem, const int cpu) {
	if (ifAlloc(mem, cpu)){
		vacant = false;
		return true;
	}else{
		std::cout<< "unable to alloc, not enough memory" <<std::endl;
		return false;
	}
}

int VM::getMem() const{return Mem;}
int VM::getCPU() const{return CPU;}
void VM::setMem(int mem){Mem = mem;}
void VM::setCPU(int cpu){CPU = cpu;}
