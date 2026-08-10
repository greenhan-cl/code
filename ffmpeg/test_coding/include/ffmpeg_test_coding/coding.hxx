#ifndef FFMPEG_TEST_CODING_CODING_HXX
#define FFMPEG_TEST_CODING_CODING_HXX

namespace ffmpeg_test_coding {

// 枚举当前 FFmpeg 库中已编译的音频和视频编解码器。
int runCodecListDemo();

// 把 60 帧程序生成的 YUV420P 图像编码为 H.264 裸码流。
int runH264EncodingDemo();

// 把 60 帧程序生成的 YUV420P 图像编码为 H.265/HEVC 裸码流。
int runHevcEncodingDemo();

// 把约 2 秒程序生成的双声道 PCM 浮点采样编码为 AAC-ADTS 码流。
int runAacEncodingDemo();

} // namespace ffmpeg_test_coding

#endif // FFMPEG_TEST_CODING_CODING_HXX
