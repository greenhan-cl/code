# FFmpeg 常用结构体与常用成员

本文依据项目当前使用的 FFmpeg SDK 编写：

- `libavformat 63`
- `libavcodec 63`
- `libavutil 61`

FFmpeg 不同大版本之间可能增加、删除或废弃结构体成员，因此学习时应以当前 SDK 的头文件为准。

## 一、先看结构体之间的关系

```text
AVFormatContext                      一个媒体输入或输出
    │
    ├── streams[0] ──> AVStream     一路视频流
    │                    └── codecpar ──> AVCodecParameters
    │
    └── streams[1] ──> AVStream     一路音频流
                         └── codecpar ──> AVCodecParameters

av_read_frame()
    ↓
AVPacket                             一包压缩数据
    ↓  avcodec_send_packet()
AVCodecContext                       某一路解码器的工作上下文
    ↓  avcodec_receive_frame()
AVFrame                              一帧原始 YUV 图像或 PCM 音频
```

最需要先掌握的七个结构体是：

| 结构体 | 一句话理解 |
| --- | --- |
| `AVFormatContext` | 表示整个媒体文件或网络媒体。 |
| `AVStream` | 表示媒体中的一路流，例如视频流或音频流。 |
| `AVCodecParameters` | 表示容器中记录的某一路编码参数。 |
| `AVCodec` | 描述 FFmpeg 中的一个编码器或解码器实现。 |
| `AVCodecContext` | 编解码器真正工作时使用的上下文。 |
| `AVPacket` | 从容器中读出的压缩数据。 |
| `AVFrame` | 解码后的原始音视频数据。 |

## 二、AVFormatContext：整个媒体的上下文

头文件：`libavformat/avformat.h`

它表示一个完整的输入或输出。例如打开一个 MP4 文件后，可以通过它找到文件中的视频流、音频流、时长和元数据。

常见创建和释放方式：

```cpp
AVFormatContext* _format_context = nullptr;

avformat_open_input(
    &_format_context,
    _input_path.c_str(),
    nullptr,
    nullptr);

avformat_find_stream_info(_format_context, nullptr);

// 使用结束后关闭输入并释放上下文。
avformat_close_input(&_format_context);
```

### 常用成员

| 成员 | 含义 | 使用提示 |
| --- | --- | --- |
| `iformat` | 当前输入使用的解封装格式。 | 只用于输入，例如 MP4、FLV、MPEG-TS。 |
| `oformat` | 当前输出使用的封装格式。 | 只用于输出。 |
| `pb` | 底层的 `AVIOContext`。 | 普通文件读取时通常不需要直接操作。 |
| `nb_streams` | 媒体中流的数量。 | 遍历 `streams` 时作为数组长度。 |
| `streams` | `AVStream*` 指针数组。 | `streams[i]` 表示第 `i` 路流。 |
| `url` | 输入或输出地址。 | 可能是文件路径，也可能是网络 URL。 |
| `start_time` | 整个媒体的开始时间。 | 单位是 `AV_TIME_BASE`，可能为 `AV_NOPTS_VALUE`。 |
| `duration` | 整个媒体的时长。 | 单位是 `AV_TIME_BASE`，不是毫秒。 |
| `bit_rate` | 媒体总码率。 | 单位是 bit/s，未知时可能为 `0`。 |
| `metadata` | 整个媒体的元数据字典。 | 例如标题、作者、编码器。 |
| `flags` | 控制解封装或封装行为的标志。 | 使用 `AVFMT_FLAG_*` 组合。 |
| `probesize` | 探测流信息时最多读取的字节数。 | 网络流首开速度和探测准确度之间的权衡。 |
| `max_analyze_duration` | 分析流信息的最大媒体时长。 | 单位是 `AV_TIME_BASE`。 |
| `interrupt_callback` | 自定义 I/O 中断回调。 | 常用于网络读取超时或用户取消。 |

### 遍历所有流

```cpp
for (unsigned int _index = 0;
     _index < _format_context->nb_streams;
     ++_index) {
    AVStream* _stream = _format_context->streams[_index];
    // 使用 _stream。
}
```

### duration 如何换算为秒

`AVFormatContext::duration` 使用的是 `AV_TIME_BASE`：

```cpp
const double _duration_seconds =
    static_cast<double>(_format_context->duration) / AV_TIME_BASE;
```

`AV_TIME_BASE` 通常是 `1000000`，因此这里的基本单位是微秒。

### 所有权

- `avformat_open_input()` 创建并管理输入上下文。
- `avformat_close_input()` 关闭输入、释放上下文，并把指针置为 `nullptr`。
- `streams` 以及其中的 `AVStream` 由 `AVFormatContext` 管理，不要单独释放。

## 三、AVStream：媒体中的一路流

头文件：`libavformat/avformat.h`

一个 MP4 通常至少有两路流：

```text
streams[0]：H.264 视频流
streams[1]：AAC 音频流
```

### 常用成员

| 成员 | 含义 | 使用提示 |
| --- | --- | --- |
| `index` | 这路流在 `AVFormatContext::streams` 中的下标。 | 与 `AVPacket::stream_index` 对应。 |
| `id` | 容器内部的流 ID。 | 不一定等于数组下标。 |
| `codecpar` | 这路流的编码参数。 | 最常使用的成员。 |
| `time_base` | 这路流时间戳的基本单位。 | PTS、DTS 和 duration 都依赖它。 |
| `start_time` | 这路流的开始时间戳。 | 使用本流的 `time_base`。 |
| `duration` | 这路流的时长。 | 使用本流的 `time_base`。 |
| `nb_frames` | 流中的帧数。 | 不一定准确；未知时为 `0`。 |
| `avg_frame_rate` | 平均帧率。 | 视频流常用。 |
| `r_frame_rate` | FFmpeg 推测的基础帧率。 | 它只是估计值，不要盲目当成真实帧率。 |
| `sample_aspect_ratio` | 像素宽高比。 | 视频流使用。 |
| `metadata` | 这一路流的元数据。 | 例如语言、轨道标题。 |
| `disposition` | 默认流、附图等属性标志。 | 使用 `AV_DISPOSITION_*` 判断。 |
| `discard` | 是否丢弃该流中的部分或全部数据。 | 播放器选择音轨时可能使用。 |

### time_base 是什么

假设：

```cpp
_stream->time_base.num == 1;
_stream->time_base.den == 12800;
```

那么一个时间戳单位就是：

```text
1 / 12800 秒
```

时间戳 `512` 对应：

```text
512 × 1 / 12800 = 0.04 秒
```

推荐使用 FFmpeg 函数转换，不要在整数之间直接相除：

```cpp
const double _seconds =
    _packet->pts * av_q2d(_stream->time_base);
```

### 找出 Packet 属于哪一路流

```cpp
AVStream* _stream =
    _format_context->streams[_packet->stream_index];
```

### 所有权

`AVStream` 由 `AVFormatContext` 管理，不要对 `_stream` 调用 `delete` 或 `av_free()`。

## 四、AVCodecParameters：容器中记录的编码参数

头文件：`libavcodec/codec_par.h`

`AVCodecParameters` 描述“文件里记录了什么”，它本身不是正在工作的解码器。

```cpp
AVCodecParameters* _codec_parameters = _stream->codecpar;
```

### 通用成员

| 成员 | 含义 |
| --- | --- |
| `codec_type` | 媒体类型，例如视频、音频、字幕。 |
| `codec_id` | 编码格式 ID，例如 `AV_CODEC_ID_H264`、`AV_CODEC_ID_AAC`。 |
| `codec_tag` | 容器中的编码标签，例如 MP4 中的 `avc1`、`mp4a`。 |
| `format` | 视频时表示像素格式，音频时表示采样格式。 |
| `bit_rate` | 这路编码数据的平均码率，单位 bit/s。 |
| `profile` | 编码 Profile，例如 H.264 High、AAC LC。 |
| `level` | 编码 Level。 |
| `extradata` | 初始化解码器所需的额外编码数据。 |
| `extradata_size` | `extradata` 的有效字节数。 |

`extradata` 的具体内容取决于编码格式。例如 MP4 中的 H.264 参数集通常会放在容器的额外数据中。一般不需要自己解析，复制到 `AVCodecContext` 即可。

### 视频常用成员

| 成员 | 含义 |
| --- | --- |
| `width`、`height` | 视频宽度和高度。 |
| `sample_aspect_ratio` | 像素宽高比。 |
| `framerate` | 编码层声明的帧率，未知时可能为 `{0, 1}`。 |
| `field_order` | 逐行或隔行扫描相关信息。 |
| `color_range` | 有限范围或全范围。 |
| `color_primaries` | 色彩原色标准。 |
| `color_trc` | 色彩传递特性。 |
| `color_space` | YUV 到 RGB 使用的色彩空间。 |
| `chroma_location` | 色度采样位置。 |
| `video_delay` | 编码造成的帧延迟信息。 |

### 音频常用成员

| 成员 | 含义 |
| --- | --- |
| `sample_rate` | 采样率，例如 48000 Hz。 |
| `ch_layout` | 声道数量、顺序和布局。 |
| `frame_size` | 固定音频帧每声道包含的采样数。 |
| `block_align` | 某些音频格式中每个数据块的字节对齐。 |
| `initial_padding` | 编码器在开头添加的填充采样数。 |
| `trailing_padding` | 编码器在结尾添加的填充采样数。 |
| `seek_preroll` | 定位后正式输出前需要预先解码的采样数。 |

### 如何交给解码器

不要逐个成员手动复制，应调用：

```cpp
avcodec_parameters_to_context(
    _codec_context,
    _stream->codecpar);
```

## 五、AVCodec：编解码器实现的描述

头文件：`libavcodec/codec.h`

`AVCodec` 描述 FFmpeg 中的一个编码器或解码器实现。它更像一份只读说明，而不是正在运行的解码器。

```cpp
const AVCodec* _decoder =
    avcodec_find_decoder(_stream->codecpar->codec_id);
```

### 常用成员

| 成员 | 含义 |
| --- | --- |
| `name` | 简短名称，例如 `h264`、`aac`。 |
| `long_name` | 更容易阅读的完整名称。 |
| `type` | 媒体类型，例如 `AVMEDIA_TYPE_VIDEO`。 |
| `id` | 对应的 `AVCodecID`。 |
| `capabilities` | 编解码器能力标志，使用 `AV_CODEC_CAP_*` 判断。 |
| `max_lowres` | 解码器支持的最大低分辨率级别。 |
| `profiles` | 支持或识别的 Profile 列表。 |
| `wrapper_name` | 外部库或硬件包装器名称。 |

### AVCodec 与 AVCodecContext 的区别

```text
AVCodec
    描述“使用哪一种解码器实现”
    通常是只读、全局共享的

AVCodecContext
    描述“这一次解码任务的参数和状态”
    每一路需要独立创建和释放
```

### 所有权

`avcodec_find_decoder()` 返回的 `AVCodec` 由 FFmpeg 管理：

- 使用 `const AVCodec*`。
- 不要修改。
- 不要释放。

## 六、AVCodecContext：编解码器工作上下文

头文件：`libavcodec/avcodec.h`

`AVCodecContext` 保存某一路编码或解码任务的配置、缓存和运行状态。

典型解码创建流程：

```cpp
const AVCodec* _decoder =
    avcodec_find_decoder(_stream->codecpar->codec_id);

AVCodecContext* _codec_context =
    avcodec_alloc_context3(_decoder);

avcodec_parameters_to_context(
    _codec_context,
    _stream->codecpar);

avcodec_open2(_codec_context, _decoder, nullptr);
```

### 通用成员

| 成员 | 含义 | 使用提示 |
| --- | --- | --- |
| `codec_type` | 视频、音频等媒体类型。 | 使用 `AVMEDIA_TYPE_*`。 |
| `codec` | 当前使用的 `AVCodec`。 | 打开成功后可查看。 |
| `codec_id` | 当前编码格式 ID。 | 例如 H.264、HEVC、AAC。 |
| `bit_rate` | 编码目标码率或解码流码率。 | 编码时经常主动设置。 |
| `flags`、`flags2` | 编解码行为标志。 | 使用 `AV_CODEC_FLAG_*`。 |
| `extradata` | 解码初始化所需的额外数据。 | 通常由参数复制函数设置。 |
| `extradata_size` | 额外数据大小。 | 不要随意修改。 |
| `time_base` | 编码时间基。 | 编码时必须重点设置；解码时通常不靠它解释输入 Packet。 |
| `pkt_timebase` | 解码输入包的时间基。 | 一般对应 `AVStream::time_base`。 |
| `framerate` | 帧率信息。 | 视频编码常用。 |
| `delay` | 编解码器内部延迟。 | 解释为什么输入与输出不是立即一一对应。 |
| `thread_count` | 线程数量。 | `0` 通常表示由 FFmpeg 自动决定。 |
| `thread_type` | 允许使用的线程方式。 | 例如帧级、切片级线程。 |
| `active_thread_type` | 实际启用的线程方式。 | 由 FFmpeg 决定。 |

### 视频常用成员

| 成员 | 含义 |
| --- | --- |
| `width`、`height` | 输出或输入画面的宽高。 |
| `coded_width`、`coded_height` | 码流中编码尺寸，可能包含裁剪区域。 |
| `pix_fmt` | 像素格式，例如 `AV_PIX_FMT_YUV420P`。 |
| `sw_pix_fmt` | 软件解码对应的像素格式。 |
| `sample_aspect_ratio` | 像素宽高比。 |
| `color_range` | 色彩范围。 |
| `color_primaries` | 色彩原色标准。 |
| `color_trc` | 色彩传递特性。 |
| `colorspace` | 色彩空间。 |
| `field_order` | 逐行或隔行相关信息。 |
| `has_b_frames` | 解码器的帧重排缓存大小信息。 |
| `gop_size` | 编码时 GOP 的目标大小。 |
| `max_b_frames` | 编码时连续 B 帧的最大数量。 |

### 音频常用成员

| 成员 | 含义 |
| --- | --- |
| `sample_rate` | 采样率。 |
| `sample_fmt` | 采样格式，例如 `AV_SAMPLE_FMT_FLTP`。 |
| `ch_layout` | 声道数量和布局。 |
| `frame_size` | 编码器或解码器的音频帧采样数。 |

### 解码时不要盲信 Context 中的帧属性

解码器可能在解析码流后更新分辨率、像素格式等信息，而且媒体中途还可能改变参数。对于已经输出的一帧，应该优先读取：

```cpp
_frame->width;
_frame->height;
_frame->format;
```

### 所有权

```cpp
AVCodecContext* _codec_context =
    avcodec_alloc_context3(_decoder);

// 使用……

avcodec_free_context(&_codec_context);
```

`avcodec_free_context()` 会关闭编解码器、释放内部资源，并把指针置为 `nullptr`。

## 七、AVPacket：一包压缩数据

头文件：`libavcodec/packet.h`

`AVPacket` 通常由 `av_read_frame()` 从容器中读取。它保存的是 H.264、AAC 等压缩数据，而不是原始像素或 PCM 采样。

### 常用成员

| 成员 | 含义 | 使用提示 |
| --- | --- | --- |
| `data` | 压缩数据首地址。 | 二进制数据，不是字符串。 |
| `size` | 压缩数据字节数。 | 不同 Packet 大小可以不同。 |
| `stream_index` | 该包属于哪一路流。 | 用它选择视频或音频解码器。 |
| `pts` | 显示时间戳。 | 使用对应 `AVStream::time_base`。 |
| `dts` | 解码时间戳。 | 有 B 帧时可能和 PTS 不同。 |
| `duration` | 这个包覆盖的媒体时长。 | 使用对应流的 `time_base`。 |
| `flags` | 关键包、损坏包等标志。 | 使用 `AV_PKT_FLAG_*`。 |
| `pos` | 包在输入中的字节位置。 | 未知时为 `-1`。 |
| `side_data` | 容器附带的额外信息。 | 高级场景使用。 |
| `side_data_elems` | Side Data 条目数量。 | 与 `side_data` 配套。 |
| `buf` | 引用计数缓冲区。 | 通常不直接操作。 |
| `time_base` | Packet 自身声明的时间基。 | 当前很多解封装、解码流程仍主要使用流的时间基。 |

### 判断关键包

```cpp
const bool _is_key_packet =
    (_packet->flags & AV_PKT_FLAG_KEY) != 0;
```

### PTS 转换为秒

```cpp
if (_packet->pts != AV_NOPTS_VALUE) {
    const AVStream* _stream =
        _format_context->streams[_packet->stream_index];

    const double _pts_seconds =
        _packet->pts * av_q2d(_stream->time_base);
}
```

### 生命周期

```cpp
AVPacket* _packet = av_packet_alloc();

while (av_read_frame(_format_context, _packet) >= 0) {
    // 使用当前 Packet。
    av_packet_unref(_packet);
}

av_packet_free(&_packet);
```

可以这样理解：

```text
av_packet_unref()：清空盒子里的当前数据，盒子继续复用
av_packet_free() ：把盒子本身也释放
```

## 八、AVFrame：一帧原始音视频数据

头文件：`libavutil/frame.h`

视频解码后，`AVFrame` 通常保存 YUV 像素；音频解码后，通常保存 PCM 采样。

### 通用成员

| 成员 | 含义 | 使用提示 |
| --- | --- | --- |
| `data` | 各数据平面的地址。 | 视频和音频的解释方式不同。 |
| `linesize` | 每个平面一行的实际跨度。 | 可能大于有效图像宽度。 |
| `extended_data` | 扩展的数据平面数组。 | 多声道 planar 音频重点使用。 |
| `format` | 像素格式或音频采样格式。 | 由这帧的媒体类型决定如何解释。 |
| `pts` | 这帧的显示时间戳。 | 根据 `time_base` 换算。 |
| `pkt_dts` | 关联输入包的 DTS。 | 未知时可能为 `AV_NOPTS_VALUE`。 |
| `time_base` | 这帧时间戳的单位。 | 与 `pts` 配套。 |
| `duration` | 这帧持续的媒体时间。 | 使用帧的 `time_base`。 |
| `flags` | 关键帧、损坏帧等标志。 | 使用 `AV_FRAME_FLAG_*`。 |
| `metadata` | 与这帧关联的元数据。 | 不是每帧都会有。 |
| `best_effort_timestamp` | FFmpeg 根据已有信息估算的显示时间戳。 | 输入时间戳不完整时可能有用。 |

### 视频常用成员

| 成员 | 含义 |
| --- | --- |
| `width`、`height` | 当前视频帧的宽度和高度。 |
| `format` | `AVPixelFormat`，例如 `AV_PIX_FMT_YUV420P`。 |
| `pict_type` | I、P、B 等图像类型。 |
| `sample_aspect_ratio` | 当前帧的像素宽高比。 |
| `color_range` | 当前帧的色彩范围。 |
| `colorspace` | 当前帧使用的色彩空间。 |

### YUV420P 中 data 与 linesize 的关系

```text
data[0]：Y 平面
data[1]：U 平面
data[2]：V 平面

linesize[0]：Y 平面每行在内存中的跨度
linesize[1]：U 平面每行在内存中的跨度
linesize[2]：V 平面每行在内存中的跨度
```

读取某行 Y 数据：

```cpp
const uint8_t* _y_row =
    _frame->data[0] + _row * _frame->linesize[0];
```

不要写成：

```cpp
_frame->data[0] + _row * _frame->width;
```

因为 `linesize[0]` 可能为了内存对齐而大于 `width`。忽略 `linesize` 是花屏、错位和绿屏的常见原因。

### 判断视频帧类型和关键帧

```cpp
const AVPictureType _picture_type = _frame->pict_type;

const bool _is_key_frame =
    (_frame->flags & AV_FRAME_FLAG_KEY) != 0;
```

当前 SDK 应使用 `flags` 判断关键帧。旧教程中直接访问 `key_frame` 的写法不应作为新代码依据。

### 音频常用成员

| 成员 | 含义 |
| --- | --- |
| `nb_samples` | 当前帧每个声道包含的采样数。 |
| `sample_rate` | 当前帧采样率。 |
| `ch_layout` | 当前帧的声道数量和布局。 |
| `format` | `AVSampleFormat`，例如 `AV_SAMPLE_FMT_FLTP`。 |
| `extended_data` | 音频各平面或声道数据地址。 |

如果格式是 planar，例如 `AV_SAMPLE_FMT_FLTP`：

```text
extended_data[0]：第 0 声道
extended_data[1]：第 1 声道
……
```

如果格式是 packed，例如 `AV_SAMPLE_FMT_S16`：

```text
data[0]：所有声道交错保存

L R L R L R ...
```

### 生命周期

```cpp
AVFrame* _frame = av_frame_alloc();

while (avcodec_receive_frame(_codec_context, _frame) >= 0) {
    // 使用当前 Frame。
    av_frame_unref(_frame);
}

av_frame_free(&_frame);
```

## 九、AVRational：分数和时间基

头文件：`libavutil/rational.h`

```cpp
typedef struct AVRational {
    int num;
    int den;
} AVRational;
```

- `num`：分子。
- `den`：分母。

例如：

```cpp
AVRational _time_base{1, 12800};
AVRational _frame_rate{25, 1};
```

它们分别表示：

```text
时间基：1 / 12800 秒
帧率：25 / 1 = 25 FPS
```

### 常用辅助函数

```cpp
const double _value = av_q2d(_time_base);
```

把一个时间戳从旧时间基转换到新时间基：

```cpp
const int64_t _new_timestamp = av_rescale_q(
    _old_timestamp,
    _old_time_base,
    _new_time_base);
```

在封装、转码和音视频同步中，`av_rescale_q()` 比手写乘除更安全，因为它会考虑整数精度和溢出问题。

## 十、AVChannelLayout：音频声道布局

头文件：`libavutil/channel_layout.h`

常用成员：

| 成员 | 含义 |
| --- | --- |
| `order` | 声道的排列方式。 |
| `nb_channels` | 声道数量。 |
| `u.mask` | 原生声道布局的位掩码。 |
| `u.map` | 自定义声道排列时的声道映射表。 |
| `opaque` | 用户或内部扩展数据。 |

最常读取的是：

```cpp
const int _channel_count =
    _frame->ch_layout.nb_channels;
```

常见布局常量：

```cpp
AVChannelLayout _mono = AV_CHANNEL_LAYOUT_MONO;
AVChannelLayout _stereo = AV_CHANNEL_LAYOUT_STEREO;
```

需要复制布局时，不建议直接依赖简单赋值，应使用：

```cpp
av_channel_layout_copy(&_destination, &_source);
```

不再使用时：

```cpp
av_channel_layout_uninit(&_destination);
```

这是因为自定义布局中可能包含需要管理的映射数组。

## 十一、AVDictionary 与 AVDictionaryEntry：元数据和选项

头文件：`libavutil/dict.h`

`AVDictionary` 本身是一个不公开内部成员的结构，应通过函数操作。

`AVDictionaryEntry` 有两个常用成员：

| 成员 | 含义 |
| --- | --- |
| `key` | 键，例如 `title`、`encoder`。 |
| `value` | 对应的字符串值。 |

### 读取指定元数据

```cpp
const AVDictionaryEntry* _entry = av_dict_get(
    _format_context->metadata,
    "encoder",
    nullptr,
    0);

if (_entry != nullptr) {
    std::cout << _entry->value << '\n';
}
```

### 遍历全部元数据

```cpp
const AVDictionaryEntry* _entry = nullptr;

while ((_entry = av_dict_iterate(
            _format_context->metadata,
            _entry)) != nullptr) {
    std::cout << _entry->key
              << " = "
              << _entry->value
              << '\n';
}
```

### 设置并释放字典

```cpp
AVDictionary* _options = nullptr;
av_dict_set(&_options, "rtsp_transport", "tcp", 0);

// 使用 _options……

av_dict_free(&_options);
```

## 十二、AVIOContext：底层输入输出

头文件：`libavformat/avio.h`

`AVIOContext` 位于协议/I/O 层，负责从文件、内存或网络中读取和写入字节。普通文件解封装时，通常由 `avformat_open_input()` 自动创建，不需要直接操作。

### 常用或常见成员

| 成员 | 含义 | 是否建议直接修改 |
| --- | --- | --- |
| `buffer` | I/O 缓冲区起始地址。 | 否。 |
| `buffer_size` | 缓冲区大小。 | 自定义 I/O 创建时设置。 |
| `buf_ptr` | 当前缓冲区读写位置。 | 否。 |
| `buf_end` | 当前有效缓冲区末尾。 | 否。 |
| `opaque` | 传递给自定义回调的用户数据。 | 自定义 I/O 时设置。 |
| `read_packet` | 自定义读取回调。 | 自定义 I/O 时设置。 |
| `write_packet` | 自定义写入回调。 | 自定义 I/O 时设置。 |
| `seek` | 自定义定位回调。 | 自定义 I/O 时设置。 |
| `pos` | 当前底层位置。 | 主要读取。 |
| `eof_reached` | 是否到达输入结尾。 | 主要读取。 |
| `error` | 最近的 I/O 错误码。 | 主要读取。 |
| `seekable` | 输入是否支持定位。 | 使用 `AVIO_SEEKABLE_*` 判断。 |
| `bytes_read` | 已读取的累计字节数。 | 只读统计。 |
| `bytes_written` | 已写入的累计字节数。 | 只读统计。 |

只有在“从一块内存解封装”“读取自定义加密文件”等场景中，才需要重点学习 `AVIOContext`。

## 十三、SwsContext 与 SwrContext

这两个也是常见的 FFmpeg 上下文，但它们是内部细节不公开的结构体：

| 结构体 | 所属库 | 作用 |
| --- | --- | --- |
| `SwsContext` | `libswscale` | 图像像素格式转换和缩放。 |
| `SwrContext` | `libswresample` | 音频采样率、采样格式和声道布局转换。 |

不要尝试访问它们的成员，应通过对应 API 创建、配置和释放。

## 十四、最重要的所有权表

| 对象 | 通常如何获得 | 如何释放 | 备注 |
| --- | --- | --- | --- |
| `AVFormatContext` | `avformat_open_input()` | `avformat_close_input()` | 输入场景。 |
| `AVStream` | `format_context->streams[i]` | 不单独释放 | 归 `AVFormatContext` 所有。 |
| `AVCodecParameters` | `stream->codecpar` | 不单独释放 | 归 `AVStream` 所有。 |
| `AVCodec` | `avcodec_find_decoder()` | 不释放 | FFmpeg 管理的只读对象。 |
| `AVCodecContext` | `avcodec_alloc_context3()` | `avcodec_free_context()` | 每路编解码任务独立创建。 |
| `AVPacket` | `av_packet_alloc()` | `av_packet_free()` | 每轮复用前后使用 `av_packet_unref()`。 |
| `AVFrame` | `av_frame_alloc()` | `av_frame_free()` | 每轮复用前后使用 `av_frame_unref()`。 |
| `AVDictionary` | `av_dict_set()` 等 | `av_dict_free()` | 内部成员不公开。 |
| `AVChannelLayout` | 初始化或复制 | `av_channel_layout_uninit()` | 自定义布局可能拥有动态内存。 |

## 十五、阅读代码时先追这条路径

看到 FFmpeg 解码代码时，先按下面顺序追踪，不要一开始研究结构体中的所有成员：

```text
AVFormatContext
    ↓ streams[stream_index]
AVStream
    ↓ codecpar
AVCodecParameters
    ↓ avcodec_parameters_to_context()
AVCodecContext

AVFormatContext
    ↓ av_read_frame()
AVPacket
    ↓ avcodec_send_packet()
AVCodecContext
    ↓ avcodec_receive_frame()
AVFrame
```

第一阶段只需熟练掌握这些成员：

```text
AVFormatContext：nb_streams、streams、duration
AVStream       ：index、codecpar、time_base
AVCodecParameters：codec_type、codec_id、width、height、sample_rate、ch_layout
AVPacket       ：stream_index、data、size、pts、dts、flags
AVFrame        ：data、linesize、width、height、nb_samples、format、pts、ch_layout
AVCodecContext ：codec_id、width、height、pix_fmt、sample_rate、sample_fmt、ch_layout
```

先理解这些成员，就能读懂大部分基础的解封装和解码代码。其他成员遇到具体需求时再查头文件即可。
