#include "ffmpeg_test_coding/basic_functions.hxx"
#include "ffmpeg_error.hxx"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace {

void printPacket(const AVFormatContext& _format_context, const AVPacket& _packet) {
    const AVStream* _stream = _format_context.streams[_packet.stream_index];
    const char* _type_name = av_get_media_type_string(_stream->codecpar->codec_type);

    std::cout << "packet: stream=" << _packet.stream_index
              << ", type=" << (_type_name == nullptr ? "unknown" : _type_name)
              << ", pts=" << _packet.pts
              << ", dts=" << _packet.dts
              << ", size=" << _packet.size
              << ", key=" << ((_packet.flags & AV_PKT_FLAG_KEY) != 0) << '\n';
}

int readPackets(AVFormatContext* const _format_context, const int _max_packet_count) {
	// 申请 AVPacket 内存
    AVPacket* _packet = av_packet_alloc();
    if (_packet == nullptr) {
        std::cerr << "Failed to allocate AVPacket.\n";
        return AVERROR(ENOMEM);
    }

    int _result = 0;
    for (int _packet_index = 0; _packet_index < _max_packet_count; ++_packet_index) {
		// av_read_frame() 会从输入流中读取一个压缩包，返回值 < 0 表示读取失败或已到达文件末尾。
        _result = av_read_frame(_format_context, _packet);
        if (_result < 0) {
            break;
        }
		// 打印压缩包信息
        printPacket(*_format_context, *_packet);
		// 释放 AVPacket 内部的引用计数数据
        av_packet_unref(_packet);
    }
	// 释放 AVPacket 内存
    av_packet_free(&_packet);
    return _result == AVERROR_EOF ? 0 : _result;
}

void demonstrateSeeking(AVFormatContext* const _format_context) {
    if (_format_context->duration <= 0 || _format_context->duration == AV_NOPTS_VALUE) {
        std::cout << "Input duration is unknown; seeking demo was skipped.\n";
        return;
    }

	// 计算输入时长的一半，尝试跳转到中间位置。
    const std::int64_t _middle_timestamp = _format_context->duration / 2;
	// avformat_seek_file() 可以在指定的时间戳范围内跳转到最接近目标时间戳的位置。
    int _result = avformat_seek_file(
        _format_context,
        -1,
        std::numeric_limits<std::int64_t>::min(),
        _middle_timestamp,
        std::numeric_limits<std::int64_t>::max(),
        0);

    if (_result >= 0) {
		// avformat_flush() 会清空解码器的缓冲区，确保后续读取的数据是从新位置开始的。
        avformat_flush(_format_context);
        std::cout << "avformat_seek_file(): moved near the middle of the input.\n";
        readPackets(_format_context, 1);
    } else {
        std::cout << "avformat_seek_file() is not supported for this input: "
                  << ffmpeg_test_coding::detail::getFfmpegErrorText(_result) << '\n';
    }

    _result = av_seek_frame(_format_context, -1, 0, AVSEEK_FLAG_BACKWARD);
    if (_result >= 0) {
        avformat_flush(_format_context);
        std::cout << "av_seek_frame(): moved back near the beginning.\n";
    } else {
        std::cout << "av_seek_frame() is not supported for this input: "
                  << ffmpeg_test_coding::detail::getFfmpegErrorText(_result) << '\n';
    }
}

int inspectInput(const std::string& _input_path) {
    // 先单独展示 AVFormatContext 的申请和释放。
    AVFormatContext* _empty_context = avformat_alloc_context();
    if (_empty_context == nullptr) {
        std::cerr << "avformat_alloc_context() failed.\n";
        return AVERROR(ENOMEM);
    }
    avformat_free_context(_empty_context);

    AVFormatContext* _format_context = nullptr;
    //打开输入文件流
    int _result = avformat_open_input(&_format_context, _input_path.c_str(), nullptr, nullptr);
    if (_result < 0) {
        std::cerr << "Failed to open input: "
                  << ffmpeg_test_coding::detail::getFfmpegErrorText(_result) << '\n';
        return _result;
    }

    //打开探测流信息
    _result = avformat_find_stream_info(_format_context, nullptr);
    if (_result < 0) {
        std::cerr << "Failed to find stream information: "
                  << ffmpeg_test_coding::detail::getFfmpegErrorText(_result) << '\n';
		//关闭输入并释放上下文，不要再调用 avformat_free_context()。
        avformat_close_input(&_format_context);
        return _result;
    }

    // av_dump_format() 会打印容器、时长、码率、流和编码参数等概要信息。
    av_dump_format(_format_context, 0, _input_path.c_str(), 0);
    _result = readPackets(_format_context, 8);
    if (_result >= 0) {
        demonstrateSeeking(_format_context);
    }

    // avformat_close_input() 会关闭输入并释放上下文，不要再调用 avformat_free_context()。
    avformat_close_input(&_format_context);
    return _result;
}

} // namespace

namespace ffmpeg_test_coding {

int runDemuxingDemo(const std::string& _input_path) {
    if (_input_path.empty()) {
        std::cerr << "Please provide a media file path or URL.\n";
        return 1;
    }

    int _result = avformat_network_init();
    if (_result >= 0) {
        _result = inspectInput(_input_path);
        avformat_network_deinit();
    }

    return _result < 0 ? 1 : 0;
}

} // namespace ffmpeg_test_coding
