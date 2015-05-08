

// below from wave demo

#include <stdio.h>
#include <assert.h>
#include <thread>
#include "Player.h"
#include "WaveBuff.h"

void waveBuffThread(HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* block);

Player::Player()
{}

void Player::operator() ()
{
	// audio blocks
	char* buff;

	// setup waveout format
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfx;
	wfx.nSamplesPerSec = 22050; /* sample rate */
	wfx.wBitsPerSample = 16; /* sample size */
	wfx.nChannels= 2; /* channels*/
	wfx.cbSize = 0; /* size of _extra_ info */
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

	// open waveout
	if((waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, NULL, NULL, NULL)) != MMSYSERR_NOERROR)
		ExitProcess(1);

	// spawn waveBuff threads
	// give them their buffers
	buff = new char[WAVE_BUFF_SIZE * BUFFER_COUNT];
	std::thread waves[BUFFER_COUNT];
	for(int i = 0; i < BUFFER_COUNT; ++i)
	{
		waves[i] = std::thread(&waveBuffThread, hWaveOut, wfx, buff);
		buff += WAVE_BUFF_SIZE;
	}

	//-------------------------------------------
	// wait for signals from wave buffers
	// push wavebuff pointers onto coordinator request queue
	// notify coordinator
	//-------------------------------------------

	// join buff threads
	for(int i = 0; i < BUFFER_COUNT; ++i)
	{
		if (waves[i].joinable())
		{
			waves[i].join();
		}
	}
	// close waveout
	waveOutClose(hWaveOut);
};


void waveBuffThread(HWAVEOUT deviceHandle, WAVEFORMATEX wfx, char* block)
{
	/*
	strange problem; the functor WaveBuff cannot be passed directly into
	the thread constructor b/c it has a mutex & condition variable.

	when a functor is passed into thread construction it's copied into the
	thread, and copy constructors for mutexes and condition variables are
	undefined.  this function is a work-around to avoid copy constructors
	*/
	WaveBuff wb;
	wb(deviceHandle, wfx, block);
}


void Player::playback(const char* filename)
{
	HWAVEOUT hWaveOut; /* device handle */
	WAVEFORMATEX wfx; /* wave audio format info */
	MMRESULT result;/* for waveOut return values */
	LPSTR block;/* pointer to the block */
	DWORD blockSize;/* holds the size of the block */
		/*
		* first we need to set up the WAVEFORMATEX structure. 
		* the structure describes the format of the audio.
		*/
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nSamplesPerSec = 22050;
	wfx.wBitsPerSample = 16;
	wfx.nChannels = 2;
	wfx.cbSize = 0;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
		/*
		* try to open the default wave device. WAVE_MAPPER is
		* a constant defined in mmsystem.h, it always points to the
		* default wave device on the system (some people have 2 or
		* more sound cards).
		*/
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	assert(result == MMSYSERR_NOERROR);
	/*
		* device is now open
		* load and play the block of audio
		*/
	block = loadAudioBlock(filename, &blockSize);
	assert(block != 0);

	writeAudioBlock(hWaveOut, block, blockSize);
	waveOutClose(hWaveOut);
}



void Player::writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size)
{
	MMRESULT result;

	WAVEHDR header;
		/*
		* initialise the block header with the size
		* and pointer.
		*/
	memset(&header, 0, sizeof(WAVEHDR));
	//ZeroMemory(&header, sizeof(WAVEHDR));
	header.dwBufferLength = size;
	header.lpData = block;
		/*
		* prepare the block for playback
		*/
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
		/*
		* write the block to the device. waveOutWrite returns immediately
		* unless a synchronous driver is used (not often).
		*/
	result = waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	assert(result == MMSYSERR_NOERROR);
		/*
		* wait a while for the block to play then start trying
		* to unprepare the header. this will fail until the block has
		* played.
		*/
	Sleep(7500);
};


char* Player::loadAudioBlock(const char* filename, DWORD* blockSize)
{
	FILE* hFile = 0;
	DWORD size = 0;
	DWORD readBytes = 0;
	void* block = 0;

	fopen_s(&hFile, filename, "r");
	assert(hFile != 0);

	fseek(hFile, 0, SEEK_END);
	size = ftell(hFile);
	rewind(hFile);

	assert(size != 0); 
	
	block = new char[size];

	fread(block, size, 1, hFile);

	assert(block != 0);
	
	fclose(hFile);
	*blockSize = size;
	return (char*)block;
}

