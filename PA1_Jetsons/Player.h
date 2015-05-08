#ifndef PLAYER_H
#define PLAYER_H

#define BUFFER_COUNT 20  // number of playback buffers

#include <windows.h>

class Player
{
public:
	Player();

	void operator()();

	static LPSTR loadAudioBlock(const char* filename, DWORD* blockSize);

	static void writeAudioBlock(HWAVEOUT hWaveOut, LPSTR block, DWORD size);

	static void playback(const char* filename);

};

#endif