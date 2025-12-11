
#include <flux/macros.hpp>

import aoc;

using i64 = std::int64_t;
using server_map_t = aoc::hash_map<std::string, std::vector<std::string>>;
using cache_t = aoc::hash_map<std::string, i64>;

auto const parse_input = [](std::string_view input) -> server_map_t {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            auto colon = line.find(':');
            return std::pair{std::string(line.substr(0, colon)),
                             flux::split_string(line.substr(colon + 2), ' ')
                                 .to<std::vector<std::string>>()};
        })
        .to<server_map_t>();
};

static int count_paths_p1(server_map_t const& servers, std::string const& from)
{
    if (from == "out") {
        return 1;
    } else {
        return flux::ref(servers.at(from))
            .map([&](std::string const& dest) {
                return count_paths_p1(servers, dest);
            })
            .sum();
    }
}

auto const part1 = [](server_map_t const& servers) {
    return count_paths_p1(servers, "you");
};

static i64 count_paths_p2(server_map_t const& servers, std::string const& from,
                          bool has_dac, bool has_fft, cache_t& cache)
{
    if (from == "out") {
        return has_dac && has_fft;
    } else if (from == "dac") {
        has_dac = true;
    } else if (from == "fft") {
        has_fft = true;
    }

    auto const state = std::string(from + char(has_dac) + char(has_fft));
    if (auto iter = cache.find(state); iter != cache.cend()) {
        return iter->second;
    }

    i64 res
        = flux::ref(servers.at(from))
              .map([&](std::string const& dest) {
                  return count_paths_p2(servers, dest, has_dac, has_fft, cache);
              })
              .sum();
    cache[state] = res;
    return res;
}

auto const part2 = [](server_map_t const& servers) {
    cache_t cache{};
    return count_paths_p2(servers, "svr", false, false, cache);
};

constexpr auto& test_data1 = R"(aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
)";

constexpr auto& test_data2 = R"(svr: aaa bbb
aaa: fft
fft: ccc
bbb: tty
tty: ccc
ccc: ddd eee
ddd: hub
hub: fff
eee: dac
dac: fff
fff: ggg hhh
ggg: out
hhh: out
)";

int main(int argc, char** argv)
{
    // Alas, no constexpr tests today
    FLUX_ASSERT(part1(parse_input(test_data1)) == 5);
    FLUX_ASSERT(part2(parse_input(test_data2)) == 2);

    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const input = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(input));
    auto [res, time] = aoc::timed(part2, input);
    std::println("Part 2: {} ({})", res, time);
}