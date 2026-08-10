#include "ffmpeg_test_coding/coding.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
}

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

std::string getErrorText(const int _error_code) {
    char _buffer[AV_ERROR_MAX_STRING_SIZE] = {};
    av_strerror(_error_code, _buffer, sizeof(_buffer));
    return _buffer;
}

bool writeVideoPackets(AVCodecContext* const _codec_context, AVPacket* const _packet,
                       std::ofstream* const _output) {
    while (true) {
        const int _result = avcodec_receive_packet(_codec_context, _packet);
        if (_result == AVERROR(EAGAIN) || _result == AVERROR_EOF) {
            return true;
        }
        if (_result < 0) {
            std::cerr << "Failed to receive an H.264 packet: " << getErrorText(_result) << '\n';
            return false;
        }

        // AVPacket 是压缩后的字节序列。此处直接写入 .h264，得到的是裸 H.264 码流，
        // 还没有 MP4 这类容器提供的索引、时长和音视频轨道信息。
        _output->write(reinterpret_cast<const char*>(_packet->data), _packet->size);
        std::cout << "packet: pts=" << _packet->pts << ", dts=" << _packet->dts
                  << ", size=" << _packet->size
                  << ", key=" << ((_packet->flags & AV_PKT_FLAG_KEY) != 0) << '\n';
        av_packet_unref(_packet);
    }
}

void fillYuv420pFrame(AVFrame* const _frame, const int _frame_index) {
    // YUV420P 有三个平面：Y 为亮度；U、V 为色度。色度平面的宽高各为亮度的一半。
    for (int _y = 0; _y < _frame->height; ++_y) {
        for (int _x = 0; _x < _frame->width; ++_x) {
            _frame->data[0][_y * _frame->linesize[0] + _x] =
                static_cast<std::uint8_t>((_x + _y + _frame_index * 3) % 256);
        }
    }

    for (int _y = 0; _y < _frame->height / 2; ++_y) {
        for (int _x = 0; _x < _frame->width / 2; ++_x) {
            _frame->data[1][_y * _frame->linesize[1] + _x] =
                static_cast<std::uint8_t>((128 + _y + _frame_index * 2) % 256);
            _frame->data[2][_y * _frame->linesize[2] + _x] =
                static_cast<std::uint8_t>((64 + _x + _frame_index * 5) % 256);
        }
    }
}

} // namespace

namespace ffmpeg_test_coding {

int runH264EncodingDemo() {
    const AVCodec* _codec = avcodec_find_encoder_by_name("libx264");
    if (_codec == nullptr) {
        std::cerr << "The local FFmpeg SDK does not contain the libx264 encoder.\n";
        return 1;
    }

    AVCodecContext* _codec_context = avcodec_alloc_context3(_codec);
    AVFrame* _frame = av_frame_alloc();
    AVPacket* _packet = av_packet_alloc();
    if (_codec_context == nullptr || _frame == nullptr || _packet == nullptr) {
        std::cerr << "Failed to allocate FFmpeg encoding objects.\n";
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    const int _width = 320;
    const int _height = 180;
    const int _frame_rate = 25;
    const int _frame_count = 60;

    // time_base 定义 PTS 的单位。这里 1 / 25 秒，因此第 0、1、2 帧的 PTS 分别表示
    // 0 ms、40 ms、80 ms；编码输入的 PTS 必须单调递增。
    _codec_context->width = _width;
    _codec_context->height = _height;
    _codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    _codec_context->time_base = AVRational{1, _frame_rate};
    _codec_context->framerate = AVRational{_frame_rate, 1};
    _codec_context->bit_rate = 400'000;
    _codec_context->gop_size = _frame_rate;
    // 不使用 B 帧可避免重排序，因此本例中输出 packet 的 PTS 和 DTS 相同。
    // 后续把它改为 1 或 2，再观察二者为何不同。
    _codec_context->max_b_frames = 0;

    int _result = avcodec_open2(_codec_context, _codec, nullptr);
    if (_result < 0) {
        std::cerr << "Failed to open libx264: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    _frame->format = _codec_context->pix_fmt;
    _frame->width = _codec_context->width;
    _frame->height = _codec_context->height;
    _result = av_frame_get_buffer(_frame, 0);
    if (_result < 0) {
        std::cerr << "Failed to allocate the raw YUV frame: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    std::error_code _file_error;
    std::filesystem::create_directories("output", _file_error);
    if (_file_error) {
        std::cerr << "Failed to create the output directory: " << _file_error.message() << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    const std::filesystem::path _output_path = "output/h264_demo.h264";
    std::ofstream _output(_output_path, std::ios::binary);
    if (!_output) {
        std::cerr << "Failed to open output file: " << _output_path.string() << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    for (int _frame_index = 0; _frame_index < _frame_count; ++_frame_index) {
        _result = av_frame_make_writable(_frame);
        if (_result < 0) {
            std::cerr << "The raw frame is not writable: " << getErrorText(_result) << '\n';
            av_packet_free(&_packet);
            av_frame_free(&_frame);
            avcodec_free_context(&_codec_context);
            return 1;
        }

        fillYuv420pFrame(_frame, _frame_index);
        _frame->pts = _frame_index;

        // send_frame 只负责把原始帧交给编码器。编码器可能因内部缓冲暂时不输出包，
        // 所以必须紧接着循环 receive_packet，把已经产出的压缩包全部取走。
        _result = avcodec_send_frame(_codec_context, _frame);
        if (_result < 0 || !writeVideoPackets(_codec_context, _packet, &_output)) {
            std::cerr << "Failed to encode frame " << _frame_index << ": " << getErrorText(_result) << '\n';
            av_packet_free(&_packet);
            av_frame_free(&_frame);
            avcodec_free_context(&_codec_context);
            return 1;
        }
    }

    // 传入 nullptr 表示不再有输入帧，用于让编码器吐出内部缓存的剩余 packet。
    _result = avcodec_send_frame(_codec_context, nullptr);
    if (_result < 0 || !writeVideoPackets(_codec_context, _packet, &_output)) {
        std::cerr << "Failed to flush the H.264 encoder: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    std::cout << "H.264 encoding finished: " << _output_path.string() << '\n';
    std::cout << "Play it with: ffplay -f h264 " << _output_path.string() << '\n';

    av_packet_free(&_packet);
    av_frame_free(&_frame);
    avcodec_free_context(&_codec_context);
    return 0;
}

} // namespace ffmpeg_test_coding
