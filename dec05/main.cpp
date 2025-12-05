
import aoc;

using u64 = std::uint64_t;

struct id_range {
    u64 lo;
    u64 hi;

    friend auto operator<=>(id_range, id_range) = default;
};

auto const parse_ranges = [](std::string_view input) -> std::vector<id_range> {
    return flux::split_string(input, '\n')
        .map([](std::string_view line) {
            auto dash = line.find('-');
            return id_range{aoc::parse<u64>(line.substr(0, dash)),
                            aoc::parse<u64>(line.substr(dash + 1))};
        })
        .to<std::vector>();
};

auto const parse_ids = [](std::string_view input) -> std::vector<u64> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map(aoc::parse<u64>)
        .to<std::vector>();
};

auto const parse_input = [](std::string_view input)
    -> std::pair<std::vector<id_range>, std::vector<u64>> {
    auto blank = input.find("\n\n");
    return {parse_ranges(input.substr(0, blank)),
            parse_ids(input.substr(blank + 2))};
};

auto const part1 = [](std::vector<id_range> const& ranges,
                      std::vector<u64> const& ids) {
    return flux::count_if(ids, [&](u64 id) {
        return flux::any(ranges, [id](id_range const& rng) {
            return id >= rng.lo && id <= rng.hi;
        });
    });
};

auto const disjoint = [](id_range const& lhs, id_range const& rhs) -> bool {
    return (lhs.hi < rhs.lo) || (rhs.hi < lhs.lo);
};

// Algorithm: repeatedly merge ranges until we can't do so any more,
// then total up the number of ids covered
auto const part2 = [](std::vector<id_range> ranges) -> u64 {
    // Pre-sort the ranges to increase the chances of finding an early overlap
    flux::sort(ranges);

    std::vector<id_range> next;
    next.reserve(ranges.size());

    while (true) {
        for (id_range rng : ranges) {
            auto overlap = flux::find_element_if(next, [&](id_range test_rng) {
                return !disjoint(rng, test_rng);
            });
            if (overlap.has_value()) {
                *overlap = {.lo = std::min(rng.lo, overlap->lo),
                            .hi = std::max(rng.hi, overlap->hi)};
            } else {
                next.push_back(rng);
            }
        }

        if (ranges.size() == next.size()) { break; }
        std::swap(ranges, next);
        next.clear();
    }

    return flux::ref(ranges)
        .map([](id_range rng) { return 1 + rng.hi - rng.lo; })
        .sum();
};

constexpr auto& test_data =
    R"(3-5
10-14
16-20
12-18

1
5
8
11
17
32
)";

auto const test = [] -> bool {
    auto const [rngs, ids] = parse_input(test_data);
    return part1(rngs, ids) == 3 && part2(rngs) == 14;
};
static_assert(test());

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const [rngs, ids] = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(rngs, ids));
    auto [soln, time] = aoc::timed(part2, rngs);
    std::println("Part 2: {} ({})", soln, time);
}