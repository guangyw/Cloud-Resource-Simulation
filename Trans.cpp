#include "Trans.h"


Trans::Trans(int ds, int band):active(true), dataSize(ds), availBand(band), remainningDataSize(ds)
{
}


Trans::~Trans(void)
{
}


void Trans::update(float t){
	remainningDataSize -= availBand * t;
	if (remainningDataSize <= 0 )
		finish();
}

void Trans::finish(){
	active = false;
}

void Trans::reduceBand(int b){
	availBand -= b;
}

bool Trans::ifActive() {return active;}

void Trans::updateBand(int band) {availBand = band;}