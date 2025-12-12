
import aoc;

struct grid_t {
    int area;
    std::vector<int> shape_counts;
};

auto const parse_input = [](std::string_view input) {
    std::array<int, 6> shape_sizes;
    for (int i : flux::ints(0, 6)) {
        input.remove_prefix(input.find_first_of(".#"));
        shape_sizes[i]
            = flux::filter(input, flux::pred::neq('\n')).take(9).count_eq('#');
        input.remove_prefix(input.find("\n\n") + 2);
    }

    auto grids
        = flux::split_string(input, '\n')
              .filter([](std::string_view line) { return !line.empty(); })
              .map([](std::string_view line) {
                  auto x = line.find('x');
                  auto colon = line.find(':');
                  return grid_t{
                      .area = aoc::parse<int>(line.substr(0, x))
                          * aoc::parse<int>(line.substr(x + 1, colon)),
                      .shape_counts
                      = flux::split_string(line.substr(colon + 2), ' ')
                            .map(aoc::parse<int>)
                            .to<std::vector>()};
              })
              .to<std::vector>();

    return std::pair(shape_sizes, std::move(grids));
};

auto const solve = [](std::array<int, 6> const& shape_sizes,
                      std::vector<grid_t> const& grids) -> std::optional<int> {
    auto trivially_solvable = flux::count_if(grids, [](grid_t const& grid) {
        return grid.area >= 9 * flux::sum(grid.shape_counts);
    });

    auto impossible = flux::count_if(grids, [&](grid_t const& grid) {
        auto min_reqd = flux::map(flux::ints(0, 6), [&](int idx) {
                            return grid.shape_counts[idx] * shape_sizes[idx];
                        }).sum();
        return grid.area < min_reqd;
    });

    if (trivially_solvable + impossible == grids.size()) {
        return trivially_solvable;
    } else {
        return std::nullopt;
    }
};

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const [sizes, grids] = parse_input(aoc::string_from_file(argv[1]));

    if (auto soln = solve(sizes, grids)) {
        std::println("Solution: {}", *soln);
    } else {
        std::println("Could not find a simple solution");
    }
}