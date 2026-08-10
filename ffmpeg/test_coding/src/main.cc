#include "ffmpeg_test_coding/basic_functions.hxx"

#include <string>
#define INPUT_PATH "D:/Study/code/ffmpeg/videos/test_1080p_25fps.mp4"

int main(const int _argument_count, char* _arguments[]) {
    const std::string _input_path =
        _argument_count > 1 ? _arguments[1] : INPUT_PATH;
	return ffmpeg_test_coding::runDemuxingDemo(_input_path);
}
