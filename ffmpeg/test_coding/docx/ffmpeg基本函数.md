# FFmpeg 基本函数

本文对应项目中的三个基础示例：

- `src/basic_functions/initialization_demo.cc`：初始化与注册。
- `src/basic_functions/demuxing_demo.cc`：打开媒体、读取压缩包和定位。
- `src/basic_functions/decoding_demo.cc`：查找并打开解码器，把压缩包解码为原始帧。

示例使用当前 SDK 提供的现代 FFmpeg API。参考资料中已经弃用或移除的旧函数，不会强行写进代码，本文会说明其替代方式。

## 一、先认识完整流程

```text
媒体文件或网络流
       │
       │ avformat_open_input / avformat_find_stream_info
       ▼
AVFormatContext（容器和流的信息）
       │
       │ av_read_frame
       ▼
AVPacket（H.264、AAC 等压缩数据）
       │
       │ avcodec_send_packet / avcodec_receive_frame
       ▼
AVFrame（解码后的 YUV 视频帧或 PCM 音频帧）
```

最重要的区分：

- `AVFormatContext` 管理整个输入媒体，例如 MP4 文件、网络地址以及其中的音视频流。
- `AVPacket` 保存从容器中取出的压缩数据，还不是一张能直接显示的图片。
- `AVCodecContext` 保存某一路编解码器的配置和工作状态。
- `AVFrame` 保存解码后的原始音视频数据，例如 YUV 图像或 PCM 音频。

## 二、初始化与注册函数

对应 `initialization_demo.cc`。

### `avdevice_register_all()`

作用：注册 `libavdevice` 支持的输入输出设备，例如摄像头、麦克风和屏幕采集设备。

```cpp
avdevice_register_all();
```

它没有参数和返回值。只有使用设备采集或设备输出时才真正需要；普通本地文件解码一般不依赖它。

### `avformat_network_init()`

作用：初始化 FFmpeg 的网络模块和相关网络加密支持。读取 RTSP、RTMP、HTTP 等网络媒体前通常调用一次。

```cpp
const int _result = avformat_network_init();
```

- 返回 `0` 或正数：成功。
- 返回负数：失败，负数是 FFmpeg 错误码。

它应该在程序级别初始化，而不是每读取一个 `AVPacket` 就调用一次。

### `avformat_network_deinit()`

作用：反初始化网络模块，与 `avformat_network_init()` 成对使用。

```cpp
avformat_network_deinit();
```

### `av_register_all()` 为什么没有出现在示例中

旧版 FFmpeg 要先调用它注册复用器、解复用器和编解码器。现代 FFmpeg 已经自动完成普通组件注册，该函数也已被移除，因此当前项目不应再调用它。

## 三、容器与解封装函数

对应 `demuxing_demo.cc`。

### `avformat_alloc_context()`

作用：申请一个空的 `AVFormatContext`。

```cpp
AVFormatContext* _format_context = avformat_alloc_context();
```

- 成功：返回上下文指针。
- 失败：返回 `nullptr`。

多数简单输入场景可以直接把空指针传给 `avformat_open_input()`，由后者创建上下文。示例单独调用一次，是为了展示它与 `avformat_free_context()` 的配对关系。

### `avformat_free_context()`

作用：释放通过 `avformat_alloc_context()` 创建、但没有交给输入关闭流程管理的格式上下文。

```cpp
avformat_free_context(_format_context);
```

如果上下文已经成功用于打开输入，结束时应调用 `avformat_close_input()`，不要再重复调用本函数。

### `avformat_open_input()`

作用：打开媒体文件、网络 URL 或其他输入源，并识别输入格式。

```cpp
AVFormatContext* _format_context = nullptr;
const int _result = avformat_open_input(
    &_format_context,
    _input_path.c_str(),
    nullptr,
    nullptr);
```

四个参数分别是：

1. `AVFormatContext**`：输出打开后的上下文，所以是二级指针。
2. 输入路径或 URL。
3. 指定输入格式；传 `nullptr` 表示让 FFmpeg 自动探测。
4. 输入选项字典；传 `nullptr` 表示没有额外选项。

返回非负数表示成功，负数表示失败。

### `avformat_find_stream_info()`

作用：读取并分析一部分媒体数据，补充音视频流的编码、时基、帧率等信息。

```cpp
const int _result = avformat_find_stream_info(_format_context, nullptr);
```

仅仅打开容器后，部分流信息可能还不完整，所以通常在 `avformat_open_input()` 后调用它。

### `av_dump_format()`

作用：把容器格式、时长、码率、流和编码参数等概要信息打印到控制台，主要用于调试和学习。

```cpp
av_dump_format(_format_context, 0, _input_path.c_str(), 0);
```

最后一个参数为 `0` 表示输入，为 `1` 表示输出。

### `av_read_frame()`

作用：从解复用器读取下一个压缩数据包。

```cpp
const int _result = av_read_frame(_format_context, _packet);
```

虽然函数名中有 `frame`，它读取到的实际对象却是 `AVPacket`，不是解码后的 `AVFrame`。

- 返回 `0`：成功获得一个包。
- 返回 `AVERROR_EOF`：输入结束。
- 返回其他负数：读取失败。

一个容器中通常同时存在音频、视频等多路流，因此读取后要通过 `_packet->stream_index` 判断该包属于哪一路流。

### `avformat_seek_file()`

作用：在一个时间范围内定位到最接近目标时间的位置。

```cpp
avformat_seek_file(
    _format_context,
    -1,
    _minimum_timestamp,
    _target_timestamp,
    _maximum_timestamp,
    0);
```

- `stream_index` 为 `-1` 时，时间戳使用 `AV_TIME_BASE` 时基。
- `minimum/target/maximum` 给出允许定位的时间范围和目标时间。
- 成功只代表完成了定位请求，实际位置还会受到关键帧和输入格式的限制。

### `av_seek_frame()`

作用：使用较简单的接口定位到某个时间戳附近。

```cpp
av_seek_frame(_format_context, -1, 0, AVSEEK_FLAG_BACKWARD);
```

`AVSEEK_FLAG_BACKWARD` 表示优先选择目标时间之前的可定位位置。视频通常需要从关键帧开始恢复，因此不一定能精确落到任意一帧。

定位后，示例调用 `avformat_flush()` 清理解复用器中定位前缓存的数据。

### `avformat_close_input()`

作用：关闭输入，并释放 `avformat_open_input()` 管理的输入上下文。

```cpp
avformat_close_input(&_format_context);
```

参数是二级指针。关闭后 FFmpeg 会把该指针置为 `nullptr`，可以减少误用已释放地址的风险。

## 四、解码函数

对应 `decoding_demo.cc`。

### `av_find_best_stream()`

作用：在容器的多路流中，查找最适合使用的一路视频流或音频流。

```cpp
const int _stream_index = av_find_best_stream(
    _format_context,
    AVMEDIA_TYPE_VIDEO,
    -1,
    -1,
    nullptr,
    0);
```

成功返回流下标，失败返回负数。实际文件可能有多路音轨或字幕，不能永远假设视频是流 `0`、音频是流 `1`。

### `avcodec_find_decoder()`

作用：根据 `AVCodecID` 查找 FFmpeg 中已经注册的解码器。

```cpp
const AVCodec* _decoder =
    avcodec_find_decoder(_stream->codecpar->codec_id);
```

成功返回只读的 `AVCodec` 描述对象，找不到则返回 `nullptr`。最常用的做法是直接使用输入流中的 `codec_id` 查找。

### `avcodec_find_decoder_by_name()`

作用：根据名字查找解码器，例如 `"h264"`。

```cpp
const AVCodec* _decoder = avcodec_find_decoder_by_name("h264");
```

需要明确指定某个实现时可以使用它；普通播放器通常按 `codec_id` 查找更自然。示例同时展示两种方式，但实际解码并不要求两种都调用。

### `avcodec_alloc_context3()`

作用：为某个解码器申请 `AVCodecContext`。

```cpp
AVCodecContext* _codec_context = avcodec_alloc_context3(_decoder);
```

这个上下文保存解码参数、内部缓存以及解码过程中的状态。失败返回 `nullptr`。

### `avcodec_parameters_to_context()`

作用：把容器流中的编码参数复制到解码器上下文。

```cpp
const int _result = avcodec_parameters_to_context(
    _codec_context,
    _stream->codecpar);
```

`AVCodecParameters` 描述文件中记录了什么编码及其参数；`AVCodecContext` 则是解码器真正工作时使用的上下文。二者用途不同，所以需要这一步复制。

### `avcodec_open2()`

作用：使用已经设置好的 `AVCodecContext` 正式打开解码器。

```cpp
const int _result = avcodec_open2(_codec_context, _decoder, nullptr);
```

第三个参数可传入解码选项字典。成功后才能发送压缩包进行解码。

### `avcodec_send_packet()`

作用：把一个压缩的 `AVPacket` 送入解码器。

```cpp
const int _result = avcodec_send_packet(_codec_context, _packet);
```

它只负责“投递压缩数据”，不保证调用一次就立刻产生一帧，因为解码器可能需要缓存、重排或组合多个包。

输入文件结束后，还要调用 `avcodec_send_packet(_codec_context, nullptr)` 发送空包，把解码器内部因重排而暂存的帧取出来，这个过程称为 flush。

### `avcodec_receive_frame()`

作用：从解码器取出已经解码完成的 `AVFrame`。

```cpp
const int _result = avcodec_receive_frame(_codec_context, _frame);
```

常见返回值：

- `0`：成功取出一帧，可以读取 `_frame`。
- `AVERROR(EAGAIN)`：目前没有更多帧，需要继续发送压缩包。
- `AVERROR_EOF`：解码器已经完全结束。
- 其他负数：发生错误。

正确模式通常是：发送一个包后，循环接收，直到得到 `EAGAIN` 或错误，而不是假定“一个包一定对应一帧”。

### `avcodec_free_context()`

作用：关闭解码器并释放 `AVCodecContext` 及其内部资源。

```cpp
avcodec_free_context(&_codec_context);
```

释放后指针会被置为 `nullptr`。现代代码通常不需要先单独调用 `avcodec_close()`。

## 五、Packet 与 Frame 的内存函数

这些函数不是参考文档的主标题，但要写出安全、可循环运行的示例就必须使用。

| 函数 | 作用 |
| --- | --- |
| `av_packet_alloc()` | 申请一个 `AVPacket` 对象。 |
| `av_packet_unref()` | 释放当前包引用的压缩数据，使同一个包对象可以继续接收下一包。 |
| `av_packet_free()` | 释放包对象本身，并把指针置空。 |
| `av_frame_alloc()` | 申请一个 `AVFrame` 对象。 |
| `av_frame_unref()` | 释放当前帧引用的数据，使帧对象可以复用。 |
| `av_frame_free()` | 释放帧对象本身，并把指针置空。 |

可以把 `unref` 理解为“清空内容，保留盒子继续用”，把 `free` 理解为“连盒子一起释放”。

## 六、旧解码接口与现代替代方式

| 参考资料中的旧函数 | 当前状态 | 现代写法 |
| --- | --- | --- |
| `avcodec_decode_video2()` | 已移除 | `avcodec_send_packet()` + `avcodec_receive_frame()` |
| `avcodec_decode_audio4()` | 已移除 | `avcodec_send_packet()` + `avcodec_receive_frame()` |
| `avcodec_close()` | 不再需要单独使用 | 直接调用 `avcodec_free_context()` |
| `av_register_all()` | 已移除 | 普通组件自动注册 |

学习旧资料时，重点理解它想表达的流程，不要照抄已经被当前头文件删除的函数名。

## 七、三个示例分别看什么

### 初始化示例

在 `main.cc` 中调用：

```cpp
return ffmpeg_test_coding::runInitializationDemo();
```

观察设备注册以及网络模块初始化、反初始化是否成功。

### 解封装示例

```cpp
return ffmpeg_test_coding::runDemuxingDemo(_input_path);
```

重点观察：

- 一个 MP4 中有哪些流。
- `av_read_frame()` 读出的音频包和视频包如何交错出现。
- 每个包的 `stream_index`、`pts`、`dts`、大小和关键帧标志。
- 定位为什么通常受到关键帧限制。

### 解码示例

```cpp
return ffmpeg_test_coding::runDecodingDemo(_input_path);
```

重点观察：

- 输入中的 H.264/AAC 压缩包如何进入不同解码器。
- 视频帧输出宽、高、像素格式和 PTS。
- 音频帧输出采样数、采样率、声道数、采样格式和 PTS。
- `AVPacket` 与 `AVFrame` 不是一一对应关系。

目前 `main.cc` 默认调用解码示例。为了保持入口简单，学习其他类别时只需替换这一行调用。

## 八、构建与运行

项目使用 MSVC 和 C++17，FFmpeg SDK 根目录默认为 `D:/SDK/ffmpeg`。

```powershell
cmake --preset vs2026-x64
cmake --build --preset vs2026-x64-debug
```

运行默认解码示例：

```powershell
.\build\vs2026-x64\Debug\ffmpeg_test_coding.exe
```

传入自己的媒体文件：

```powershell
.\build\vs2026-x64\Debug\ffmpeg_test_coding.exe "D:\videos\sample.mp4"
```

如果只记一条主线，请记住：`avformat` 负责从容器中拿到 `AVPacket`，`avcodec` 负责把 `AVPacket` 解码成 `AVFrame`。
