#include <iostream>
#include <fstream>
#include <SDL.h>
#pragma comment(lib,"SDL2.lib")
#undef main
using namespace std;

void  fill_audio(void* udata, Uint8* stream, int len) {
	//SDL 2.0
	SDL_memset(stream, 0, len);
	auto ifs = (ifstream*)udata;
	ifs->read((char*)stream, len);
	if (ifs->gcount() <= 0)
	{
		cout << "end" << endl;
		SDL_Quit();
	}
	cout << "." << endl;
}

int main(int argc, char* argv[])
{
	//Init
	SDL_Init(SDL_INIT_AUDIO);

	//SDL_AudioSpec
	SDL_AudioSpec wanted_spec;
	//wanted_spec.freq = 44100;
	wanted_spec.freq = 44100;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = 2;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;
	wanted_spec.callback = fill_audio;
	ifstream ifs("test_pcm.pcm", ios::binary);
	if (!ifs)
	{
		std::cerr << "cannot open test_pcm.pcm" << endl;
		return -1;
	}
	wanted_spec.userdata = &ifs;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		cerr << "can't open audio " << SDL_GetError() << endl;
		//printf("can't open audio.\n");
		return -1;
	}

	//Play
	SDL_PauseAudio(0);

	getchar();
	SDL_Quit();

	return 0;
}