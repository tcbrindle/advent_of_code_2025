
import aoc;

using u64 = std::uint64_t;

// flux::find_max returns the *last* position if several are equally
// maximal, but for this problem we need the *first* position
auto const find_first_max =
    std::bind_back(flux::find_min, flux::cmp::reverse_compare);

template <std::size_t N>
auto const calculate_joltage = [](std::string_view line) -> u64 {
    std::array<std::size_t, N> indices;
    std::size_t window_size = line.size() - N + 1;
    indices[0] = find_first_max(flux::slice(line, 0, window_size));
    u64 jolts = line[indices[0]] - '0';
    for (std::size_t i = 1; i < N; i++) {
        indices[i] = find_first_max(
            flux::slice(line, indices[i - 1] + 1, window_size + i));
        jolts = 10 * jolts + (line[indices[i]] - '0');
    }
    return jolts;
};

template <std::size_t N>
auto const solve = [](std::string_view input) {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map(calculate_joltage<N>)
        .sum();
};

auto const part1 = solve<2>;
auto const part2 = solve<12>;

constexpr auto& test_data =
    R"(987654321111111
811111111111119
234234234234278
818181911112111
)";

static_assert(part1(test_data) == 357);
static_assert(part2(test_data) == 3121910778619);

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