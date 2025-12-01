
import aoc;

namespace {

constexpr auto parse_input = [](std::string_view input) {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            int dist = aoc::parse<int>(line.substr(1));
            return line.at(0) == 'L' ? -dist : dist;
        })
        .to<std::vector>();
};

constexpr auto part1 = [](std::vector<int> const& offsets) {
    return flux::ref(offsets).scan(std::plus{}, 50).count_if([](int i) {
        return i % 100 == 0;
    });
};

constexpr auto part2 = [](std::vector<int> const& offsets) {
    int zero_count = 0;
    int pos = 50;

    for (int offset : offsets) {
        if (offset >= 0) {
            zero_count += (pos + offset) / 100;
        } else {
            zero_count -= offset / 100;
            offset %= 100;
            zero_count += pos != 0 && (pos + offset <= 0);
        }
        pos = (pos + offset + 100) % 100;
    }

    return zero_count;
};

constexpr std::string_view test_input =
    R"(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82
)";

static_assert(part1(parse_input(test_input)) == 3);
static_assert(part2(parse_input(test_input)) == 6);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const input = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(input));
    std::println("Part 2: {}", part2(input));
}