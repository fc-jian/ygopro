#ifndef YGOPRO_SERVER_ARGS_H
#define YGOPRO_SERVER_ARGS_H

#include <array>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>

namespace ygo {

constexpr const char* CUBE_DECK_LIMITS_MARKER = "--cube-deck-limits";

struct ServerExtensionArgs {
	bool has_deck_limits{};
	std::array<int, 4> deck_limits{};
	int seed_start{13};
};

inline bool IsDecimalArgument(const char* value) {
	if(!value || !*value)
		return false;
	for(const unsigned char* current = reinterpret_cast<const unsigned char*>(value); *current; ++current) {
		if(*current < '0' || *current > '9')
			return false;
	}
	return true;
}

inline bool ParseDeckLimit(const char* value, int max_zone_size, int& parsed) {
	if(!IsDecimalArgument(value))
		return false;
	errno = 0;
	char* end{};
	const long result = std::strtol(value, &end, 10);
	if(errno == ERANGE || end == value || *end != '\0' || result < 0 || result > max_zone_size || result > INT_MAX)
		return false;
	parsed = static_cast<int>(result);
	return true;
}

// New callers use an explicit marker. The four-number layout remains accepted for
// compatibility with already deployed Cube srvpro versions.
inline bool ParseServerExtensionArgs(int argc, const char* const argv[], int max_zone_size,
	ServerExtensionArgs& result, std::string& error) {
	result = ServerExtensionArgs{};
	if(argc <= 13)
		return true;

	int limits_start = -1;
	if(std::strcmp(argv[13], CUBE_DECK_LIMITS_MARKER) == 0) {
		if(argc < 18) {
			error = "--cube-deck-limits requires main_min, main_max, extra_max and side_max";
			return false;
		}
		limits_start = 14;
	} else if(argc >= 17 && IsDecimalArgument(argv[13]) && IsDecimalArgument(argv[14])
		&& IsDecimalArgument(argv[15]) && IsDecimalArgument(argv[16])) {
		limits_start = 13;
	} else {
		return true;
	}

	for(size_t index = 0; index < result.deck_limits.size(); ++index) {
		if(!ParseDeckLimit(argv[limits_start + static_cast<int>(index)], max_zone_size, result.deck_limits[index])) {
			error = "Cube deck limits must be decimal integers within the supported zone size";
			return false;
		}
	}
	if(result.deck_limits[0] > result.deck_limits[1]) {
		error = "Cube main deck minimum cannot exceed its maximum";
		return false;
	}

	result.has_deck_limits = true;
	result.seed_start = limits_start + static_cast<int>(result.deck_limits.size());
	return true;
}

} // namespace ygo

#endif // YGOPRO_SERVER_ARGS_H
