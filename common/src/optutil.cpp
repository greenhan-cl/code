#include "optutil.h"

namespace comm {

bool OptUtil::parse_args(int argc, char** argv,
	std::unordered_map<char, std::string>& opt_arg_map) {
	if (argc < 2 || argv == nullptr) {
		return false;
	}

	for (int _i = 0; _i < argc; _i += 2) {
		std::string _opt = argv[_i];

		if (_opt.length() != 2 || _opt[0] != '-') {
			return false;
		}

		char _c_opt = _opt[1];

		if (_i + 1 >= argc) {
			return false;
		}

		std::string _arg = argv[_i + 1];
		opt_arg_map[_c_opt] = _arg;
	}

	return true;
}

}
