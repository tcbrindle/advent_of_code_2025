
import aoc;

using u32 = std::uint32_t;
using i64 = std::int64_t;

struct vec3 {
    int x = 0, y = 0, z = 0;
};

auto const dist2 = [](vec3 const& a, vec3 const& b) {
    i64 dx = a.x - b.x;
    i64 dy = a.y - b.y;
    i64 dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
};

auto const parse_input = [](std::string_view input) -> std::vector<vec3> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            auto [x, y, z] = flux::split_string(line, ',')
                                 .map(aoc::parse<int>)
                                 .adjacent<3>()
                                 .front()
                                 .value();
            return vec3{x, y, z};
        })
        .to<std::vector>();
};

template <int N>
auto const part1 = [](std::vector<vec3> const& boxes) {
    // Create a list of pairs of box ids
    std::vector<std::pair<u32, u32>> pairs;
    for (auto i : flux::iota(0uz, boxes.size())) {
        for (auto j : flux::iota(i + 1, boxes.size())) {
            pairs.emplace_back(i, j);
        }
    }

    // Turn it into a min heap
    auto proj = [&](auto const& p) {
        return dist2(boxes[p.first], boxes[p.second]);
    };

    std::ranges::make_heap(pairs, std::greater{}, proj);

    // Create a list of circuits, where each circuit is a list of box ids
    // Initially each box is in its own circuit
    auto circuits = flux::iota(0uz, boxes.size())
                        .map([](u32 i) { return std::vector{i}; })
                        .template to<std::vector>();

    // Create a mapping from box id to circuit id
    auto lights_to_circuits =
        flux::to<std::vector<u32>>(flux::iota(0uz, boxes.size()));

    for (auto _ : flux::ints(0, N)) {
        auto [box_id1, box_id2] = pairs.front();
        auto circuit_id1 = lights_to_circuits[box_id1];
        auto circuit_id2 = lights_to_circuits[box_id2];

        auto& circuit1 = circuits[circuit_id1];
        auto& circuit2 = circuits[circuit_id2];

        circuit1 = flux::set_union(std::move(circuit1), std::move(circuit2))
                       .template to<std::vector>();

        for (auto id : circuit1) { lights_to_circuits[id] = circuit_id1; }

        std::ranges::pop_heap(pairs, std::greater{}, proj);
        pairs.pop_back();
    }

    std::ranges::partial_sort(circuits, circuits.begin() + 3,
                              std::ranges::greater{}, flux::size);

    return flux::ref(circuits).map(flux::size).take(3).product();
};

auto const part2 = [](std::vector<vec3> const& boxes) -> i64 {
    std::vector<std::pair<u32, u32>> pairs;
    for (auto i : flux::iota(0uz, boxes.size())) {
        for (auto j : flux::iota(i + 1, boxes.size())) {
            pairs.emplace_back(i, j);
        }
    }

    auto proj = [&](auto const& p) {
        return dist2(boxes[p.first], boxes[p.second]);
    };

    std::ranges::make_heap(pairs, std::greater{}, proj);

    auto circuits = flux::iota(0uz, boxes.size())
                        .map([](u32 i) { return std::vector{i}; })
                        .template to<std::vector>();

    auto lights_to_circuits =
        flux::to<std::vector<u32>>(flux::iota(0uz, boxes.size()));

    while (true) {
        auto [box_id1, box_id2] = pairs.front();
        auto circuit_id1 = lights_to_circuits[box_id1];
        auto circuit_id2 = lights_to_circuits[box_id2];

        auto& circuit1 = circuits[circuit_id1];
        auto& circuit2 = circuits[circuit_id2];

        std::vector<u32> new_circuit;
        new_circuit.reserve(circuit1.size() + circuit2.size());
        std::ranges::set_union(circuit1, circuit2,
                               std::back_inserter(new_circuit));
        circuit1 = std::move(new_circuit);

        if (circuit1.size() == boxes.size()) {
            return i64(boxes[box_id1].x) * i64(boxes[box_id2].x);
        }

        for (auto id : circuit1) { lights_to_circuits[id] = circuit_id1; }

        std::ranges::pop_heap(pairs, std::greater{}, proj);
        pairs.pop_back();
    }
};

constexpr auto& test_data =
    R"(162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
)";

static_assert(part1<10>(parse_input(test_data)) == 40);
static_assert(part2(parse_input(test_data)) == 25272);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(std::cerr, "No input");
        return -1;
    }

    auto const boxes = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1<1000>(boxes));
    std::println("Part 2: {}", part2(boxes));
}