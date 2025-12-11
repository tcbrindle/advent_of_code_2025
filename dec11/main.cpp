
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

static i64 count_paths(server_map_t const& servers, std::string const& from,
                       std::string const& to, cache_t& cache)
{
    if (from == to) {
        return 1;
    } else if (from == "out") {
        return 0;
    }

    if (auto iter = cache.find(from); iter != cache.cend()) {
        return iter->second;
    }

    i64 res = flux::ref(servers.at(from))
                  .map([&](std::string const& next) {
                      return count_paths(servers, next, to, cache);
                  })
                  .sum();
    cache[from] = res;
    return res;
}

auto const part1 = [](server_map_t const& servers) {
    cache_t cache{};
    return count_paths(servers, "you", "out", cache);
};

// For both the test data and my input, 'fft' always precedes 'dac'
auto const part2 = [](server_map_t const& servers) {
    cache_t cache1{}, cache2{}, cache3{};
    return count_paths(servers, "svr", "fft", cache1)
        * count_paths(servers, "fft", "dac", cache2)
        * count_paths(servers, "dac", "out", cache3);
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