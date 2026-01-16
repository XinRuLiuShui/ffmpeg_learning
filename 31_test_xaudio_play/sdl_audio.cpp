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
	//audio->set_speed(2);
	std::ifstream ifs("test_pcm.pcm", std::ios::binary);
	if (!ifs) return -1;

	unsigned char buf[1024] = { 0 };
	audio->set_volume(50);
	for (;;) {
		ifs.read((char*)(buf), sizeof(buf));
		if (ifs.gcount() <= 0) break;          // ¶Áµ½ EOF ÍË³ö
		audio->Push(buf, (ifs.gcount()));


	}
	getchar();
	audio->set_speed(0.5);
	getchar();
	audio->set_speed(2);
	getchar();
	audio->close();
	return 0;

}