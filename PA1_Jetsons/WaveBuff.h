#ifndef WAVE_BUFF_H
#define WAVE_BUFF_H

#include <windows.h>
#include <mutex>
#include <condition_variable>

#define WAVE_BUFF_SIZE 1024*2  // play audio from 2k buffers


class WaveBuff
{
public:
	WaveBuff();

	void operator() (HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* buff);

	//---------------------------
	std::mutex mtx;
	std::condition_variable playReady;
	bool hasData;
};

#endif