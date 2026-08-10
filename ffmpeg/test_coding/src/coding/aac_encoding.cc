#include "ffmpeg_test_coding/coding.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

#include <cmath>
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

void writeAdtsHeader(std::ofstream* const _output, const int _aac_data_size) {
    // AAC 编码器产生的是一个 AAC access unit。ADTS 在每个 access unit 前补一个 7 字节
    // 头，使 .aac 文件可以直接识别；它只是帧格式，不是 MP4 那种多轨容器。
    const int _profile = 1;       // AAC-LC 的 Audio Object Type 是 2，ADTS 中存 type - 1。
    const int _sample_rate_index = 3; // 48000 Hz 在 ADTS 的索引为 3。
    const int _channel_config = 2;    // 双声道。
    const int _packet_length = _aac_data_size + 7;

    std::uint8_t _header[7] = {};
    _header[0] = 0xff;
    _header[1] = 0xf1;
    _header[2] = static_cast<std::uint8_t>((_profile << 6) | (_sample_rate_index << 2) |
                                            (_channel_config >> 2));
    _header[3] = static_cast<std::uint8_t>(((_channel_config & 3) << 6) | (_packet_length >> 11));
    _header[4] = static_cast<std::uint8_t>((_packet_length >> 3) & 0xff);
    _header[5] = static_cast<std::uint8_t>(((_packet_length & 7) << 5) | 0x1f);
    _header[6] = 0xfc;
    _output->write(reinterpret_cast<const char*>(_header), sizeof(_header));
}

bool writeAudioPackets(AVCodecContext* const _codec_context, AVPacket* const _packet,
                       std::ofstream* const _output) {
    while (true) {
        const int _result = avcodec_receive_packet(_codec_context, _packet);
        if (_result == AVERROR(EAGAIN) || _result == AVERROR_EOF) {
            return true;
        }
        if (_result < 0) {
            std::cerr << "Failed to receive an AAC packet: " << getErrorText(_result) << '\n';
            return false;
        }

        writeAdtsHeader(_output, _packet->size);
        _output->write(reinterpret_cast<const char*>(_packet->data), _packet->size);
        std::cout << "packet: pts=" << _packet->pts << ", duration=" << _packet->duration
                  << ", size=" << _packet->size << '\n';
        av_packet_unref(_packet);
    }
}

void fillStereoSineWave(AVFrame* const _frame, const int _sample_rate, const std::int64_t _first_sample) {
    // AAC 这里使用 FLTP：F 是 float，P 是 planar。即左、右声道分别处于 data[0]、data[1]，
    // 而不是按 L,R,L,R 交错排列。planar/packed 是音频数据布局的关键概念。
    constexpr double pi = 3.14159265358979323846;
    constexpr double frequency = 440.0;
    constexpr double amplitude = 0.2;
    float* const _left_channel = reinterpret_cast<float*>(_frame->data[0]);
    float* const _right_channel = reinterpret_cast<float*>(_frame->data[1]);

    for (int _sample_index = 0; _sample_index < _frame->nb_samples; ++_sample_index) {
        const double _time = static_cast<double>(_first_sample + _sample_index) / _sample_rate;
        const float _sample = static_cast<float>(amplitude * std::sin(2.0 * pi * frequency * _time));
        _left_channel[_sample_index] = _sample;
        _right_channel[_sample_index] = _sample;
    }
}

} // namespace

namespace ffmpeg_test_coding {

int runAacEncodingDemo() {
    const AVCodec* _codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (_codec == nullptr) {
        std::cerr << "The local FFmpeg SDK does not contain the AAC encoder.\n";
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

    const int _sample_rate = 48'000;
    const int _seconds = 2;

    _codec_context->sample_rate = _sample_rate;
    _codec_context->sample_fmt = AV_SAMPLE_FMT_FLTP;
    _codec_context->bit_rate = 128'000;
    _codec_context->profile = AV_PROFILE_AAC_LOW;
    _codec_context->time_base = AVRational{1, _sample_rate};
    av_channel_layout_default(&_codec_context->ch_layout, 2);

    int _result = avcodec_open2(_codec_context, _codec, nullptr);
    if (_result < 0) {
        std::cerr << "Failed to open the AAC encoder: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    _frame->nb_samples = _codec_context->frame_size;
    _frame->format = _codec_context->sample_fmt;
    _frame->sample_rate = _codec_context->sample_rate;
    _result = av_channel_layout_copy(&_frame->ch_layout, &_codec_context->ch_layout);
    if (_result >= 0) {
        _result = av_frame_get_buffer(_frame, 0);
    }
    if (_result < 0) {
        std::cerr << "Failed to allocate the raw PCM frame: " << getErrorText(_result) << '\n';
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

    const std::filesystem::path _output_path = "output/aac_demo.aac";
    std::ofstream _output(_output_path, std::ios::binary);
    if (!_output) {
        std::cerr << "Failed to open output file: " << _output_path.string() << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    const int _frame_count = _seconds * _sample_rate / _frame->nb_samples;
    std::int64_t _next_pts = 0;
    for (int _frame_index = 0; _frame_index < _frame_count; ++_frame_index) {
        _result = av_frame_make_writable(_frame);
        if (_result < 0) {
            std::cerr << "The raw PCM frame is not writable: " << getErrorText(_result) << '\n';
            av_packet_free(&_packet);
            av_frame_free(&_frame);
            avcodec_free_context(&_codec_context);
            return 1;
        }

        fillStereoSineWave(_frame, _sample_rate, _next_pts);
        // 对音频而言 PTS 的单位是一个采样：第一帧为 0，下一帧增加 frame_size（通常 1024）。
        _frame->pts = _next_pts;
        _next_pts += _frame->nb_samples;

        _result = avcodec_send_frame(_codec_context, _frame);
        if (_result < 0 || !writeAudioPackets(_codec_context, _packet, &_output)) {
            std::cerr << "Failed to encode AAC frame " << _frame_index << ": " << getErrorText(_result) << '\n';
            av_packet_free(&_packet);
            av_frame_free(&_frame);
            avcodec_free_context(&_codec_context);
            return 1;
        }
    }

    _result = avcodec_send_frame(_codec_context, nullptr);
    if (_result < 0 || !writeAudioPackets(_codec_context, _packet, &_output)) {
        std::cerr << "Failed to flush the AAC encoder: " << getErrorText(_result) << '\n';
        av_packet_free(&_packet);
        av_frame_free(&_frame);
        avcodec_free_context(&_codec_context);
        return 1;
    }

    std::cout << "AAC encoding finished: " << _output_path.string() << '\n';
    std::cout << "Play it with: ffplay " << _output_path.string() << '\n';

    av_packet_free(&_packet);
    av_frame_free(&_frame);
    avcodec_free_context(&_codec_context);
    return 0;
}

} // namespace ffmpeg_test_coding
