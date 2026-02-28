


> Written with [StackEdit](https://stackedit.io/).

## `AVFormatContext`
`AVFormatContext` 是 FFmpeg 中的一个重要结构体，主要用于处理多媒体文件的解封装（demuxing）和封装（muxing）。它存储了媒体文件的各种信息，比如流的格式、编码类型、流的数量等。

### 主要功能：

1.  **打开和关闭文件**：`AVFormatContext` 可以用来打开媒体文件或者流。通过调用 `avformat_open_input()` 函数来打开一个输入文件，或者通过 `avformat_close_input()` 来关闭它。
    
2.  **读取媒体信息**：它包含了文件的格式信息，如视频、音频流的数量、流的解码器信息、文件的元数据等。通过 `avformat_find_stream_info()` 来读取媒体文件的流信息。
    
3.  **流的管理**：每个媒体文件可能包含多个流（如视频流、音频流、字幕流等）。`AVFormatContext` 存储这些流的信息。每个流都由 `AVStream` 结构表示，`AVStream` 包含了流的各种属性，比如编解码器信息、时长、帧率等。
    
4.  **流的解封装和封装**：通过 `AVFormatContext`，你可以将多媒体数据从一个容器格式（比如 MP4、MKV 等）解封装成原始的数据流，或者将原始数据流封装成目标格式。通过 `av_read_frame()` 函数来从输入流中读取帧数据。
    
5.  **读取/写入元数据**：它可以存储和访问媒体文件的元数据，如标题、作者、版权等信息。
    

### 结构体定义
``` C
typedef struct AVFormatContext {
    const AVClass *av_class;
    int flags;
    AVIOContext *pb;                // 输入/输出控制结构，管理读取/写入
    const AVInputFormat *iformat;   // 输入文件格式
    const AVOutputFormat *oformat;  // 输出文件格式
    AVStream **streams;             // 流数组
    int nb_streams;                 // 流的数量
    int metadata;                   // 元数据
    ...
} AVFormatContext;
```
### 常用函数

1.  **`avformat_open_input()`**：打开媒体文件或流。
    
2.  **`avformat_find_stream_info()`**：获取流信息。
    
3.  **`avformat_read_frame()`**：读取媒体文件中的一帧数据。
    
4.  **`avformat_write_header()`**：写入媒体文件头（用于封装输出）。
    
5.  **`avformat_close_input()`**：关闭文件输入。
    

### 总结

`AVFormatContext` 是 FFmpeg 用来处理媒体文件格式、流信息和元数据的核心结构。它通过流的方式提供对多种音视频格式的支持，并能够处理解码和编码的各种任务。

## `AVCodecParameters`
`AVCodecParameters` 是 FFmpeg 中的一个结构体，用于存储与媒体流相关的编解码器参数信息。它通常与 `AVStream` 一起使用，后者表示一个媒体流（如视频流、音频流）。`AVCodecParameters` 主要包含了媒体流的编码信息，例如音视频的编码格式、分辨率、帧率、比特率、采样率等。

### 主要功能：

1.  **编解码器参数存储**：`AVCodecParameters` 存储了编码器或解码器所需要的所有参数。这些参数对于编解码操作至关重要。比如视频的分辨率、颜色格式、帧率，或者音频的采样率、通道数、编码类型等。
    
2.  **与 `AVCodec` 配合使用**：`AVCodecParameters` 提供的是流的编码器参数，而 `AVCodec` 则是处理实际编码或解码操作的结构体。通过 `AVCodecParameters` 中的编码参数，`AVCodec` 可以进行正确的编解码。
    
3.  **多媒体文件流信息**：通过 `AVCodecParameters` 可以了解流的具体编码信息，这对于解码和封装多媒体文件非常重要。

### 结构体定义
``` C
typedef struct AVCodecParameters {
    int codec_type;             // 编解码器类型，如 AVMEDIA_TYPE_VIDEO, AVMEDIA_TYPE_AUDIO 等
    int codec_id;               // 编解码器 ID，如 AV_CODEC_ID_H264, AV_CODEC_ID_MP3 等
    int bit_rate;               // 比特率（单位：比特/秒）
    int bits_per_coded_sample;  // 每个编码样本的位数
    int channel_layout;         // 音频通道布局（用于音频流）
    int channels;               // 音频流的通道数
    int sample_rate;            // 音频流的采样率
    int width, height;          // 视频流的宽度和高度（像素）
    int format;                 // 媒体流的像素格式或音频采样格式
    int profile;                // 编解码器配置文件（如 H264 的 Profile）
    int level;                  // 编解码器级别（如 H264 的 Level）
    uint8_t *extradata;         // 编解码器附加数据（如 h264 的 SPS/PPS）
    int extradata_size;         // 附加数据的大小
    ...
} AVCodecParameters;
```
### 关键字段解释

1.  **`codec_type`**：表示流的类型，比如音频、视频或字幕等。常见的值有：
    
    -   `AVMEDIA_TYPE_AUDIO`：音频流
        
    -   `AVMEDIA_TYPE_VIDEO`：视频流
        
    -   `AVMEDIA_TYPE_SUBTITLE`：字幕流
        
2.  **`codec_id`**：表示流使用的编码器 ID。例如：
    
    -   `AV_CODEC_ID_H264`：H.264 编码
        
    -   `AV_CODEC_ID_AAC`：AAC 音频编码
        
    -   `AV_CODEC_ID_MP3`：MP3 音频编码
        
3.  **`bit_rate`**：表示流的比特率（单位：比特/秒）。视频或音频流的质量和文件大小与比特率密切相关。
    
4.  **`width` 和 `height`**：仅适用于视频流，表示视频帧的宽度和高度（单位：像素）。
    
5.  **`channels` 和 `sample_rate`**：仅适用于音频流，分别表示音频流的通道数和采样率。
    
6.  **`extradata` 和 `extradata_size`**：对于一些编码格式，除了基础的参数外，还需要额外的编码信息（如 H.264 的 SPS/PPS），这些信息存储在 `extradata` 字段中。
    

### 常见用途

1.  **获取流的编码信息**：在读取多媒体文件时，通过 `AVStream` 获取流的编码参数，通常可以通过 `AVStream->codecpar` 来访问。
    
2.  **配置解码器**：在解码视频或音频流时，`AVCodecParameters` 提供了所需的编解码器参数，使得解码器能够正确处理输入的流数据。
    
3.  **封装文件**：在进行文件封装时（比如转码或录制视频），需要通过 `AVCodecParameters` 中的编码参数来选择正确的编码格式。

### 例子：如何使用 `AVCodecParameters`

假设我们已经打开了一个视频文件并且获取了 `AVStream`：
``` c
AVFormatContext *fmt_ctx = NULL;
if (avformat_open_input(&fmt_ctx, "input.mp4", NULL, NULL) < 0) {
    // 打开文件失败
}
if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
    // 获取流信息失败
}

// 获取第一个视频流
AVStream *video_stream = fmt_ctx->streams[0];

// 获取该流的编解码器参数
AVCodecParameters *codecpar = video_stream->codecpar;
printf("Codec ID: %d\n", codecpar->codec_id);
printf("Width: %d, Height: %d\n", codecpar->width, codecpar->height);
```
### 总结

`AVCodecParameters` 是 FFmpeg 中用于描述和存储多媒体流编码参数的结构体。它包含了流的格式、编码信息、分辨率、比特率等关键信息，能够帮助解码器正确处理流数据，也能够在进行流封装或转码时提供必要的参数。

## 音视频同步问题

时间戳重缩放操作是多媒体处理中的一个重要步骤，尤其是在处理不同媒体格式、不同流或不同时间基时。时间戳重缩放的目的主要是为了确保在不同的系统或流之间正确同步，并保证视频或音频流的时间信息在解码、处理和播放过程中保持一致。具体来说，重缩放操作有以下几个关键原因：

### 1. **不同流的时间基不同**

不同的媒体流（如视频流、音频流）可能有不同的时间基（`timebase`）。时间基定义了时间戳单位，通常是一个分数形式（`AVRational`），表示一个单位时间的大小。

-   **视频流**：可能以帧为单位进行计时，时间基通常为某个帧率的倒数，例如 `1/30`（表示每秒 30 帧）。
    
-   **音频流**：可能以采样为单位进行计时，时间基通常为采样率的倒数，例如 `1/44100`（表示每秒 44,100 个采样）。
    

在解封装过程中，流的时间基需要转换为统一的时间基，以便正确地进行同步。例如，如果你有一个视频流和一个音频流，视频流可能是以帧为单位，而音频流是以采样为单位。为了让它们在同一时刻正确播放，就需要对它们的时间戳进行重缩放。

### 2. **时间戳单位的统一**

不同的流可能在处理过程中使用不同的时间戳单位。例如，某些情况下，视频流的时间戳可能以帧为单位，而音频流的时间戳可能以毫秒为单位。为了将这些时间戳整合在一起并保持同步，必须将它们转换成相同的单位，这通常需要重缩放。

举个例子，假设一个视频流的时间基为 `1/30`，即每帧的时间为 `1/30` 秒，而音频流的时间基为 `1/44100`，即每个采样的时间为 `1/44100` 秒。如果我们要对比视频和音频的时间戳，就需要将它们都转换为统一的单位，比如毫秒或秒。通过重缩放，可以确保它们的时间戳正确同步。

### 3. **确保同步**

时间戳重缩放操作帮助确保不同流（如音频和视频）在同一时刻正确地同步播放。例如，视频帧可能每 1/30 秒更新一次，而音频帧可能每 1/44100 秒更新一次。当这些流被混合或播放时，必须确保它们在相同的时间轴上准确对齐。这时，重缩放时间戳就显得非常重要。

-   如果没有重缩放，视频流和音频流可能会因为时间基不同而错位，导致播放时音视频不同步（例如，视频帧提前或延迟）。
    
-   通过重缩放时间戳，可以将音频和视频的时间信息转换成一个统一的时间基（例如，毫秒），从而确保它们正确地同步播放。
    

### 4. **帧率与时间基的差异**

视频流的帧率通常是一个固定的值，比如每秒 30 帧，而音频流的采样率通常是每秒 44,100 次采样。这两个参数之间并没有直接的关系，视频帧与音频样本的时序关系也不直接匹配。因此，重缩放操作可以帮助将视频帧的时间戳（通常以帧为单位）转换为与音频流的时间戳相同的单位，通常是秒或毫秒，从而便于流的同步。

### 5. **处理不同格式的转换**

在不同格式之间转换时，时间基可能会发生变化。例如，某个文件的音频流使用一个时间基，而另一个文件的音频流可能使用不同的时间基。为了在不同格式之间正确地处理流，时间戳重缩放是必要的。

### 6. **缓冲区和播放的时间调整**

在实时流媒体应用中（如视频会议、直播等），时间戳的准确性至关重要。流的传输过程中，可能会发生缓冲，或者由于带宽波动，某些流可能需要提前或延迟处理。为了避免不同流之间的时间差异，需要对时间戳进行适当的重缩放，以便在播放时保证音视频同步。

### 7. **跨平台的兼容性**

不同的操作系统、硬件平台和媒体播放器可能对时间戳的处理有所不同。例如，一些平台可能使用毫秒为单位，而另一些平台可能使用微秒、秒或帧作为单位。通过重缩放，可以确保在不同平台上的兼容性，使得时间戳在各个平台之间的转换无误。

----------

### 总结

时间戳重缩放操作的核心目的是为了在多媒体处理中解决时间基不一致、流不同步等问题。通过调整不同流的时间基，使得它们能够在同一时刻正确地同步播放，并确保在文件封装、解码、渲染等过程中的时间信息的一致性和准确性。

**常见场景**：

-   多流（如音频和视频）的同步播放。
    
-   不同格式和不同时间基的流转换。
    
-   确保音视频流在播放时同步。


## 代码解析：时间戳重缩放操作

#### 1. **`RescaleTime` 函数**：
``` cpp
//xformat.cpp
bool XFormat::RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* time_base)
{
    if (!pkt || !time_base)
    {
        return false;
    }

    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }

    auto out_stream = fmt_ctx_->streams[pkt->stream_index];

    // 调整 PTS 和 DTS
    pkt->pts = av_rescale_q_rnd(pkt->pts - offset_pts, *time_base, out_stream->time_base, 
                                (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    pkt->dts = av_rescale_q_rnd(pkt->dts - offset_pts, *time_base, out_stream->time_base, 
                                (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

    // 调整 duration
    pkt->duration = av_rescale_q(pkt->duration, *time_base, out_stream->time_base);

    pkt->pos = -1;

    return true;
}
```
-   **核心功能**：这个函数对 `AVPacket` 的时间戳（PTS、DTS）和持续时间（duration）进行重缩放，将它们从输入流的时间基（`time_base`）转换为输出流的时间基（`out_stream->time_base`）。
    
-   **音视频同步原理**：通过 `av_rescale_q_rnd()` 和 `av_rescale_q()` 函数，音视频流的时间戳被重新调整，确保每个流的时间戳都使用相同的时间基。由于音频和视频的时间基通常不同（例如，音频以采样为单位，而视频以帧为单位），这种时间戳重缩放将它们统一为相同的时间基。
    
    -   `pkt->pts`：表示当前帧的显示时间。
        
    -   `pkt->dts`：表示当前帧的解码时间。
        
    -   `pkt->duration`：表示当前帧的持续时间。
        
-   **为什么需要调整**：视频和音频流通常使用不同的时间基。例如，视频流的时间基可能基于帧（每帧的时间），而音频流基于采样率（每个采样的时间）。因此，我们需要将这些流的时间戳转换为统一的时间基，才能正确同步它们。
    
-   **`offset_pts`**：这个偏移量用于调整时间戳，这在流的转换或同步时很有用。例如，可能存在时间戳的偏移或延迟需要调整。
    

#### 2. **`RescaleToMs` 函数**：
``` cpp
long long XFormat::RescaleToMs(long long pts, int index)
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_ || index < 0 || index > fmt_ctx_->nb_streams)
    {
        return 0;
    }
    auto in_timebase = fmt_ctx_->streams[index]->time_base;
    AVRational out_timebase = { 1,1000 }; // 毫秒时间基
    return av_rescale_q(pts, in_timebase, out_timebase);
}
```
-   **核心功能**：这个函数将时间戳从输入流的时间基（`in_timebase`）转换为毫秒（`out_timebase = {1, 1000}`）。这在播放时非常有用，因为播放器通常以毫秒为单位进行同步和渲染。
    
-   **音视频同步**：在音视频同步的场景中，时间戳通常需要转换为相同的单位（如毫秒）。这个函数确保无论是视频流还是音频流，都能将其时间戳转换为相同的时间单位，从而方便后续的处理。
    

#### 3. **`set_time_out_ms` 函数**：
``` cpp
void XFormat::set_time_out_ms(int ms)
{
    unique_lock<mutex> lock(mtx_);
    time_out_ms_ = ms;

    // 设定超时处理回调函数
    if (fmt_ctx_)
    {
        AVIOInterruptCB cb = { TimeoutCallback, this };
        fmt_ctx_->interrupt_callback = cb;
    }
}
```

``` cpp
//int (*callback)(void*);
static int TimeoutCallback(void* para)
{
	auto xf = (XFormat*)para;
	if (xf->IsTimeout())
	{
		cout << "TimeoutCallback" << endl;
		return 1;
	}
	//正常阻塞
	return 0;
}
```
-   **核心功能**：该函数设置了流的超时回调，当流的读取超时或阻塞时，回调会触发。这对于流媒体播放中的实时性要求非常重要。
    
-   **音视频同步**：实时性对于音视频同步至关重要。如果超时回调触发，流的处理可能会被暂停或重新同步。比如在视频播放时，如果音频流发生延迟，超时回调可能会触发，重新调整同步。
    

### 音视频同步的实现原理

1.  **时间基统一**：
    
    -   音频流的时间基通常以采样为单位（例如，44100 Hz），而视频流的时间基通常以帧为单位。为了使音视频能够正确同步，我们需要将所有流的时间戳转换为统一的时间基。例如，将所有流的时间戳都转换为毫秒，或者转换为视频帧的时间基。
        
    -   这就是 `RescaleTime` 和 `RescaleToMs` 函数的作用，它们将不同时间基的时间戳转换为相同的时间单位，以便它们可以在同一时刻进行播放。
        
2.  **精确控制播放时序**：
    
    -   `RescaleTime` 对每个 `AVPacket` 进行时间戳重缩放，确保每个流的时间戳在播放时能够正确地同步。例如，在视频流和音频流之间，如果视频帧的时间戳需要与音频样本的时间戳对齐，时间戳就必须被统一为相同的时间基。
        
    -   `RescaleToMs` 将时间戳转换为毫秒，这使得播放器能够更加精确地控制每一帧的播放时机，避免出现音视频不同步的问题。
        
3.  **超时控制**：
    
    -   在流媒体应用中，音视频同步不仅需要准确的时间戳重缩放，还需要保证流的实时性。如果某一流（如视频流或音频流）由于某些原因（例如网络延迟）没有及时到达，可能会导致播放不同步。通过设置超时回调（如 `set_time_out_ms`），当超时发生时可以调整流的处理方式（例如丢弃延迟的帧或调整播放节奏），从而避免音视频不同步的问题。
        

### 总结

音视频同步的关键在于确保不同流的时间戳在播放时能够在同一时间轴上正确对齐。你提供的代码通过以下几个步骤实现这一目标：

1.  **时间戳重缩放**：通过 `RescaleTime` 和 `RescaleToMs` 函数将不同流（如视频流和音频流）的时间戳转换为统一的时间基（如毫秒），确保它们能在同一时刻同步播放。
    
2.  **超时控制**：通过 `set_time_out_ms` 函数设置超时回调，确保在网络或其他原因导致延迟时能进行相应的调整，保证音视频流的实时性。
    
3.  **统一时间基**：将视频和音频流的时间基统一为一个标准（如毫秒），并使用 `av_rescale_q` 和 `av_rescale_q_rnd` 等函数进行精确的时间戳重缩放，使得流能够正确对齐，避免音视频不同步。
    

通过这些操作，能够实现音视频的正确同步，确保播放器可以流畅地显示和播放音视频内容。

## demux解封装
1. **打开媒体文件（`Open` 函数）**
``` cpp
AVFormatContext* XDemux::Open(const char* url)
{
    AVFormatContext* fmt_ctx = nullptr;

    AVDictionary* opts = nullptr;
    // 设置超时为 3 秒
    av_dict_set(&opts, "stimeout", "3000000", 0);

    // 先分配context
    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx)
    {
        return nullptr;
    }

    // 打开媒体文件
    int re = avformat_open_input(&fmt_ctx, url, nullptr, &opts);
    av_dict_free(&opts);
    CERR(re);

    // 获取流信息
    re = avformat_find_stream_info(fmt_ctx, nullptr);
    CERR(re);

    // 打印封装格式信息
    av_dump_format(fmt_ctx, 0, url, 0);
    return fmt_ctx;
}
```
-   **`AVFormatContext`**：是 FFmpeg 中用于处理媒体文件的结构体。它存储了关于媒体文件的所有信息，包括流的数量、每个流的编解码器信息、时间基、格式等。
    
-   **`avformat_alloc_context()`**：分配一个 `AVFormatContext`，为媒体文件的解封装准备环境。
    
-   **`av_dict_set()`**：设置一些选项，如超时（`stimeout`）。这里设置的是超时为 3 秒，单位是微秒（3,000,000 微秒，即 3 秒）。这会在后续的网络操作中起到作用，控制超时的响应。
    
-   **`avformat_open_input()`**：通过指定的 URL 打开输入媒体文件（可以是本地文件、RTSP 流等）。如果是 RTSP 流，默认使用 UDP 协议，注释中的代码可以强制改为 TCP 协议。
    
-   **`avformat_find_stream_info()`**：在媒体文件成功打开后，`avformat_find_stream_info()` 函数用来获取媒体文件的流信息（例如：视频流、音频流等）。这一步会分析整个媒体文件，找出其中包含的视频、音频等流，并为每个流分配编解码器信息。
    
-   **`av_dump_format()`**：打印媒体文件的详细信息，帮助调试和了解文件的封装格式。
2. **读取数据包（`Read` 函数）**
``` cpp
bool XDemux::Read(AVPacket* pkt)
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }
    auto re = av_read_frame(fmt_ctx_, pkt);
    CERR(re);
    // 记时用于超时判断
    last_time_ = NowMs();
    return true;
}
```
-   **`AVPacket`**：是 FFmpeg 中的结构体，表示音视频数据包（包含解码前的原始数据）。每个 `AVPacket` 通常包含了一个流的数据。
    
-   **`av_read_frame()`**：该函数用于读取媒体文件中的下一帧数据（包括音频、视频、字幕等）。它从文件或流中提取数据包，并将数据填充到 `AVPacket` 中。返回值是读取的状态，成功时返回 0，失败时返回负数。
    
-   **`unique_lock<mutex>`**：该代码使用互斥锁（`mutex`）来保护对共享资源的访问，确保在多线程环境下对 `fmt_ctx_` 的访问是安全的。
    
-   **`last_time_ = NowMs();`**：记录当前时间（毫秒），用于后续的超时判断。这样做通常是为了在读取数据包时，如果某个操作超时，可以进行处理。
### 3. **流的定位（`Seek` 函数）**
``` cpp
bool XDemux::Seek(long long pts, int stream_index)
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }
    int re = av_seek_frame(fmt_ctx_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    if (re < 0)
    {
        return false;
    }
    return true;
}
```
-   **`av_seek_frame()`**：该函数用于在流中定位到特定的时间戳（`pts`）。`stream_index` 表示要操作的流的索引，`pts` 是需要跳转到的时间戳。
    
    -   **`AVSEEK_FLAG_FRAME`**：表示按帧定位。
        
    -   **`AVSEEK_FLAG_BACKWARD`**：表示回退到目标帧（寻找前向的帧）。这通常用于定位到流的具体时间点或帧。
        
-   **`pts`**：通常表示时间戳（presentation timestamp），决定帧何时显示。`av_seek_frame()` 会根据提供的时间戳和流信息进行定位。
    
-   **`unique_lock<mutex>`**：使用锁保护操作，确保多线程环境下对共享资源 `fmt_ctx_` 的安全访问。
### 流程总结：

1.  **打开媒体文件**：
    
    -   使用 `avformat_open_input()` 打开指定的 URL（可以是本地文件或网络流）。
        
    -   获取并解析媒体文件中的流信息（`avformat_find_stream_info()`）。
        
    -   打印文件的详细格式信息（`av_dump_format()`）。
        
2.  **读取数据包**：
    
    -   使用 `av_read_frame()` 从媒体文件中读取数据包（音频、视频或其他流的数据）。
        
    -   每次读取数据包时，记录当前时间，以便后续的超时检查。
        
3.  **流定位（Seek）**：
    
    -   通过 `av_seek_frame()` 在媒体文件中根据时间戳进行定位。可以根据时间戳回退到目标帧的位置。
        
4.  **超时处理**：
    
    -   在流读取过程中，如果发生超时，通过 `TimeoutCallback` 回调函数进行处理，确保不会发生阻塞或卡死。

## decode 解码
1. **`Send` 函数**
``` cpp
bool XDecode::Send(AVPacket* pkt)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)
    {
        return false;
    }
    int ret = avcodec_send_packet(c_, pkt);
    if (ret != 0)
    {
        return false;
    }
    return true;
}
```
-   **功能**：将数据包（`AVPacket`）发送到解码器（`c_`）。在解码过程中，数据包需要先被发送到解码器，然后解码器会从中提取帧进行解码。这个函数封装了 `avcodec_send_packet()` 函数，用于发送解码数据包。
    
-   **关键步骤**：
    
    -   使用 `unique_lock<mutex>` 锁定解码器，以保证多线程安全。
        
    -   检查解码器是否有效，如果 `c_` 为 `nullptr`，则返回 `false`。
        
    -   调用 FFmpeg 的 `avcodec_send_packet()` 函数将 `pkt` 数据包发送给解码器。`avcodec_send_packet` 会将数据包发送到解码器的内部队列，解码器随后可以从队列中提取数据进行解码。
        
    -   如果发送成功，返回 `true`，否则返回 `false`。
2. **`Recv` 函数**
``` cpp
bool XDecode::Recv(AVFrame* frame, bool is_hw_copy)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)
    {
        return false;
    }
    int ret = avcodec_receive_frame(c_, frame);

    if (ret == 0)
    {
        if (is_hw_copy && c_->hw_device_ctx)
        {
            AVFrame* sw = av_frame_alloc();
            if (!sw) return false;

            ret = av_hwframe_transfer_data(sw, frame, 0);
            if (ret < 0)
            {
                av_frame_free(&sw);
                return false;
            }

            // 用 sw 替换调用者传入 frame 的内容
            av_frame_unref(frame);
            av_frame_move_ref(frame, sw);

            av_frame_free(&sw);
        }
        return true;
    }
    else
    {
        return false;
    }
    return false;
}
```
-   **功能**：从解码器接收解码后的帧（`AVFrame`），并进行必要的硬件加速复制。如果解码器解码成功，它将数据存储到 `frame` 中。
    
-   **关键步骤**：
    
    -   使用 `unique_lock<mutex>` 锁定解码器，保证多线程安全。
        
    -   检查解码器是否有效，如果 `c_` 为 `nullptr`，则返回 `false`。
        
    -   调用 `avcodec_receive_frame()` 获取解码后的帧。如果成功，返回 `0`，并将解码后的帧存入 `frame`。
        
    -   如果需要硬件加速复制（`is_hw_copy` 为 `true`），并且解码器已配置硬件设备上下文（`c_->hw_device_ctx`），则进行硬件加速数据的转换：
        
        -   分配一个新的 `AVFrame`（`sw`），这是一个用于存储软件解码数据的帧。
            
        -   使用 `av_hwframe_transfer_data()` 将硬件解码的帧数据复制到 `sw`。
            
        -   将硬件解码的帧替换为软件解码的帧，并释放 `sw` 帧。
3. **`End` 函数**
``` cpp
std::vector<AVFrame*> XDecode::End()
{
    std::vector<AVFrame*> res;
    unique_lock<mutex> lock(mux_);
    if (!c_) return res;

    // 1. 发送 NULL 告诉解码器没有更多数据
    int re = avcodec_send_packet(c_, NULL);
    if (re < 0) return res;

    // 2. 循环接收所有剩余帧
    while (true)
    {
        AVFrame* frame = av_frame_alloc();
        re = avcodec_receive_frame(c_, frame);

        if (re == 0)
        {
            // 成功取到一帧，保存
            res.push_back(frame);
        }
        else if (re == AVERROR_EOF)
        {
            // 解码器真正结束
            av_frame_free(&frame);
            break;
        }
        else if (re == AVERROR(EAGAIN))
        {
            // flush 时一般不会出现，但保险起见释放 frame
            av_frame_free(&frame);
            break;
        }
        else
        {
            // 其他错误
            av_frame_free(&frame);
            break;
        }
    }

    return res;
}
```
-   **功能**：当解码过程完成时，调用此函数以清理解码器并接收所有剩余的解码帧。此函数会发送一个 `NULL` 数据包告知解码器没有更多数据，并继续接收解码后的帧直到解码器没有更多数据为止。
    
-   **关键步骤**：
    
    -   **发送 `NULL` 数据包**：调用 `avcodec_send_packet(c_, NULL)` 向解码器发送一个 `NULL` 包，告诉解码器接下来没有更多的数据包。这通常用于解码器的“刷新”阶段，即解码器会继续解码并输出所有剩余的帧。
        
    -   **接收剩余帧**：在解码器收到 `NULL` 数据包后，它会返回所有已解码的帧。使用 `avcodec_receive_frame()` 来接收剩余的帧。成功接收时，将帧存入 `res` 向量。
        
    -   **错误处理**：
        
        -   **`AVERROR_EOF`**：表示解码器已完成解码，所有数据已处理完。此时释放帧并退出。
            
        -   **`AVERROR(EAGAIN)`**：表示需要更多的数据来解码，通常在流处理中会遇到这种情况。这里的 `flush` 处理其实是清理流，退出循环。
            
        -   其他错误情况也会释放帧并退出。
            
-   **返回值**：函数返回一个包含所有接收帧的 `std::vector<AVFrame*>`，这些帧可以进一步处理或显示。
    

### 总结

-   **`Send`**：将数据包发送到解码器，等待解码器解码。
    
-   **`Recv`**：从解码器接收解码后的帧。如果启用了硬件加速复制（`is_hw_copy`），它会将硬件解码帧复制到软件解码帧。
    
-   **`End`**：告知解码器没有更多数据（发送 `NULL` 数据包），并接收所有剩余的帧。
    

这些函数共同协作，实现了一个典型的视频解码流程，包括数据的发送与接收、硬件加速的支持、以及解码结束后的处理。`End` 函数确保解码器在没有更多数据时能够正确结束并返回所有剩余的帧。

## encode 编码
1. **`Encode` 函数**
``` cpp
AVPacket* XEncode::Encode(const AVFrame* frame)
{
    unique_lock<mutex> lock(mux_);
    if (!c_ || !frame)
    {
        return nullptr;
    }
    auto pkt = av_packet_alloc();

    // 将原始数据发送给线程进行压缩
    int re = avcodec_send_frame(c_, frame);
    if (re != 0)
    {
        return nullptr;
    }

    // 调用之后会立刻返回，但不代表已经压缩完成，需要重复询问
    // 每次调用会重新生成pkt的空间，所以调用完之后需要释放
    re = avcodec_receive_packet(c_, pkt);
    if (re == 0)
    {
        return pkt;
    }

    av_packet_free(&pkt);
    if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
    {
        return nullptr;
    }
    if (re < 0)
    {
        PrintErr(re);
    }
    return nullptr;
}
```
#### 关键功能：编码视频帧

-   **`unique_lock<mutex>`**：保证多线程环境下对 `c_`（解码器上下文）的安全访问。`mux_` 是一个互斥锁，防止多个线程同时访问 `c_`，从而确保线程安全。
    
-   **`if (!c_ || !frame)`**：如果 `c_`（编码上下文）或输入帧 `frame` 为空，直接返回 `nullptr`。
    
-   **`av_packet_alloc()`**：分配一个新的 `AVPacket` 对象。`AVPacket` 存储编码后的数据，最终会输出给调用者。
    
-   **`avcodec_send_frame(c_, frame)`**：将帧（`frame`）发送给编码器。该函数将原始数据帧（例如未压缩的图像帧）传递给编码器进行压缩。注意，这个调用是 **非阻塞的**，它会将帧放入编码器内部的队列，并立即返回。
    
    -   如果返回值不是 `0`，说明发送失败，函数直接返回 `nullptr`。
        
-   **`avcodec_receive_packet(c_, pkt)`**：尝试从编码器接收已压缩的编码数据（即 `AVPacket`）。该函数会阻塞，直到编码器处理完之前发送的帧并将编码后的数据填充到 `pkt` 中。
    
    -   如果编码成功（返回值为 `0`），返回编码后的数据包 `pkt`。
        
    -   如果返回 `AVERROR(EAGAIN)`，表示编码器还没有准备好输出数据，需要继续调用 `avcodec_receive_packet()`。
        
    -   如果返回 `AVERROR_EOF`，表示没有更多数据可供编码，编码器已完成。
        
    -   其他错误返回负值，通过 `PrintErr(re)` 打印错误信息。
        
-   **资源释放**：如果编码过程没有成功生成数据包，调用 `av_packet_free(&pkt)` 释放分配的内存。
    

#### 总结：

`Encode` 函数的作用是将输入的原始帧（如未压缩的视频帧）发送给编码器进行编码，并尝试从编码器中接收生成的压缩后的数据包。如果编码成功，返回一个包含编码数据的 `AVPacket`；否则返回 `nullptr`。

2. **`End` 函数**
``` cpp
std::vector<AVPacket*> XEncode::End()
{
    std::vector<AVPacket*> res;
    unique_lock<mutex> lock(mux_);
    if (!c_)
    {
        return res;
    }

    // 发送NULL，获取缓冲
    int re = avcodec_send_frame(c_, NULL);
    if (re != 0)
    {
        return res;
    }

    while (re >= 0)
    {
        auto pkt = av_packet_alloc();
        // 调用之后会立刻返回，但不代表已经压缩完成，需要重复询问
        // 每次调用会重新生成pkt的空间，所以调用完之后需要释放
        re = avcodec_receive_packet(c_, pkt);

        if (re != 0)
        {
            av_packet_free(&pkt);
            break;
        }
        res.push_back(pkt);
    }
    return res;
}
```
#### 关键功能：结束编码并接收所有剩余的编码数据

-   **`avcodec_send_frame(c_, NULL)`**：通过向编码器发送一个 `NULL` 帧，告知编码器没有更多的帧数据要编码。这是一个“刷新”操作，目的是让编码器处理并返回所有剩余的编码数据包。发送 `NULL` 会强制编码器输出缓存中所有未输出的数据。
    
    -   如果返回值不是 `0`，表示发送失败，返回一个空的 `std::vector<AVPacket*>`。
        
-   **循环接收编码后的数据包**：
    
    -   `while (re >= 0)`：持续调用 `avcodec_receive_packet()` 来接收编码器输出的所有数据包，直到编码器返回错误或者没有更多数据。
        
    -   每次从编码器接收到一个压缩后的数据包（`AVPacket`），并将其存入 `res` 向量中。
        
-   **`av_packet_free(&pkt)`**：如果接收数据包失败，释放数据包 `pkt` 以避免内存泄漏。
    
-   **返回值**：函数最终返回一个包含所有编码后的 `AVPacket` 的 `std::vector<AVPacket*>`，这些数据包可以进一步用于存储或传输。
    

#### 总结：

`End` 函数的作用是结束编码过程并接收所有剩余的编码数据包。通过向编码器发送一个 `NULL` 帧，告知编码器没有更多的数据需要编码。然后，循环接收编码器输出的所有数据包，并将它们存入 `std::vector<AVPacket*>` 向量中返回。

### 总结整个编码流程

1.  **编码单帧**：
    
    -   在 `Encode` 函数中，传入的每一帧（`AVFrame`）都会被送入编码器进行编码。编码器生成的压缩数据包（`AVPacket`）被返回给调用者。
        
2.  **结束编码**：
    
    -   `End` 函数用于结束编码过程，发送一个 `NULL` 数据包给编码器，强制编码器输出所有剩余的编码数据包。接收到的所有数据包被存储在一个 `std::vector<AVPacket*>` 中并返回。
        
3.  **错误处理**：
    
    -   在编码过程中，如果出现错误（如返回负值），会通过 `PrintErr(re)` 打印错误信息。
        
4.  **内存管理**：
    
    -   在每次从编码器接收数据包后，都会释放之前分配的 `AVPacket` 内存，确保没有内存泄漏。
        

通过这两个函数，`XEncode` 类实现了视频帧的编码过程和编码结束后的数据提取。

## mux 封装
1. **`Open` 函数**
``` cpp
AVFormatContext* XMux::Open(const char* url,
    AVCodecParameters* video_para,
    AVCodecParameters* audio_para)
{
    AVFormatContext* ec = nullptr;
    auto re = avformat_alloc_output_context2(&ec, NULL, NULL, url);
    CERR(re);
    
    //音视频流创建顺序必须要跟在format里面设置的一样，否则会报错，无法运行
    if (video_para)
    {
        AVStream* out_video_stream = avformat_new_stream(ec, nullptr);
        avcodec_parameters_copy(out_video_stream->codecpar, video_para);
    }

    if (audio_para)
    {
        AVStream* out_audio_stream = avformat_new_stream(ec, nullptr);
        avcodec_parameters_copy(out_audio_stream->codecpar, audio_para);
    }

    // 打开输出io
    re = avio_open(&ec->pb, url, AVIO_FLAG_WRITE);
    CERR(re);

    // 打印封装信息
    av_dump_format(ec, 0, url, 1);

    return ec;
}
```
#### 功能：

-   这个函数的目的是 **初始化输出封装格式**（例如 MP4、MKV 等），并为音频和视频流创建相应的流（`AVStream`）。
    

#### 详细步骤：

-   **`avformat_alloc_output_context2`**：创建一个新的 `AVFormatContext`，这是 FFmpeg 中封装格式的上下文。`url` 是输出文件的路径，可以是一个文件路径或网络协议地址。
    
-   **创建音视频流**：
    
    -   如果提供了 `video_para`，则创建一个视频流，并复制 `video_para` 中的编码参数到流中。
        
    -   如果提供了 `audio_para`，则创建一个音频流，并复制 `audio_para` 中的编码参数到流中。
        
-   **打开输出 I/O**：
    
    -   使用 `avio_open` 打开输出文件（或网络协议），以写入模式打开。
        
-   **打印封装信息**：调用 `av_dump_format` 打印封装格式信息，帮助调试输出流的配置。
    

#### 总结：

`Open` 函数初始化了一个输出上下文，并为音频和视频流创建了对应的流，同时打开了输出文件的写入操作。

### 2. **`WriteHeader` 函数**
``` cpp
bool XMux::WriteHeader()
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }
    auto re = avformat_write_header(fmt_ctx_, NULL);
    CERR(re);
    
    // 打印封装信息
    av_dump_format(fmt_ctx_, 0, fmt_ctx_->url, 1);

    begin_video_pts = -1;
    begin_audio_pts = -1;

    return true;
}
```
#### 功能：

-   该函数用于 **写入输出封装文件的头部信息**。封装文件的头部通常包含媒体文件的基本信息，比如音视频流的格式、时间基、编解码信息等。
    

#### 详细步骤：

-   **`avformat_write_header`**：写入封装头部。它需要一个有效的 `AVFormatContext`（即 `fmt_ctx_`）。该函数会进行一些必要的设置，如处理流和输出文件的元数据。
    
-   **`av_dump_format`**：打印封装格式的详细信息，帮助调试。
    
-   **时间戳初始化**：`begin_video_pts` 和 `begin_audio_pts` 用于记录视频和音频流的起始时间戳，初始化为 `-1`，意味着它们还未开始。并且适应需要重复输出视频的场景。
    

#### 总结：

`WriteHeader` 写入封装文件的头部信息，确保文件正确初始化，之后可以开始写入音视频数据。

3. **`Write` 函数**
``` cpp
bool XMux::Write(AVPacket* pkt)
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }

    // 没读取到pts, 重新设置
    if (pkt->pts == AV_NOPTS_VALUE)
    {
        pkt->pts = 0;
        pkt->dts = 0;
    }

    if (pkt->stream_index == video_index_)
    {
        pkt->stream_index = 0;
        if (begin_video_pts < 0)
        {
            begin_video_pts = pkt->pts;
        }
        lock.unlock();
        RescaleTime(pkt, begin_video_pts, src_video_time_base_);
        lock.lock();
    }

    if (pkt->stream_index == audio_index_)
    {
        pkt->stream_index = 1;
        if (begin_audio_pts < 0)
        {
            begin_audio_pts = pkt->pts;
        }
        lock.unlock();
        RescaleTime(pkt, begin_audio_pts, src_audio_time_base_);
        lock.lock();
    }

    cout << pkt->pts << flush;
    auto re = av_interleaved_write_frame(fmt_ctx_, pkt);
    CERR(re);
    return true;
}
```
#### 功能：

-   该函数用于将编码后的音视频数据包写入封装文件中。
    

#### 详细步骤：

-   **时间戳设置**：如果 `pkt->pts`（时间戳）为 `AV_NOPTS_VALUE`，即没有时间戳，则重置为 `0`，避免时间戳缺失导致的错误。
    
-   **视频流和音频流的时间戳处理**：
    
    -   如果数据包属于视频流（`pkt->stream_index == video_index_`），则更新视频流的时间戳，调用 `RescaleTime` 函数将时间戳转换为适当的时间基。
        
    -   如果数据包属于音频流（`pkt->stream_index == audio_index_`），则更新音频流的时间戳，调用 `RescaleTime` 函数将时间戳转换为适当的时间基。
        
-   **`av_interleaved_write_frame`**：将音视频数据包写入封装文件，`pkt` 包含视频或音频数据。`av_interleaved_write_frame` 会根据流的顺序正确地将音视频包交替写入。
    

#### 总结：

`Write` 函数将编码后的音视频数据包写入输出文件，并对时间戳进行调整，以确保音视频流正确同步。

### 4. **`WriteTrailer` 函数**
``` cpp
bool XMux::WriteTrailer()
{
    unique_lock<mutex> lock(mtx_);
    if (!fmt_ctx_)
    {
        return false;
    }
    av_interleaved_write_frame(fmt_ctx_, nullptr); // 写入排序缓冲
    auto re = av_write_trailer(fmt_ctx_);
    CERR(re);
    return true;
}
```
#### 功能：

-   该函数用于 **写入封装文件的尾部信息**，并清理文件中剩余的音视频数据。
    

#### 详细步骤：

-   **`av_interleaved_write_frame`**：写入排序缓冲，确保文件中的音视频数据顺序正确。
    
-   **`av_write_trailer`**：写入封装文件的尾部信息，完成文件的封装。
    

#### 总结：

`WriteTrailer` 在写入文件尾部信息后完成整个多路复用过程，保证封装文件的完整性。

### 5. **`set_src_video_time_base` 和 `set_src_audio_time_base` 函数**
``` cpp
void XMux::set_src_video_time_base(AVRational* tb)
{
    if (!tb)
    {
        return;
    }
    unique_lock<mutex> lock(mtx_);
    if (!src_video_time_base_)
    {
        src_video_time_base_ = new AVRational();
    }
    *src_video_time_base_ = *tb;
}

void XMux::set_src_audio_time_base(AVRational* tb)
{
    if (!tb)
    {
        return;
    }
    unique_lock<mutex> lock(mtx_);
    if (!src_audio_time_base_)
    {
        src_audio_time_base_ = new AVRational();
    }
    *src_audio_time_base_ = *tb;
}
```
#### 功能：

-   这两个函数用于 **设置视频和音频流的时间基**，确保多路复用时音视频流的时间戳能够正确转换。
    

### 6. **析构函数**
``` cpp
XMux::~XMux()
{
    unique_lock<mutex> lock(mtx_);
    if (src_video_time_base_)
    {
        delete src_video_time_base_;
        src_video_time_base_ = nullptr;
    }
    if (src_audio_time_base_)
    {
        delete src_audio_time_base_;
        src_audio_time_base_ = nullptr;
    }
}
```
#### 功能：

-   **清理资源**：析构函数释放 `src_video_time_base_` 和 `src_audio_time_base_` 指针占用的内存。
    

----------

### 总结：

该 `XMux` 类封装了 FFmpeg 多路复用的核心操作，包括：

-   初始化封装格式和流。
    
-   写入文件头部和尾部。
    
-   将编码后的音视频包写入输出文件。
    
-   处理音视频时间戳，确保同步。
    
-   提供了设置时间基和资源清理功能。
    

这个类用于实现音视频的合并输出，并且支持流的顺序、时间戳的调整、以及封装格式的处理。

## 渲染
1. **`Creat` 函数**
``` cpp
XVideoView* XVideoView::Creat(RenderType type = SDL)
{
    switch (type)
    {
    case XVideoView::SDL:
        return new XSDL();
        break;
    default:
        break;
    }
    return nullptr;
}
```
#### 功能：

-   **工厂方法**，根据渲染类型 (`RenderType`)，创建并返回一个适当的渲染器对象（如 `XSDL` 类）。
    
-   目前支持的渲染类型只有 `SDL`，即使用 `SDL` 作为渲染库。
    

### 2. **`Init` 函数**
``` cpp
bool XVideoView::Init(AVCodecParameters* para)
{
    if (!para)
    {
        return false;
    }
    auto fmt = (Format)para->format;
    switch (para->format)
    {
    case AV_PIX_FMT_YUV420P:
        fmt = YUV420P;
        break;
    default:
        break;
    }
    return Init(para->width, para->height, fmt);
}
```
#### 功能：

-   初始化视频显示，接收 `AVCodecParameters` 作为输入，提取视频的格式、宽度和高度等信息。
    
-   根据 `AVCodecParameters` 中的 `format` 字段设置相应的像素格式。在这里，它处理了 `AV_PIX_FMT_YUV420P` 格式，其他格式可能需要进一步处理。
    
-   最终调用 `Init` 函数进行更详细的初始化。
    

### 3. **`DrawFrame` 函数**
``` cpp
bool XVideoView::DrawFrame(AVFrame* frame)
{
    if (!frame || !frame->data[0])
    {
        return false;
    }
    count_++;
    if (beg_ms_ <= 0)
    {
        beg_ms_ = clock();
    }
    else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000)
    {
        render_fps_ = count_;
        count_ = 0;
        beg_ms_ = clock();
    }

    switch (frame->format)
    {
    case AV_PIX_FMT_YUV420P:
        return Draw(
            frame->data[0], frame->linesize[0],  // Y
            frame->data[1], frame->linesize[1],  // U
            frame->data[2], frame->linesize[2]   // V
            );
        break;
    case AV_PIX_FMT_BGRA:
    case AV_PIX_FMT_ARGB:
    case AV_PIX_FMT_RGBA:
        return Draw(frame->data[0], frame->linesize[0]);
        break;
    case AV_PIX_FMT_RGB24:
        return Draw(frame->data[0], 0);
        break;
    case AV_PIX_FMT_NV12:
        return Draw(frame->data[0], frame->linesize[0],
                    frame->data[1], frame->linesize[1]);
        break;
    default:
        break;
    }
    return false;
}
```
#### 功能：

-   **处理和绘制视频帧**：该函数每次接收一个 `AVFrame`，根据其像素格式决定如何渲染帧。
    
-   **FPS统计**：通过 `clock()` 函数计算每秒渲染的帧数 (`render_fps_`)，以便统计渲染性能。
    
    -   `beg_ms_` 记录帧开始的时间，如果经过一秒钟（通过 `clock()` 计算），则更新 `render_fps_`。
        
    -   每次绘制后，`count_` 自增，表示已绘制的帧数。
        
-   **像素格式转换**：根据不同的像素格式（如 `YUV420P`、`BGRA`、`RGB24` 等）选择合适的绘制函数（`Draw`），每种格式的处理方式略有不同：
    
    -   `AV_PIX_FMT_YUV420P`：YUV420P 格式，分别传入 Y、U、V 数据和对应的行大小（`linesize`）。
        
    -   `AV_PIX_FMT_BGRA`、`AV_PIX_FMT_ARGB`、`AV_PIX_FMT_RGBA`：常见的 BGRA、ARGB、RGBA 格式，仅传入一个数据平面。
        
    -   `AV_PIX_FMT_RGB24`：RGB24 格式，只处理一个数据平面。
        
    -   `AV_PIX_FMT_NV12`：NV12 格式，Y 和 UV 数据平面分别传入。
        

### 4. **`Open` 函数**
``` cpp
bool XVideoView::Open(std::string filepath)
{
    if (ifs_.is_open())
    {
        ifs_.close();
    }
    ifs_.open(filepath, ios::binary);

    ifs_.seekg(0, ios::end);    // 移到文件结尾
    file_size_ = ifs_.tellg();  // 文件指针的位置
    ifs_.seekg(0, ios::beg);    // 移到文件开头

    return ifs_.is_open();
}
```
#### 功能：

-   打开一个视频文件，读取文件内容并初始化相关数据。
    
-   使用 `ifstream` 打开文件，并获取文件的大小（`file_size_`），以便后续的读取操作。
    

### 5. **`Read` 函数**
``` cpp
AVFrame* XVideoView::Read()
{
    if (width_ <= 0 || height_ <= 0 || !ifs_)
    {
        return nullptr;
    }

    // 如果已经申请，但是参数发生变化的话，释放并重新分配
    if (frame_)
    {
        if (frame_->width != width_
            || frame_->height != height_
            || frame_->format != fmt_
            )
        {
            av_frame_free(&frame_);
        }
    }

    // 如果 frame_ 不存在或者已经被释放
    if (!frame_)
    {
        frame_ = av_frame_alloc();
        // 图像参数
        frame_->width = width_;
        frame_->height = height_;
        frame_->format = fmt_;

        frame_->linesize[0] = width_ * 4;

        if (frame_->format == AV_PIX_FMT_YUV420P)
        {
            frame_->linesize[0] = width_;
            frame_->linesize[1] = width_ / 2;
            frame_->linesize[2] = width_ / 2;
        }
        else if (frame_->format == AV_PIX_FMT_RGB24)
        {
            frame_->linesize[0] = width_ * 3;
        }

        // 生成 AVFrame 空间
        int re = av_frame_get_buffer(frame_, 0);
        if (re != 0)
        {
            char buf[1024] = { 0 };
            av_strerror(re, buf, sizeof(buf));
            cout << buf << endl;
            av_frame_free(&frame_);
            return nullptr;
        }
    }

    if (!frame_)
    {
        return nullptr;
    }

    if (ifs_.tellg() == file_size_)
    {
        ifs_.seekg(0, ios::beg);
    }

    if (frame_->format == AV_PIX_FMT_YUV420P)
    {
        ifs_.read((char*)frame_->data[0], width_ * height_);
        ifs_.read((char*)frame_->data[1], width_ * height_ / 4);
        ifs_.read((char*)frame_->data[2], width_ * height_ / 4);
    }
    else if (frame_->format == AV_PIX_FMT_BGRA
        || frame_->format == AV_PIX_FMT_ARGB
        || frame_->format == AV_PIX_FMT_RGBA)
    {
        ifs_.read((char*)frame_->data[0], width_ * height_ * 4);
    }
    else if (frame_->format == AV_PIX_FMT_RGB24)
    {
        ifs_.read((char*)frame_->data[0], width_ * height_ * 3);
    }

    if (ifs_.gcount() == 0)
    {
        return nullptr;
    }
    return frame_;
}
```
#### 功能：

-   该函数从文件中读取视频帧数据并将其存储在 `AVFrame` 中。
    
-   如果视频帧的尺寸或格式发生变化，则释放之前的帧并重新分配内存。
    
-   根据像素格式（如 YUV420P、BGRA 等），从文件中按正确的格式读取数据并填充到 `AVFrame` 对象中。
    
-   在文件末尾时，重新定位文件指针到文件开头，循环读取数据。
    


#### 详细步骤：

-   **检查和分配 `AVFrame`**：如果 `frame_` 存在并且参数发生变化，则释放原有的 `AVFrame` 并重新分配内存。
    
-   **读取数据**：根据像素格式从文件中读取对应的数据到 `AVFrame` 的数据平面中。
    
-   **返回帧**：返回读取到的帧，如果没有更多数据可读则返回 `nullptr`。
    

----------

### 总结：

`XVideoView` 类用于从文件中读取视频帧数据，并将其绘制到屏幕上。它支持多种像素格式（如 YUV420P、RGB24、BGRA 等）并可以通过不同的渲染方式（如 SDL）进行显示。主要功能包括帧的初始化、读取、渲染以及 FPS 统计等。

## SDL
1. **`InitVideo` 函数**
```cpp
static bool InitVideo()
{
    static bool is_first = true;
    static mutex mux;
    unique_lock<mutex> sdl_lock(mux);
    if (!is_first)
    {
        return true;
    }
    is_first = false;
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        cout << SDL_GetError() << endl;
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    return true;
}
```
#### 功能：

-   该函数确保 SDL 库只初始化一次，避免多次初始化导致的错误。
    
-   使用 `SDL_Init(SDL_INIT_VIDEO)` 初始化 SDL 的视频子系统。
    
-   `SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")` 设置 SDL 渲染的缩放质量为高（抗锯齿）。
    

### 2. **`XSDL::Init` 函数**
``` cpp
bool XSDL::Init(int w, int h, Format fmt)
{
    if (w <= 0 || h <= 0)
    {
        return false;
    }
    InitVideo();  // 初始化 SDL 视频子系统
    unique_lock<mutex> sdl_locl(mtx_);
    width_ = w;
    height_ = h;
    fmt_ = fmt;

    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }

    // 创建窗口
    if (!win_)
    {
        if (!win_id_)
        {
            win_ = SDL_CreateWindow(
                "",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w,
                h,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
            );
        }
        else
        {
            win_ = SDL_CreateWindowFrom(win_id_);
        }
    }
    if(!win_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    // 创建渲染器
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    if (!render_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    // 创建材质（纹理），选择适当的像素格式
    unsigned int sdl_fmt = SDL_PIXELFORMAT_IYUV;  // 默认YUV格式
    switch (fmt)
    {
        case XVideoView::RGBA:
            sdl_fmt = SDL_PIXELFORMAT_RGBA32;
            break;
        case XVideoView::BGRA:
            sdl_fmt = SDL_PIXELFORMAT_BGRA32;
            break;
        case XVideoView::ARGB:
            sdl_fmt = SDL_PIXELFORMAT_ARGB32;
            break;
        case XVideoView::YUV420P:
            sdl_fmt = SDL_PIXELFORMAT_IYUV;
            break;
        case XVideoView::RGB24:
            sdl_fmt = SDL_PIXELFORMAT_RGB24;
            break;
        case XVideoView::NV12:
            sdl_fmt = SDL_PIXELFORMAT_NV12;
            break;
        default:
            break;
    }

    // 创建纹理，纹理存储在 GPU 上
    texture_ = SDL_CreateTexture(render_,
                                 sdl_fmt,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 w,
                                 h);
    if (!texture_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    return true;
}
```
#### 功能：

-   该函数用于初始化 SDL 渲染系统，包括：
    
    -   初始化 SDL 库。
        
    -   创建窗口、渲染器、纹理。
        
    -   根据给定的像素格式设置合适的 SDL 格式。
        
    -   为纹理创建合适的 GPU 存储，并为视频帧数据分配渲染空间。
        

#### 关键步骤：

1.  **初始化 SDL**：调用 `InitVideo` 初始化 SDL 视频子系统。
    
2.  **创建窗口**：使用 `SDL_CreateWindow` 创建一个窗口。窗口大小由 `w` 和 `h` 确定。
    
3.  **创建渲染器**：通过 `SDL_CreateRenderer` 创建一个加速渲染器。
    
4.  **创建纹理**：使用 `SDL_CreateTexture` 创建一个纹理，纹理格式根据输入的 `Format` 类型确定，支持多种格式（如 `YUV420P`、`RGBA` 等）。
    

### 3. **`XSDL::Close` 函数**
``` cpp
void XSDL::Close()
{
    unique_lock<mutex> sdl_locl(mtx_);
    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }
    if (win_)
    {
        SDL_DestroyWindow(win_);
        win_ = nullptr;
    }
}
```
#### 功能：

-   释放 SDL 创建的资源，包括纹理、渲染器和窗口，防止内存泄漏。
    

### 4. **`XSDL::IsExit` 函数**
``` cpp
bool XSDL::IsExit()
{
    SDL_Event ev;
    SDL_WaitEventTimeout(&ev, 1);
    if (ev.type == SDL_QUIT)
    {
        return true;
    }
    return false;
}
```
#### 功能：

-   等待并处理 SDL 事件，检查是否有退出事件（如用户关闭窗口）。
    
-   如果 `SDL_QUIT` 事件发生，返回 `true`，否则返回 `false`。
    

### 5. **`XSDL::Draw` 函数**
``` cpp
bool XSDL::Draw(const unsigned char* data, int linesize)
{
    if (!data)
    {
        return false;
    }
    unique_lock<mutex> sdl_locl(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
    {
        return false;
    }

    if (linesize <= 0)
    {
        switch (fmt_)
        {
        case XVideoView::RGBA:
        case XVideoView::ARGB:
            linesize = width_ * 4;
            break;
        case XVideoView::YUV420P:
            linesize = width_;
            break;
        case XVideoView::RGB24:
            linesize = width_ * 3;
            break;
        default:
            break;
        }
    }

    auto re = SDL_UpdateTexture(texture_, NULL, data, linesize);

    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderClear(render_);
    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = scale_w_;
        rect.h = scale_h_;
        prect = &rect;
    }

    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderPresent(render_);
    return true;
}
```
#### 功能：

-   该函数负责将视频帧数据渲染到窗口。
    
-   使用 `SDL_UpdateTexture` 更新纹理数据，将新的帧数据传输到 GPU 上。
    
-   使用 `SDL_RenderCopy` 和 `SDL_RenderPresent` 渲染纹理并显示到屏幕。
    

### 6. **`XSDL::Draw` 函数（YUV格式）**
``` cpp
bool XSDL::Draw(const unsigned char* y, int y_pitch, const unsigned char* u, int u_pitch, const unsigned char* v, int v_pitch)
{
    if (!y || !u || !v)
    {
        return false;
    }
    unique_lock<mutex> sdl_locl(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
    {
        return false;
    }

    auto re = SDL_UpdateYUVTexture(texture_, NULL,
        y, y_pitch,
        u, u_pitch,
        v, v_pitch
    );

    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderClear(render_);
    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = scale_w_;
        rect.h = scale_h_;
        prect = &rect;
    }

    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderPresent(render_);
    return true;
}
```
#### 功能：

-   该函数特别针对 YUV 格式的图像（YUV420）进行处理。
    
-   使用 `SDL_UpdateYUVTexture` 更新 YUV 格式的纹理，并渲染到屏幕。
    

----------

### 总结：

`XSDL` 类封装了 SDL 的窗口和渲染器创建、视频帧绘制、资源释放等操作。它支持多种像素格式的渲染，包括 YUV 和 RGB 格式。主要功能包括：

-   初始化 SDL 视频子系统。
    
-   创建窗口、渲染器和纹理。
    
-   渲染视频帧数据到窗口。
    
-   支持线程安全操作。
