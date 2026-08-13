#include "ffmpeg_test_coding/basic_functions.hxx"
#include "ffmpeg_error.hxx"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <SDL2/SDL.h>

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace {

struct VideoPlayerState {
    AVFormatContext* m_format_context = nullptr;
    AVCodecContext* m_codec_context = nullptr;
    AVFrame* m_frame = nullptr;
    AVPacket* m_packet = nullptr;
    AVStream* m_video_stream = nullptr;
    int m_video_stream_index = -1;

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture = nullptr;

    int64_t m_first_pts = AV_NOPTS_VALUE;
    std::uint64_t m_start_ticks = 0;
    bool m_should_quit = false;
};

void freeVideoPlayer(VideoPlayerState* const _state) {
    SDL_DestroyTexture(_state->m_texture);
    SDL_DestroyRenderer(_state->m_renderer);
    SDL_DestroyWindow(_state->m_window);
    SDL_Quit();

    av_packet_free(&_state->m_packet);
    av_frame_free(&_state->m_frame);
    avcodec_free_context(&_state->m_codec_context);
    avformat_close_input(&_state->m_format_context);
}

//打开编解码器
int openVideoDecoder(const std::string& _input_path, VideoPlayerState* const _state) {
    //打开媒体文件并创建格式上下文
    int _result = avformat_open_input(&_state->m_format_context, _input_path.c_str(), nullptr, nullptr);
    if (_result < 0) {
        return _result;
    }

    //查找格式上下午的媒体流信息
    _result = avformat_find_stream_info(_state->m_format_context, nullptr);
    if (_result < 0) {
        return _result;
    }

    //查找最适合的媒体流信息的索引
    _state->m_video_stream_index = av_find_best_stream(
        _state->m_format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (_state->m_video_stream_index < 0) {
        return _state->m_video_stream_index;
    }

    //根据之前找到的视频流索引，获取对应的 AVStream 指针
    _state->m_video_stream = _state->m_format_context->streams[_state->m_video_stream_index];
    //获取对应媒体流的一个编解码参数
    const AVCodecParameters* _codec_parameters = _state->m_video_stream->codecpar;
    //根据媒体流中编码器id查找对应的一个编码器（用什么格式的解码器）
    const AVCodec* _decoder = avcodec_find_decoder(_codec_parameters->codec_id);
    if (_decoder == nullptr) {
        return AVERROR_DECODER_NOT_FOUND;
    }

    //创建编码上下文信息
    _state->m_codec_context = avcodec_alloc_context3(_decoder);
    if (_state->m_codec_context == nullptr) {
        return AVERROR(ENOMEM);
    }

    //为编码器拷贝解码信息（详细解码信息，如视频品质等）
    _result = avcodec_parameters_to_context(_state->m_codec_context, _codec_parameters);
    if (_result >= 0) {
        _result = avcodec_open2(_state->m_codec_context, _decoder, nullptr);
    }
    if (_result < 0) {
        return _result;
    }

    //创建帧和包信息
    _state->m_frame = av_frame_alloc();
    _state->m_packet = av_packet_alloc();
    if (_state->m_frame == nullptr || _state->m_packet == nullptr) {
        return AVERROR(ENOMEM);
    }

    std::cout << "Video decoder: " << _decoder->name << '\n';
    std::cout << "Video size: " << _state->m_codec_context->width << 'x'
              << _state->m_codec_context->height << '\n';
    return 0;
}

int openSdlVideoOutput(VideoPlayerState* const _state) {
    if (_state->m_codec_context->pix_fmt != AV_PIX_FMT_YUV420P) {
        std::cerr << "This first SDL demo only accepts YUV420P video. Actual pixel format: "
                  << av_get_pix_fmt_name(_state->m_codec_context->pix_fmt) << '\n';
        return AVERROR(EINVAL);
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return AVERROR_EXTERNAL;
    }

    const int _window_width = std::min(_state->m_codec_context->width, 1280);
    const int _window_height = std::min(_state->m_codec_context->height, 720);
    _state->m_window = SDL_CreateWindow(
        "FFmpeg + SDL2 video demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _window_width,
        _window_height,
        SDL_WINDOW_RESIZABLE);
    if (_state->m_window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        return AVERROR_EXTERNAL;
    }

    _state->m_renderer = SDL_CreateRenderer(
        _state->m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_state->m_renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        return AVERROR_EXTERNAL;
    }

    // SDL_PIXELFORMAT_IYUV 与 FFmpeg 的 AV_PIX_FMT_YUV420P 都是 Y、U、V 三个平面。
    _state->m_texture = SDL_CreateTexture(
        _state->m_renderer,
        SDL_PIXELFORMAT_IYUV,
        SDL_TEXTUREACCESS_STREAMING,
        _state->m_codec_context->width,
        _state->m_codec_context->height);
    if (_state->m_texture == nullptr) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << '\n';
        return AVERROR_EXTERNAL;
    }

    _state->m_start_ticks = SDL_GetTicks64();
    return 0;
}

void processSdlEvents(VideoPlayerState* const _state) {
    SDL_Event _event;
    while (SDL_PollEvent(&_event) != 0) {
        if (_event.type == SDL_QUIT ||
            (_event.type == SDL_KEYDOWN && _event.key.keysym.sym == SDLK_ESCAPE)) {
            _state->m_should_quit = true;
        }
    }
}

void waitForPresentationTime(VideoPlayerState* const _state, const AVFrame* const _frame) {
    const int64_t _pts = _frame->best_effort_timestamp;
    if (_pts == AV_NOPTS_VALUE) {
        return;
    }

    if (_state->m_first_pts == AV_NOPTS_VALUE) {
        _state->m_first_pts = _pts;
        _state->m_start_ticks = SDL_GetTicks64();
        return;
    }

    const int64_t _relative_pts = _pts - _state->m_first_pts;
    const int64_t _target_elapsed_ms = av_rescale_q(
        _relative_pts, _state->m_video_stream->time_base, AVRational{1, 1000});
    while (!_state->m_should_quit) {
        processSdlEvents(_state);
        const std::int64_t _elapsed_ms = static_cast<std::int64_t>(SDL_GetTicks64() - _state->m_start_ticks);
        const std::int64_t _remaining_ms = _target_elapsed_ms - _elapsed_ms;
        if (_remaining_ms <= 0) {
            return;
        }
        SDL_Delay(static_cast<std::uint32_t>(std::min<std::int64_t>(_remaining_ms, 10)));
    }
}

int displayFrame(VideoPlayerState* const _state) {
    waitForPresentationTime(_state, _state->m_frame);
    if (_state->m_should_quit) {
        return 0;
    }

    // data[0..2] 分别是 Y、U、V 平面；linesize[0..2] 是每一行实际跨越的字节数。
    const int _result = SDL_UpdateYUVTexture(
        _state->m_texture,
        nullptr,
        _state->m_frame->data[0], _state->m_frame->linesize[0],
        _state->m_frame->data[1], _state->m_frame->linesize[1],
        _state->m_frame->data[2], _state->m_frame->linesize[2]);
    if (_result != 0) {
        std::cerr << "SDL_UpdateYUVTexture failed: " << SDL_GetError() << '\n';
        return AVERROR_EXTERNAL;
    }

    SDL_RenderClear(_state->m_renderer);
    SDL_RenderCopy(_state->m_renderer, _state->m_texture, nullptr, nullptr);
    SDL_RenderPresent(_state->m_renderer);
    return 0;
}

int receiveAndDisplayFrames(VideoPlayerState* const _state) {
    while (!_state->m_should_quit) {
        const int _result = avcodec_receive_frame(_state->m_codec_context, _state->m_frame);
        if (_result == AVERROR(EAGAIN) || _result == AVERROR_EOF) {
            return 0;
        }
        if (_result < 0) {
            return _result;
        }

        const int _display_result = displayFrame(_state);
        av_frame_unref(_state->m_frame);
        if (_display_result < 0) {
            return _display_result;
        }
    }
    return 0;
}

int decodeAndDisplay(VideoPlayerState* const _state) {
    int _result = 0;
    while (!_state->m_should_quit &&
           (_result = av_read_frame(_state->m_format_context, _state->m_packet)) >= 0) {
        if (_state->m_packet->stream_index == _state->m_video_stream_index) {
            _result = avcodec_send_packet(_state->m_codec_context, _state->m_packet);
            if (_result >= 0) {
                _result = receiveAndDisplayFrames(_state);
            }
        }
        av_packet_unref(_state->m_packet);

        if (_result < 0) {
            return _result;
        }
        processSdlEvents(_state);
    }

    if (_result != AVERROR_EOF || _state->m_should_quit) {
        return _result == AVERROR_EOF ? 0 : _result;
    }

    _result = avcodec_send_packet(_state->m_codec_context, nullptr);
    if (_result < 0 && _result != AVERROR_EOF) {
        return _result;
    }
    return receiveAndDisplayFrames(_state);
}

} // namespace

namespace ffmpeg_test_coding {

int runSdlVideoDemo(const std::string& _input_path) {
    VideoPlayerState _state;
    int _result = openVideoDecoder(_input_path, &_state);
    if (_result >= 0) {
        _result = openSdlVideoOutput(&_state);
    }
    if (_result >= 0) {
        _result = decodeAndDisplay(&_state);
    }

    if (_result < 0) {
        std::cerr << "SDL video demo failed: " << detail::getFfmpegErrorText(_result) << '\n';
    }
    freeVideoPlayer(&_state);
    return _result < 0 ? 1 : 0;
}

} // namespace ffmpeg_test_coding
