#ifndef OPT_UTIL_H__
#define OPT_UTIL_H__

#include <string>
#include <unordered_map>

namespace comm {

/**
 * Parses command-line options.
 */
class OptUtil {
public:
	// Each option must use the "-X" format and have one argument.
	static bool parse_args(int argc, char** argv,
		std::unordered_map<char, std::string>& opt_arg_map);
};

}

#endif

