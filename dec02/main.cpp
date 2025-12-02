
import aoc;
import ctre;

namespace {

using u8 = std::uint8_t;
using u64 = std::uint64_t;

struct id_range {
    u64 from;
    u64 to;
};

auto const parse_input = [](std::string_view input) -> std::vector<id_range> {
    return flux::map(ctre::search_all<"(\\d+)-(\\d+)">(input),
                     [](auto match) -> id_range {
                         auto [_, from, to] = match;
                         return {aoc::parse<u64>(from), aoc::parse<u64>(to)};
                     })
        .to<std::vector>();
};

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

auto const is_invalid_id_p1 = [](u64 id) -> bool {
    auto const str = to_string(id);
    if (str.size() % 2 != 0) { return false; }
    auto mid = str.size() / 2;
    return flux::equal(flux::ref(str).take(mid), flux::ref(str).drop(mid));
};

auto const is_invalid_id_p2 = [](u64 id) -> bool {
    auto const str = to_string(id);

    return flux::iota(1uz, 1 + str.size() / 2)
        .filter([&](auto sz) { return str.size() % sz == 0; })
        .map([&](auto sz) { return flux::ref(str).chunk(sz); })
        .any([&](flux::multipass_sequence auto chunks) {
            auto first = chunks.front().value();
            return std::move(chunks).drop(1).all(
                [&](auto chunk) { return flux::equal(chunk, first); });
        });
};

template <auto TestFn>
auto const test_ranges = [](std::vector<id_range> const& input) -> u64 {
    return flux::ref(input)
        .map([](id_range rng) { return flux::iota(rng.from, rng.to + 1); })
        .flatten()
        .filter(TestFn)
        .sum();
};

auto const part1 = test_ranges<is_invalid_id_p1>;
auto const part2 = test_ranges<is_invalid_id_p2>;

constexpr std::string_view test_data =
    R"(11-22,95-115,998-1012,1188511880-1188511890,222220-222224,
1698522-1698528,446443-446449,38593856-38593862,565653-565659,
824824821-824824827,2121212118-2121212124
)";

static_assert(part1(parse_input(test_data)) == 1227775554);
static_assert(part2(parse_input(test_data)) == 4174379265);

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