#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include "common.h"

using namespace std;

class Timer
{
public:
	Timer(){
		count = 0;
		this->start = clock();
		this->end = this->start;
	};
	~Timer(){};
	void stopTimer(){
		this->end = clock();
		this->elapsed_seconds = (end - start)/(double) CLOCKS_PER_SEC;
		this->start = this->end;
		count++;
	};
	void print(){
		stopTimer();
		LOGI("Time taken: %.8fs", elapsed_seconds);
	};
	
private:
	int count;
	clock_t start;
	clock_t end;
	double elapsed_seconds;
};

#endif // TIMER_H

