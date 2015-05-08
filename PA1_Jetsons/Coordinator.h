#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "WaveBuff.h"
#include <queue>

#define LOAD_BUFF_SIZE 524288; // 512k

class Coordinator
{
public:
	Coordinator();

	void operator() ();

	void pushRequest();

	void pullData();

	//---------------------------------
	// audio data buffers
	char* buffA;
	char* buffB;
	// current target to be pulled into / pushed out of
	char* currIn;
	char* currOut;
	// request queue to load wave buffers
	std::queue<WaveBuff*> requestQueue;
	// mutexes for buffers
	std::mutex mtxA;
	std::mutex mtxB;
	// condition variables for buffer use
	std::condition_variable condA;
	std::condition_variable condB;
	// amount of data in buffers
	size_t sizeA;
	size_t sizeB;
	// flags for the buffers
	bool isAfull;
	bool isBfull;
};

#endif