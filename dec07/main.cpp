
import aoc;

using u64 = std::uint64_t;

struct grid2d {
    std::string data;
    int width;
    int height;

    constexpr auto operator[](int i, int j) const -> char
    {
        return data[j * width + i];
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return {.data =
                flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
            .width = (int) flux::find(input, '\n'),
            .height = (int) flux::count_eq(input, '\n')};
};

auto const solve = [](grid2d const& grid) -> std::pair<int, u64> {
    std::flat_map<int, u64> beam_counts, next_beam_counts;
    beam_counts[flux::find(grid.data, 'S')] = 1;
    int split_count = 0;

    for (int row : flux::iota(1, grid.height)) {
        for (auto [col, count] : beam_counts) {
            if (grid[col, row] == '^') {
                next_beam_counts[col - 1] += count;
                next_beam_counts[col + 1] += count;
                ++split_count;
            } else {
                next_beam_counts[col] += count;
            }
        }
        std::swap(beam_counts, next_beam_counts);
        next_beam_counts.clear();
    }

    return {split_count, flux::sum(beam_counts.values())};
};

constexpr auto& test_data = R"(.......S.......
...............
.......^.......
...............
......^.^......
...............
.....^.^.^.....
...............
....^.^...^....
...............
...^.^...^.^...
...............
..^...^.....^..
...............
.^.^.^.^.^...^.
...............
)";

static_assert([] {
    auto [p1, p2] = solve(parse_input(test_data));
    return p1 == 21 && p2 == 40;
}());

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));
    auto [result, time] = aoc::timed(solve, grid);
    std::println("Part 1: {}", result.first);
    std::println("Part 2: {}", result.second);
    std::println("Time: {}", time);
}