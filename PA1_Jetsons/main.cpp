#include <thread>
#include "Coordinator.h"
#include "WaveBuff.h"
#include "Loader.h"
#include "Player.h"

#include <stdio.h>
#include <assert.h>



void loadingThread();
void playbackThread();
void coordinatorThread();
void waveBuffThread(HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* block);

std::condition_variable loadCond;
std::mutex loaderMtx;
bool loaderHasData = false;

void main()
{
	std::thread loader(loadingThread);

	std::thread coordinator(coordinatorThread);

	std::thread player(playbackThread);

	Player::playback("wave_0.wav");

	if (loader.joinable())
		loader.join();
	if (coordinator.joinable())
		coordinator.join();
	if (player.joinable())
		player.join();
};

void loadingThread()
{
	/*
	strange problem; the functor WaveBuff cannot be passed directly into
	the thread constructor b/c it has a mutex & condition variable.

	when a functor is passed into thread construction it's copied into the
	thread, and copy constructors for mutexes and condition variables are
	undefined.  this function is a work-around to avoid copy constructors
	*/
	Loader ld;
	ld();
}

void coordinatorThread()
{
	/*
	strange problem; the functor WaveBuff cannot be passed directly into
	the thread constructor b/c it has a mutex & condition variable.

	when a functor is passed into thread construction it's copied into the
	thread, and copy constructors for mutexes and condition variables are
	undefined.  this function is a work-around to avoid copy constructors
	*/
	Coordinator cd;
	cd();
}

void playbackThread()
{
	/*
	strange problem; the functor WaveBuff cannot be passed directly into
	the thread constructor b/c it has a mutex & condition variable.

	when a functor is passed into thread construction it's copied into the
	thread, and copy constructors for mutexes and condition variables are
	undefined.  this function is a work-around to avoid copy constructors
	*/
	Player pl;
	pl();
}


static void CALLBACK playbackFinished(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	// only function if playback is done
	if(uMsg != WOM_DONE)
		return;

	// push dwParam1 (a wavebuffer identifier??) onto a playback queue?
	// notify playback that something's done

}


// need to set up request queue for coordinator
// playback thread pushes block pointers onto the queue

// need to set up callback function for wave buffers
// sets condition var to need filling?
// signal playback to push a request for data

// need to set up locks on the main audio buffer


//void playbackThread()
//{
//	// audio blocks
//	char* buff;
//
//	// setup waveout format
//	HWAVEOUT hWaveOut;
//	WAVEFORMATEX wfx;
//	wfx.nSamplesPerSec = 22050; /* sample rate */
//	wfx.wBitsPerSample = 16; /* sample size */
//	wfx.nChannels= 2; /* channels*/
//	wfx.cbSize = 0; /* size of _extra_ info */
//	wfx.wFormatTag = WAVE_FORMAT_PCM;
//	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
//	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
//
//	// open waveout
//	if((waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, NULL, NULL, NULL)) != MMSYSERR_NOERROR)
//		ExitProcess(1);
//
//	// spawn waveBuff threads
//	// allocate their buffers
//	buff = new char[WAVE_BUFF_SIZE * BUFFER_COUNT];
//	std::thread waves[BUFFER_COUNT];
//	//WaveBuff* wBuffs = new WaveBuff[BUFFER_COUNT];
//	for(int i = 0; i < BUFFER_COUNT; ++i)
//	{
//		//waves[i] = std::thread((WaveBuff()), hWaveOut, wfx, buff);
//		waves[i] = std::thread(&waveBuffThread, hWaveOut, wfx, buff);
//		buff += WAVE_BUFF_SIZE;
//	}
//
//	//-------------------------------------------
//	// wait for signals from wave buffers
//	// push wavebuff pointers onto coordinator request queue
//	// notify coordinator
//	//-------------------------------------------
//
//	// join buff threads
//	for(int i = 0; i < BUFFER_COUNT; ++i)
//	{
//		if (waves[i].joinable())
//		{
//			waves[i].join();
//		}
//	}
//	// close waveout
//	waveOutClose(hWaveOut);
//};

//void coordinatorThread()
//{
//	// create A / B buffers
//	char* buffA = new char[FILE_BUFF_SIZE];
//	char* buffB = new char[FILE_BUFF_SIZE];
//
//	int readFiles = 0;
//	// loop for 23 files or requests in queue
//	while(readFiles < 23 || false)
//	{
//		// if loader has data
//		if (true)
//		{
//			// pull
//			// increment file count
//			++readFiles;
//		}
//		// if requests queued
//		if (true)
//		{
//			// if data in push buffer
//			if (true)
//			{
//				// push
//			}
//			// else
//			else
//			{
//				// swap push buffer
//				// if !data in that push buffer
//				if (true)
//				{
//					// break - need to wait on loader
//					break;
//				}
//				// push
//			}
//		}
//	}
//	// delete buffers
//	delete(buffA);
//	delete(buffB);
//};

//void waveBuffThread(HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* block)
//{
//	// create a buffer
//	//char* buff = new char[WAVE_BUFF_SIZE];  // pass this in instead
//
//	// create header
//	WAVEHDR wHdr;
//
//	// loop through buffers
//	// each loop, wait to be signalled as filled
//	int loop = 1;
//
//	while (loop)
//	{
//		// reset old header data
//		memset(&wHdr, 0, sizeof(WAVEHDR));
//		wHdr.dwBufferLength = WAVE_BUFF_SIZE;
//		wHdr.lpData = block;
//
//		// prepare new header
//		waveOutPrepareHeader(deviceHandle, &wHdr, sizeof(WAVEHDR));
//
//		// waveout write
//		//waveOutWrite(deviceHandle, &wHdr, sizeof(WAVEHDR));
//
//		--loop;
//	}
//	
//};

//void loadingThread()
//{
//	// create buffer
//	char* const buff = new char[FILE_BUFF_SIZE]; // 512k buffer
//	DWORD size = 0;
//	FILE* hFile = 0;
//	char* filename = new char[11];
//	// loop for 23 files
//	for (int i = 0; i < 23; ++i)
//	{	
//		// set current file
//		sprintf_s(filename, 11, "wave_%d.wav",i+1);
//
//		// set lock & wait
//		std::unique_lock<std::mutex> lk(loaderMtx);
//		loadCond.wait(lk, []{return !loaderHasData;});
//
//		// open file
//		fopen_s(&hFile, filename, "r");
//		assert(hFile != 0);
//
//		// get size
//		fseek(hFile, 0, SEEK_END);
//		size = ftell(hFile);
//		rewind(hFile);
//		assert(size != 0); 
//
//		// load data
//		fread(buff, size, 1, hFile);
//		assert(buff != 0);
//	
//		fclose(hFile);
//		loaderHasData = true;
//	}
//
//	// delete buffer
//	delete(buff);
//};


