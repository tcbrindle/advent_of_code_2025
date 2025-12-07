
import aoc;

using u64 = std::uint64_t;

auto const parse_input =
    [](std::string_view input) -> std::vector<std::string> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) { return std::string(line); })
        .to<std::vector>();
};

auto const part1 = [](std::vector<std::string> const& input) -> u64 {
    auto const& ops = flux::ref(input.back()).filter(flux::pred::in('+', '*'));

    auto totals =
        flux::slice(input, 0, input.size() - 1)
            .map([](std::string_view line) {
                return flux::split_string(line, ' ')
                    .filter([](std::string_view s) { return !s.empty(); })
                    .map(aoc::parse<u64>)
                    .to<std::vector>();
            })
            .fold_first([&ops](auto totals, auto vals) {
                flux::zip_for_each(
                    [](char op, u64& total, u64 val) {
                        total = op == '+' ? total + val : total * val;
                    },
                    ops, totals, vals);
                return totals;
            })
            .value();

    return flux::sum(totals);
};

auto const part2 = [](std::vector<std::string> const& input) -> u64 {
    auto const n_cols = input.at(0).size();
    auto const n_rows = input.size() - 1;

    return flux::ints(0, n_cols)
        .chunk_by([&ops = input.back()](flux::index_t, flux::index_t idx) {
            return ops.at(idx) != '+' && ops.at(idx) != '*';
        })
        .map([&input, n_rows](auto col_rng) {
            char op = input.back().at(col_rng.front().value());
            u64 id = op == '+' ? 0 : 1;
            auto sz = col_rng.size() - 1;
            return std::move(col_rng)
                .map([&input, n_rows, id](flux::index_t col_idx) {
                    auto seq =
                        flux::ints(0, n_rows).map([&, col_idx](auto row_idx) {
                            return input.at(row_idx).at(col_idx);
                        });
                    return aoc::try_parse<u64>(seq).value_or(id); // Hack
                })
                .fold(
                    [op](u64 acc, u64 val) -> u64 {
                        return op == '+' ? acc + val : acc * val;
                    },
                    id);
        })
        .sum();
};

constexpr auto& test_data = "123 328  51 64 \n"
                            " 45 64  387 23 \n"
                            "  6 98  215 314\n"
                            "*   +   *   +  ";

static_assert(part1(parse_input(test_data)) == 4277556);
static_assert(part2(parse_input(test_data)) == 3263827);

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