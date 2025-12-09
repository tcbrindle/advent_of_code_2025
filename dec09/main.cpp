
import aoc;
import ctre;

using i64 = std::int64_t;
using point = aoc::vec2_t<i64>;
using line = std::pair<point, point>;

auto const parse_input = [](std::string_view input) -> std::vector<point> {
    return flux::map(ctre::search_all<"(\\d+),(\\d+)">(input),
                     [](auto match) {
                         auto [_, x, y] = match;
                         return point{x.to_number(), y.to_number()};
                     })
        .to<std::vector>();
};

auto const area = [](point const& a, point const& b) -> i64 {
    return (1 + aoc::abs(a.x - b.x)) * (1 + aoc::abs(a.y - b.y));
};

auto const part1 = [](std::vector<point> const& tiles) {
    return flux::cartesian_power_map<2>(flux::ref(tiles), area).max().value();
};

auto const intersects = [](line const& l1, line const& l2) -> bool {
    auto [minx1, maxx1] = std::minmax(l1.first.x, l1.second.x);
    auto [miny1, maxy1] = std::minmax(l1.first.y, l1.second.y);

    auto [minx2, maxx2] = std::minmax(l2.first.x, l2.second.x);
    auto [miny2, maxy2] = std::minmax(l2.first.y, l2.second.y);

    return maxx2 > minx1 && minx2 < maxx1 && maxy2 > miny1 && miny2 < maxy1;
};

auto const check_intersections = [](point const& tl, point const& br,
                                    std::vector<point> const& tiles) -> bool {
    point tr{br.x, tl.y};
    point bl{tl.x, br.y};
    auto edges =
        std::array<line, 4>{line{tl, tr}, {tr, br}, {br, bl}, {bl, tl}};
    auto polygon = flux::ref(tiles).cycle(2).take(tiles.size() + 1).pairwise();

    return flux::cartesian_product(std::move(polygon), std::move(edges))
        .none(flux::unpack(intersects));
};

auto const part2 = [](std::vector<point> const& tiles) {
    return flux::cartesian_power<2>(flux::ref(tiles))
        .filter(flux::unpack([&](point from, point to) {
            if (from.x == to.x || from.y == to.y) { return false; }
            point tl = {std::min(from.x, to.x) + 1, std::min(from.y, to.y) + 1};
            point br = {std::max(from.x, to.x) - 1, std::max(from.y, to.y) - 1};
            return check_intersections(tl, br, tiles);
        }))
        .map(flux::unpack(area))
        .max()
        .value_or(0);
};

constexpr auto& test_data = R"(7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3
)";

static_assert(part1(parse_input(test_data)) == 50);
static_assert(part2(parse_input(test_data)) == 24);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const tiles = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(tiles));
    std::println("Part 2: {}", part2(tiles));
}