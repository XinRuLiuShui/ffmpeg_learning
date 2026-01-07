#include <iostream>
#include "xdemux_task.h"
using namespace std;

int main(int argc, char* argv[])
{
	cout << "test xcodec lib" << endl;
	XDemuxTask demux_task;
	demux_task.Open("911Mothers_2010W-480p.mp4");
	demux_task.Start();
	getchar();
	return 0;
}