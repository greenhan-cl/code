#include "ffmpeg_test_coding/coding.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
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
            std::cerr << "Failed to receive an HEVC packet: " << getErrorText(_result) << '\n';
            return false;
        }

        _output->write(reinterpret_cast<const char*>(_packet->data), _packet->size);
        std::cout << "packet: pts=" << _packet->pts << ", dts=" << _packet->dts
                  << ", size=" << _packet->size
                  << ", key=" << ((_packet->flags & AV_PKT_FLAG_KEY) != 0) << '\n';
        av_packet_unref(_packet);
    }
}

void fillYuv420pFrame(AVFrame* const _frame, const int _frame_index) {
    for (int _y = 0; _y < _frame->height; ++_y) {
        for (int _x = 0; _x < _frame->width; ++_x) {
            _frame->data[0][_y * _frame->linesize[0] + _x] =
                static_cast<std::uint8_t>((_x * 2 + _y + _frame_index * 4) % 256);
        }
    }

    for (int _y = 0; _y < _frame->height / 2; ++_y) {
        for (int _x = 0; _x < _frame->width / 2; ++_x) {
            _frame->data[1][_y * _frame->linesize[1] + _x] =
                static_cast<std::uint8_t>((96 + _x + _frame_index * 3) % 256);
            _frame->data[2][_y * _frame->linesize[2] + _x] =
                static_cast<std::uint8_t>((160 + _y + _frame_index * 2) % 256);
        }
    }
}

} // namespace

namespace ffmpeg_test_coding {

int runHevcEncodingDemo() {
    const AVCodec* _codec = avcodec_find_encoder_by_name("libx265");
    if (_codec == nullptr) {
        std::cerr << "The local FFmpeg SDK does not contain the libx265 encoder.\n";
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

    _codec_context->width = _width;
    _codec_context->height = _height;
    _codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    _codec_context->time_base = AVRational{1, _frame_rate};
    _codec_context->framerate = AVRational{_frame_rate, 1};
    _codec_context->bit_rate = 300'000;
    _codec_context->gop_size = _frame_rate;
    // 不使用 B 帧可避免重排序，因此本例中输出 packet 的 PTS 和 DTS 相同。
    _codec_context->max_b_frames = 0;

    // libx265 的预设属于编码器私有选项；ultrafast 使这个教学示例能很快跑完。
    av_opt_set(_codec_context->priv_data, "preset", "ultrafast", 0);
    av_opt_set(_codec_context->priv_data, "tune", "zerolatency", 0);

    int _result = avcodec_open2(_codec_context, _codec, nullptr);
    if (_result < 0) {
        std::cerr << "Failed to open libx265: " << getErrorText(_result) << '\n';
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

    const std::filesystem::path _output_path = "output/hevc_demo.h265";
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

        // H.265 和 H.264 使用完全相同的 FFmpeg 编码 API；差别主要来自选中的编码器
        // 以及各自的压缩工具、编码参数和产生的码流格式。
        _result = avcodec_send_frame(_codec_context, _frame);
        if (_result < 0 || !writeVideoPackets(_codec_context, _packet, &_output)) {
            std::cerr << "Failed to encode frame " << _frame_index << ": " << getErrorText(_result) << '\n';
            av_packet_free(&_packet);
            av_frame_free(&_frame);
            avcodec_free_context(&_codec_context);
            return 1;
        }
    }

    _result = avcodec_send_frame(_codec_context, nullptr);
    if (_result < 0 || !writeVideoPackets(_codec_context, _packet, &_output)) {
        std::cerr << "Failed to flush the HEVC encoder: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    std::cout << "HEVC encoding finished: " << _output_path.string() << '\n';
    std::cout << "Play it with: ffplay -f hevc " << _output_path.string() << '\n';

    av_packet_free(&_packet);
    av_frame_free(&_frame);
    avcodec_free_context(&_codec_context);
    return 0;
}

} // namespace ffmpeg_test_coding
