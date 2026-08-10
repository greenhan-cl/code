#include "ffmpeg_test_coding/basic_functions.hxx"
#include "ffmpeg_error.hxx"

extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}

#include <iostream>

namespace ffmpeg_test_coding {

int runInitializationDemo() {
    // FFmpeg 4.0 之后不再需要 av_register_all()；普通组件由 FFmpeg 自动注册。
    // 设备组件仍通过 avdevice_register_all() 注册，例如摄像头、麦克风和屏幕采集。
    avdevice_register_all();
    std::cout << "Device components registered.\n";

    // 网络初始化应在进程级别成对调用，不要在每读取一个 packet 时反复初始化。
    const int _result = avformat_network_init();
    if (_result < 0) {
        std::cerr << "Failed to initialize FFmpeg network support: "
                  << detail::getFfmpegErrorText(_result) << '\n';
        return 1;
    }

    std::cout << "Network support initialized.\n";
    avformat_network_deinit();
    std::cout << "Network support deinitialized.\n";
    return 0;
}

} // namespace ffmpeg_test_coding
