#include "ffmpeg_test_coding/basic_functions.hxx"
#include "ffmpeg_error.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
}

#include <iostream>
#include <string>

namespace {

constexpr int max_frame_count = 5;

struct DecoderState {
    AVMediaType m_media_type = AVMEDIA_TYPE_UNKNOWN;
    int m_stream_index = -1;
    int m_frame_count = 0;
    AVCodecContext* m_codec_context = nullptr;
    AVFrame* m_frame = nullptr;
};

void freeDecoder(DecoderState* const _state) {
    av_frame_free(&_state->m_frame);
    avcodec_free_context(&_state->m_codec_context);
    _state->m_stream_index = -1;
}

int openDecoder(AVFormatContext* const _format_context, const AVMediaType _media_type,
                DecoderState* const _state) {
	// 查找指定类型的流索引
    const int _stream_index = av_find_best_stream(
        _format_context, _media_type, -1, -1, nullptr, 0);
    if (_stream_index < 0) {
        return _stream_index;
    }

	// 查找对应流的解码器信息
    const AVCodecParameters* _codec_parameters =
        _format_context->streams[_stream_index]->codecpar;
    //根据id找解码器
    const AVCodec* _decoder_by_id = avcodec_find_decoder(_codec_parameters->codec_id);
    if (_decoder_by_id == nullptr) {
        return AVERROR_DECODER_NOT_FOUND;
    }

    // 同一个解码器也可以按名称查找。实际项目通常按 codec_id 查找即可。
    const AVCodec* _decoder_by_name = avcodec_find_decoder_by_name(_decoder_by_id->name);
    const AVCodec* _decoder = _decoder_by_name == nullptr ? _decoder_by_id : _decoder_by_name;
    AVCodecContext* _codec_context = avcodec_alloc_context3(_decoder);
    if (_codec_context == nullptr) {
        return AVERROR(ENOMEM);
    }

	//将编解码器参数复制到编解码器上下文中
    int _result = avcodec_parameters_to_context(_codec_context, _codec_parameters);
    if (_result >= 0) {
		// 打开解码器
        _result = avcodec_open2(_codec_context, _decoder, nullptr);
    }
    if (_result < 0) {
        avcodec_free_context(&_codec_context);
        return _result;
    }

	// 创建 AVFrame 用于存储解码后的帧数据
    AVFrame* _frame = av_frame_alloc();
    if (_frame == nullptr) {
        avcodec_free_context(&_codec_context);
        return AVERROR(ENOMEM);
    }

    _state->m_media_type = _media_type;
    _state->m_stream_index = _stream_index;
    _state->m_codec_context = _codec_context;
    _state->m_frame = _frame;

    const char* _type_name = av_get_media_type_string(_media_type);
    std::cout << "Opened " << (_type_name == nullptr ? "unknown" : _type_name)
              << " decoder: " << _decoder->name
              << ", stream=" << _stream_index << '\n';
    return 0;
}

void printDecodedFrame(const DecoderState& _state) {
    if (_state.m_media_type == AVMEDIA_TYPE_VIDEO) {
        std::cout << "video frame: index=" << _state.m_frame_count
                  << ", pts=" << _state.m_frame->pts
                  << ", size=" << _state.m_frame->width << 'x' << _state.m_frame->height
                  << ", format=" << _state.m_frame->format << '\n';
        return;
    }

    std::cout << "audio frame: index=" << _state.m_frame_count
              << ", pts=" << _state.m_frame->pts
              << ", samples=" << _state.m_frame->nb_samples
              << ", sample_rate=" << _state.m_frame->sample_rate
              << ", channels=" << _state.m_frame->ch_layout.nb_channels
              << ", format=" << _state.m_frame->format << '\n';
}

int receiveFrames(DecoderState* const _state) {
    while (_state->m_frame_count < max_frame_count) {
		// 把数据从_state->m_codec_context解码到_state->m_frame中
        const int _result = avcodec_receive_frame(_state->m_codec_context, _state->m_frame);
        if (_result == AVERROR(EAGAIN) || _result == AVERROR_EOF) {
            return 0;
        }
        if (_result < 0) {
            return _result;
        }

        ++_state->m_frame_count;
		// 打印解码后的帧信息
        printDecodedFrame(*_state);
		// 释放 AVFrame 内部的引用计数数据
        av_frame_unref(_state->m_frame);
    }

    return 0;
}

int decodePacket(const AVPacket& _packet, DecoderState* const _state) {
    if (_state->m_codec_context == nullptr ||
        _packet.stream_index != _state->m_stream_index ||
        _state->m_frame_count >= max_frame_count) {
        return 0;
    }

	//读取到的压缩数据包发送给解码器进行解码
    const int _result = avcodec_send_packet(_state->m_codec_context, &_packet);
    if (_result < 0) {
        return _result;
    }
    return receiveFrames(_state);
}

int flushDecoder(DecoderState* const _state) {
    if (_state->m_codec_context == nullptr ||
        _state->m_frame_count >= max_frame_count) {
        return 0;
    }

    const int _result = avcodec_send_packet(_state->m_codec_context, nullptr);
    if (_result < 0 && _result != AVERROR_EOF) {
        return _result;
    }
    return receiveFrames(_state);
}

bool decodingFinished(const DecoderState& _video_state, const DecoderState& _audio_state) {
    const bool _video_finished = _video_state.m_codec_context == nullptr ||
                                 _video_state.m_frame_count >= max_frame_count;
    const bool _audio_finished = _audio_state.m_codec_context == nullptr ||
                                 _audio_state.m_frame_count >= max_frame_count;
    return _video_finished && _audio_finished;
}

int decodeInput(AVFormatContext* const _format_context,
                DecoderState* const _video_state, DecoderState* const _audio_state) {
	//创建 AVPacket 用于存储读取的压缩数据包
    AVPacket* _packet = av_packet_alloc();
    if (_packet == nullptr) {
        return AVERROR(ENOMEM);
    }

    int _result = 0;
	// 循环读取压缩数据包并解码，直到解码完成或读取结束
    while (!decodingFinished(*_video_state, *_audio_state) &&
           (_result = av_read_frame(_format_context, _packet)) >= 0) {
        _result = decodePacket(*_packet, _video_state);
        if (_result >= 0) {
            _result = decodePacket(*_packet, _audio_state);
        }
        av_packet_unref(_packet);

        if (_result < 0) {
            break;
        }
    }

    av_packet_free(&_packet);
    if (_result != AVERROR_EOF) {
        return _result;
    }

    _result = flushDecoder(_video_state);
    if (_result >= 0) {
        _result = flushDecoder(_audio_state);
    }
    return _result;
}

int openInput(const std::string& _input_path, AVFormatContext** const _format_context) {
	// 打开输入文件或 URL，并读取流信息
    int _result = avformat_open_input(_format_context, _input_path.c_str(), nullptr, nullptr);
    if (_result >= 0) {
		// 读取流信息,填充 AVStream->codecpar
        _result = avformat_find_stream_info(*_format_context, nullptr);
    }
    return _result;
}

} // namespace

namespace ffmpeg_test_coding {

int runDecodingDemo(const std::string& _input_path) {
    if (_input_path.empty()) {
        std::cerr << "Please provide a media file path or URL.\n";
        return 1;
    }

    int _result = avformat_network_init();
    if (_result < 0) {
        std::cerr << "Network initialization failed: "
                  << detail::getFfmpegErrorText(_result) << '\n';
        return 1;
    }

    AVFormatContext* _format_context = nullptr;
	// 分别保存视频解码器和音频解码器的状态。
    DecoderState _video_state;
    DecoderState _audio_state;

    _result = openInput(_input_path, &_format_context);
    if (_result >= 0) {
        const int _video_result = openDecoder(
            _format_context, AVMEDIA_TYPE_VIDEO, &_video_state);
        const int _audio_result = openDecoder(
            _format_context, AVMEDIA_TYPE_AUDIO, &_audio_state);

        if (_video_result < 0 && _audio_result < 0) {
            _result = _video_result;
        } else {
            _result = decodeInput(_format_context, &_video_state, &_audio_state);
        }
    }

    if (_result < 0) {
        std::cerr << "Decoding failed: " << detail::getFfmpegErrorText(_result) << '\n';
    }

    freeDecoder(&_audio_state);
    freeDecoder(&_video_state);
    avformat_close_input(&_format_context);
    avformat_network_deinit();
    return _result < 0 ? 1 : 0;
}

} // namespace ffmpeg_test_coding
