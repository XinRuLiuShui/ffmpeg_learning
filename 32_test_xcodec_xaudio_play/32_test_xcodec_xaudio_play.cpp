#include "xaudio_play.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	string in_file = "v1080.mp4";
	XDecodeTask decode_task;
	XDemuxTask demux_task;
	if (!demux_task.Open("v1080.mp4"))
	{
		cerr << "demux_task Open failed" << endl;
		return -1;
	}
	
	auto vpara = demux_task.CopyVideoPara();
	auto apara = demux_task.CopyAudioPara();
	if (!decode_task.Open(apara->para))
	{
		cerr << "decode_task Open failed" << endl;
		return -2;
	}
	demux_task.set_next(&decode_task);

	decode_task.set_video_stream_index(demux_task.video_index());
	decode_task.set_audio_stream_index(demux_task.audio_index());
	decode_task.set_stream_index(demux_task.audio_index());
	decode_task.set_is_frame_cache(true);

	auto audio = XAudioPlay::Instance();

	if (!audio->Open(apara->para))
	{
		cerr << "audio Open failed" << endl;
		return -3;
	}
	demux_task.Start();
	decode_task.Start();

	for (;;)
	{
		auto f = decode_task.GetFrame();
		if (!f)
		{
			MSleep(10);
			continue;
		}
		audio->Push(f);
		XFreeFrame(&f);
	}
	getchar();
}