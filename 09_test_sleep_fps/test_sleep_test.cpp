#include <iostream>
#include <ctime>
#include <thread>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")



using namespace std;

void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			break;
		}
	}
}
int main()
{
	timeBeginPeriod(1);
	auto fps = 0;
	int beg = clock();
	for (;;)
	{
		int tmp = clock();
		fps++;
		MSleep(10);
		cout << clock() - tmp << " " << flush;
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= 1000)
		{
			cout << "MSleep fps:" << fps << endl;
			break;
		}
	}
	return 0;
}