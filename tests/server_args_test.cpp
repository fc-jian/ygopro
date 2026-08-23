#include "../gframe/server_args.h"

#include <cassert>
#include <string>
#include <vector>

namespace {

std::vector<const char*> Arguments(const std::vector<std::string>& values) {
	std::vector<const char*> result;
	result.reserve(values.size());
	for(const auto& value : values)
		result.push_back(value.c_str());
	return result;
}

std::vector<std::string> BaseArguments() {
	return { "ygopro", "0", "-1", "0", "0", "5", "T", "F", "8000", "5", "1", "999", "1" };
}

} // namespace

int main() {
	{
		auto values = BaseArguments();
		auto argv = Arguments(values);
		ygo::ServerExtensionArgs parsed;
		std::string error;
		assert(ygo::ParseServerExtensionArgs(static_cast<int>(argv.size()), argv.data(), 250, parsed, error));
		assert(!parsed.has_deck_limits);
		assert(parsed.seed_start == 13);
	}
	{
		auto values = BaseArguments();
		values.insert(values.end(), { "--cube-deck-limits", "40", "80", "30", "30", "seed" });
		auto argv = Arguments(values);
		ygo::ServerExtensionArgs parsed;
		std::string error;
		assert(ygo::ParseServerExtensionArgs(static_cast<int>(argv.size()), argv.data(), 250, parsed, error));
		assert(parsed.has_deck_limits);
		assert((parsed.deck_limits == std::array<int, 4>{ 40, 80, 30, 30 }));
		assert(parsed.seed_start == 18);
	}
	{
		auto values = BaseArguments();
		values.insert(values.end(), { "40", "80", "30", "30", "seed" });
		auto argv = Arguments(values);
		ygo::ServerExtensionArgs parsed;
		std::string error;
		assert(ygo::ParseServerExtensionArgs(static_cast<int>(argv.size()), argv.data(), 250, parsed, error));
		assert(parsed.has_deck_limits);
		assert(parsed.seed_start == 17);
	}
	for(const auto& limits : std::vector<std::vector<std::string>>{
		{ "--cube-deck-limits", "81", "80", "30", "30" },
		{ "--cube-deck-limits", "40", "251", "30", "30" },
		{ "--cube-deck-limits", "40", "80", "-1", "30" },
		{ "--cube-deck-limits", "40", "80", "30" },
	}) {
		auto values = BaseArguments();
		values.insert(values.end(), limits.begin(), limits.end());
		auto argv = Arguments(values);
		ygo::ServerExtensionArgs parsed;
		std::string error;
		assert(!ygo::ParseServerExtensionArgs(static_cast<int>(argv.size()), argv.data(), 250, parsed, error));
		assert(!error.empty());
	}
	return 0;
}
