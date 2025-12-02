
import aoc;
import ctre;

namespace {

using u64 = std::uint64_t;

// constexpr replacement for std::to_string
auto const to_string = [](u64 val) -> std::string {
    std::string out;
    out.push_back('0' + (val % 10));
    while (true) {
        val /= 10;
        if (val == 0) { break; }
        out.push_back('0' + (val % 10));
    }
    return out;
};

template <ctll::fixed_string Regex>
auto const test_ranges = [](std::string_view input) -> u64 {
    return flux::map(ctre::search_all<"(\\d+)-(\\d+)">(input),
                     [](auto match) {
                         auto [_, lo, hi] = match;
                         return flux::iota(aoc::parse<u64>(lo),
                                           1 + aoc::parse<u64>(hi));
                     })
        .flatten()
        .filter([](u64 value) { return ctre::match<Regex>(to_string(value)); })
        .sum();
};

auto const part1 = test_ranges<"(\\d+)\\1">;
auto const part2 = test_ranges<"(\\d+)\\1+">;

constexpr std::string_view test_data =
    R"(11-22,95-115,998-1012,1188511880-1188511890,222220-222224,
1698522-1698528,446443-446449,38593856-38593862,565653-565659,
824824821-824824827,2121212118-2121212124
)";

static_assert(part1(test_data) == 1227775554);
static_assert(part2(test_data) == 4174379265);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    std::string const input = aoc::string_from_file(argv[1]);

    std::println("Part 1: {}", part1(input));
    std::println("Part 2: {}", part2(input));
}