#ifndef LOADER_H
#define LOADER_H

#include <windows.h>
#include <mutex>
#include <condition_variable>

#define FILE_BUFF_SIZE 1024*512 // load files into 512k buffers

class Loader
{
public:
	Loader();

	void operator()();
	//--------------------------
	static char* buff;
	static std::mutex mtx; //protects the buffer
	static std::condition_variable loadReady;
	static DWORD size; //amount of data in buffer
	static bool hasData; //flag if data is 'fresh'
};

#endif
