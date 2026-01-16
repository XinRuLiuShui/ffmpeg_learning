#include "xaudio_play.h"
#include <iostream>
#include <fstream>
//#include <SDL.h>
//#pragma comment(lib,"SDL2.lib")
//#undef main
using namespace std;


int main(int argc, char* argv[])
{
	auto audio = XAudioPlay::Instance();
	XAudioSpec spec;
	spec.freq = 44100;
	audio->Open(spec);

	std::ifstream ifs("test_pcm.pcm", std::ios::binary);
	if (!ifs) return -1;

	unsigned char buf[1024] = { 0 };
	for (;;) {
		ifs.read((char*)(buf), sizeof(buf));
		if (ifs.gcount() <= 0) break;          // ¶Áµ½ EOF ÍË³ö
		audio->Push(buf, (ifs.gcount()));
	}
	getchar();
	return 0;

}