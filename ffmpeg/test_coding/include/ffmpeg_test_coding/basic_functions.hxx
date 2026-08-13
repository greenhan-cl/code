#ifndef FFMPEG_TEST_CODING_BASIC_FUNCTIONS_HXX
#define FFMPEG_TEST_CODING_BASIC_FUNCTIONS_HXX

#include <string>

namespace ffmpeg_test_coding {

// 注册设备组件，并演示网络模块的初始化和反初始化。
int runInitializationDemo();

// 打开媒体、读取流信息与压缩包，并演示两种定位接口。
int runDemuxingDemo(const std::string& _input_path);

// 解封装媒体并分别解码其中的视频流和音频流。
int runDecodingDemo(const std::string& _input_path);

// 解码视频流，并使用 SDL2 将解码后的 YUV 帧显示在窗口中。
int runSdlVideoDemo(const std::string& _input_path);

} // namespace ffmpeg_test_coding

#endif // FFMPEG_TEST_CODING_BASIC_FUNCTIONS_HXX
