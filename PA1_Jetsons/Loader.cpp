#include "Loader.h"
#include <assert.h>

// initialize static member data
char* Loader::buff = 0;
DWORD Loader::size = 0;
bool Loader::hasData = false;
std::mutex Loader::mtx;
std::condition_variable Loader::loadReady;


Loader::Loader()
{
	buff = new char[FILE_BUFF_SIZE]; // 512k buffer
	hasData = false;
	size = 0;
}

void Loader::operator()()
{
	FILE* hFile = 0;
	char* filename = new char[12];
	DWORD tmpSize;

	// loop for 23 files
	for (int i = 0; i < 23; ++i)
	{	
		// set current file
		sprintf_s(filename, 12, "wave_%d.wav",i);

		// open file
		fopen_s(&hFile, filename, "r");
		assert(hFile != 0);

		// get size
		fseek(hFile, 0, SEEK_END);
		tmpSize = ftell(hFile);
		rewind(hFile);
		assert(tmpSize != 0);

		// set lock & wait
		std::unique_lock<std::mutex> lk(mtx);
		loadReady.wait(lk, [this]{return !hasData;});

		// load data
		fread(buff, size, 1, hFile);
		assert(buff != 0);
		// set size of data
		size = tmpSize;

		// free the lock
		lk.unlock();
	
		fclose(hFile);
		hasData = true;

	}

	// delete buffer
	delete(buff);
};