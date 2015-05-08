#include "Coordinator.h"
#include "Loader.h"



Coordinator::Coordinator()
{
	buffA = new char[FILE_BUFF_SIZE]; // 512k
	buffB = new char[FILE_BUFF_SIZE]; // 512k
	sizeA = 0;
	sizeB = 0;
	isAfull = false;
	isBfull = false;
}

void Coordinator::operator()()
{
	int readFiles = 0;
	// loop for 23 files or requests in queue
	while(readFiles < 23 || false)
	{
		// check for new data to pull
		if (Loader::hasData)
		{
			// acquire lock on loader buffer
			std::unique_lock<std::mutex> lk(Loader::mtx);
			Loader::loadReady.wait(lk, []{return Loader::hasData;});
			// pull
			pullData();
			// free lock
			lk.unlock();
			// increment file count
			++readFiles;
		}
		// if requests queued
		if (requestQueue.empty())
		{
			// if data in push buffer
			if (true)
			{
				// push
			}
			// else
			else
			{
				// swap push buffer
				// if !data in that push buffer
				if (true)
				{
					// break - need to wait on loader
					break;
				}
				// push
			}
		}
	}
	// delete buffers
	delete(buffA);
	delete(buffB);
};


void Coordinator::pushRequest()
{
	// pop address from requestqueue
	WaveBuff* target = requestQueue.front();
	std::lock_guard<std::mutex> lk(target->mtx);
	requestQueue.pop();
	// write from 
}

void Coordinator::pullData()
{
	// grab data from the loading buffer
}