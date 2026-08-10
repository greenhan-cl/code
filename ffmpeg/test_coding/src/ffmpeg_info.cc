#include "ffmpeg_test_coding/ffmpeg_info.hxx"

extern "C" {
#include <libavutil/avutil.h>
}

namespace ffmpeg_test_coding {

std::string getFfmpegVersion() {
    return av_version_info();
}

} // namespace ffmpeg_test_coding
