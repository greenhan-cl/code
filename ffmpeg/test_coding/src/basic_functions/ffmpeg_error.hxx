#ifndef FFMPEG_TEST_CODING_FFMPEG_ERROR_HXX
#define FFMPEG_TEST_CODING_FFMPEG_ERROR_HXX

extern "C" {
#include <libavutil/error.h>
}

#include <string>

namespace ffmpeg_test_coding::detail {

inline std::string getFfmpegErrorText(const int _error_code) {
    char _buffer[AV_ERROR_MAX_STRING_SIZE] = {};
    av_strerror(_error_code, _buffer, sizeof(_buffer));
    return _buffer;
}

} // namespace ffmpeg_test_coding::detail

#endif // FFMPEG_TEST_CODING_FFMPEG_ERROR_HXX
