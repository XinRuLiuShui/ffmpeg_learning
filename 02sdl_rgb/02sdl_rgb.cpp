#include <iostream>
#include <SDL.h>

using namespace std;

#pragma comment(lib,"SDL2.lib")
#undef main

int main(int argc, char* argv[])
{
	int w = 800;
	int h = 600;

	if (SDL_Init(SDL_INIT_VIDEO))
	{
		cout << SDL_GetError() << endl;
		return -1;
	}

	auto screen = SDL_CreateWindow(
			"test",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			w,
			h,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
	if (!screen)
	{
		cout << SDL_GetError() << endl;
		return -1;
	}
	

	auto render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
	if (!render)
	{
		cout << SDL_GetError() << endl;
		return -1;
	}

	auto texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		w, h
	);
	if (!texture)
	{
		cout << SDL_GetError() << endl;
		return -1;
	}

	shared_ptr<unsigned char> rgb(new unsigned char[w * h * 4]);
	auto r = rgb.get();
	int temp = 255;
	int frame = 0;
	bool loop = false;
	int dir = -1;
	for (;;)
	{
		SDL_Event ev;
		SDL_WaitEventTimeout(&ev, 10);
		if (ev.type == SDL_QUIT)
		{
			SDL_DestroyWindow(screen);
			break;
		}
		//temp = abs((frame & 511) - 255);
		//frame++;
		
		/*if (loop)
		{
			temp++;
			if (temp==0)
			{
				temp = 255;
				loop = false;
			}
		}
		else {
			temp--;
			if (temp==255)
			{
				temp = 0;
				loop = true;
			}
		}*/
		temp += dir;
		if (temp == 0 || temp == 255)
		{
			dir = -dir;
		}
		for (int j = 0; j < h; j++)
		{
			int b = j * w * 4;
			for (int i = 0; i < w * 4; i += 4)
			{
				r[b + i] = temp;		//R
				r[b + i + 1] = 0;	//G
				r[b + i + 2] = 0;	//B
				r[b + i + 3] = 0;	//A
			}
		}

		SDL_UpdateTexture(texture, NULL, r, w * 4);

		SDL_RenderClear(render);

		SDL_Rect sdl_rect;
		sdl_rect.x = 0;
		sdl_rect.y = 0;
		sdl_rect.w = w;
		sdl_rect.h = h;

		SDL_RenderCopy(render, texture, NULL, &sdl_rect);

		SDL_RenderPresent(render);
	}
	

	getchar();
	return 0;

}