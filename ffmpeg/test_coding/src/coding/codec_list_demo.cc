#include "ffmpeg_test_coding/coding.hxx"
#include "ffmpeg_test_coding/ffmpeg_info.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include <iostream>

namespace ffmpeg_test_coding {

int runCodecListDemo() {
    // Codec（编解码器）是一套压缩或还原音视频数据的规则，例如 H.264、AAC。
    // 它不同于 MP4：MP4 是容器，H.264/AAC 是容器内可存放的编码规则。
    std::cout << "FFmpeg version: " << getFfmpegVersion() << "\n\n";
    std::cout << "This demo lists codecs compiled into the local FFmpeg SDK.\n";
    std::cout << "It does not inspect a media file yet.\n\n";

    void* _opaque = nullptr;
    const AVCodec* _codec = nullptr;
    int _video_count = 0;
    int _audio_count = 0;
    int _printed_count = 0;
    const int _max_printed_count = 20;

    // av_codec_iterate() 每次返回一个编解码器；_opaque 保存遍历状态。
    while ((_codec = av_codec_iterate(&_opaque)) != nullptr) {
        if (_codec->type != AVMEDIA_TYPE_VIDEO && _codec->type != AVMEDIA_TYPE_AUDIO) {
            continue;
        }

        if (_codec->type == AVMEDIA_TYPE_VIDEO) {
            ++_video_count;
        } else {
            ++_audio_count;
        }

        // 编解码器数量很多，先只显示前 20 个，避免输出淹没关键观察点。
        if (_printed_count >= _max_printed_count) {
            continue;
        }

        const char* _media_type = av_get_media_type_string(_codec->type);
        const char* _role = av_codec_is_decoder(_codec) ? "decoder" : "encoder";
        const char* _long_name = _codec->long_name == nullptr ? "(no description)" : _codec->long_name;

        std::cout << '[' << _media_type << "] " << _role << "\n"
                  << "  name: " << _codec->name << "\n"
                  << "  info: " << _long_name << "\n";
        ++_printed_count;
    }

    std::cout << "\nSummary: " << _video_count << " video codecs, "
              << _audio_count << " audio codecs.\n";
    std::cout << "Observation: a codec describes compression; a container describes how streams are stored together.\n";
    return 0;
}

} // namespace ffmpeg_test_coding
