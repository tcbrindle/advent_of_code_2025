
import aoc;

using coord = aoc::vec2_t<int>;

struct grid2d {
    std::string data;
    int width;
    int height;

    constexpr auto operator[](coord c) const -> char
    {
        if (c.x < 0 || c.x >= width || c.y < 0 || c.y >= height) {
            return '.';
        } else {
            return data[c.y * width + c.x];
        }
    }

    constexpr auto operator[](coord c) -> char&
    {
        if (c.x < 0 || c.x >= width || c.y < 0 || c.y >= height) {
            __builtin_trap();
        } else {
            return data[c.y * width + c.x];
        }
    }

    constexpr auto coordinates() const -> flux::multipass_sequence auto
    {
        return flux::cartesian_product_map(
            [](int x, int y) { return coord{x, y}; }, flux::iota(0, width),
            flux::iota(0, height));
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::in('.', '@')).to<std::string>(),
        .width = int(flux::find(input, '\n')),
        .height = int(flux::count_eq(input, '\n'))};
};

auto const can_access = [](grid2d const& grid, coord pos) -> bool {
    constexpr std::array<coord, 8> neighbours = {
        coord{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
        {1, 0},        {-1, 1}, {0, 1},  {1, 1}};

    return flux::map(neighbours, [&](coord n) {
               return grid[pos + n];
           }).count_eq('@') < 4;
};

auto const part1 = [](grid2d const& grid) {
    return grid.coordinates()
        .filter([&](coord pos) { return grid[pos] == '@'; })
        .count_if([&](coord pos) { return can_access(grid, pos); });
};

auto const part2 = [](grid2d grid) {
    int total_removed = 0;

    while (true) {
        int removed = 0;
        grid.coordinates()
            .filter([&](coord pos) {
                return grid[pos] == '@' && can_access(grid, pos);
            })
            .for_each([&](coord pos) {
                grid[pos] = '.';
                ++removed;
            });

        total_removed += removed;
        if (removed == 0) { return total_removed; }
    }
};

constexpr auto& test_data =
    R"(..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.
)";

static_assert(part1(parse_input(test_data)) == 13);
static_assert(part2(parse_input(test_data)) == 43);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    grid2d const grid = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(grid));
    std::println("Part 2: {}", part2(grid));
}