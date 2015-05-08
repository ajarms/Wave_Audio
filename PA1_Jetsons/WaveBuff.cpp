#include "WaveBuff.h"


WaveBuff::WaveBuff()
{
	hasData = false;
	mtx;
}

void WaveBuff::operator() (HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* buff)
{
	// create header
	WAVEHDR wHdr;

	// loop through buffers
	// each loop, wait to be signalled as filled
	int loop = 1;

	while (loop)
	{
		std::unique_lock<std::mutex> lk( mtx );
		playReady.wait(lk, [this]{return hasData;});

		// reset old header data
		memset(&wHdr, 0, sizeof(WAVEHDR));
		wHdr.dwBufferLength = WAVE_BUFF_SIZE;
		wHdr.lpData = buff;

		// prepare new header
		waveOutPrepareHeader(deviceHandle, &wHdr, sizeof(WAVEHDR));

		// waveout write
		//waveOutWrite(deviceHandle, &wHdr, sizeof(WAVEHDR));

		--loop;
	}

};

