
export module aoc;

export import flux;
export import std;
export import ankerl.unordered_dense;

namespace aoc {

export constexpr auto abs = []<std::signed_integral T>(T val) -> T {
    return val < T{} ? -val : val;
};

export template <typename K, typename V>
using hash_map = ankerl::unordered_dense::map<K, V>;

export template <typename K>
using hash_set = ankerl::unordered_dense::set<K>;

// This function is not great, but nor are the alternatives:
//  * std::from_chars - not constexpr, requires contiguous input
//  * std::atoi - same
//  * std::stoi - needs a std::string, throws on failure
//  * istreams - urgh
// So, we're gonna roll our own
// No, I'm not going to try and do this for floating point...
export template <flux::num::integral I>
constexpr auto try_parse = []<flux::iterable F>
    requires std::same_as<flux::iterable_value_t<F>, char>
(F&& f) static -> std::optional<I> {
    constexpr auto is_space = flux::pred::in(' ', '\f', '\n', '\r', '\t', '\v');
    constexpr auto is_digit = flux::pred::geq('0') && flux::pred::leq('9');

    auto drop_spaces = flux::drop_while(std::ref(f), is_space);
    flux::iteration_context auto ctx = flux::iterate(drop_spaces);

    I multiplier = 1;
    std::optional<I> value = std::nullopt;

    if (auto cur = flux::next_element(ctx)) {
        char c = *cur;

        if (c == '-') {
            if constexpr (flux::num::signed_integral<I>) {
                multiplier = -1;
            } else {
                return std::nullopt;
            }
        } else if (c == '+') {
            // pass
        } else if (is_digit(c)) {
            value = c - '0';
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }

    while (auto digit = flux::next_element(ctx)) {
        char c = *digit;
        if (!is_digit(c)) { break; }
        value = 10 * value.value_or(0) + (c - '0');
    }

    if (value) { *value *= multiplier; }
    return value;
};

static_assert(not try_parse<int>("").has_value());
static_assert(not try_parse<int>("+").has_value());
static_assert(try_parse<unsigned short>("8").value() == 8);
static_assert(try_parse<int>("-1").value() == -1);

export template <flux::num::integral I>
constexpr auto parse = []<flux::iterable F>
    requires std::same_as<flux::iterable_value_t<F>, char>
(F&& seq) static -> I { return try_parse<I>(std::forward<F&&>(seq)).value(); };

export constexpr auto string_from_file =
    [](const char* path) static -> std::string {
    std::ifstream file(path);
    return flux::to<std::string>(*file.rdbuf());
};

export struct timer {
    using clock = std::chrono::high_resolution_clock;

    template <typename D = std::chrono::microseconds>
    auto elapsed() const -> D
    {
        return std::chrono::duration_cast<D>(clock::now() - start_);
    }

    void reset() { start_ = clock::now(); }

private:
    typename clock::time_point start_ = clock::now();
};

export template <typename D = std::chrono::microseconds, typename F,
                 typename... Args>
constexpr auto timed(F&& f, Args&&... args)
    -> std::pair<std::invoke_result_t<F, Args...>, D>
{
    timer t;
    return {std::invoke(std::forward<F>(f), std::forward<Args>(args)...),
            t.elapsed<D>()};
}

export template <typename T>
struct vec2_t {
    T x = T{};
    T y = T{};

    friend auto operator==(vec2_t const& lhs, vec2_t const& rhs)
        -> bool = default;
    friend auto operator<=>(vec2_t const& lhs, vec2_t const& rhs) = default;

    friend constexpr auto operator+=(vec2_t& lhs, vec2_t const& rhs) -> vec2_t&
    {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    friend constexpr auto operator-=(vec2_t& lhs, vec2_t const& rhs) -> vec2_t&
    {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }

    friend constexpr auto operator+(vec2_t lhs, vec2_t const& rhs) -> vec2_t
    {
        return lhs += rhs;
    }

    friend constexpr auto operator-(vec2_t lhs, vec2_t const& rhs) -> vec2_t
    {
        return lhs -= rhs;
    }

    friend constexpr auto operator*(T const& scalar, vec2_t const& vec)
        -> vec2_t
    {
        return {vec.x * scalar, vec.y * scalar};
    }
};

} // namespace aoc

template <typename T>
struct ankerl::unordered_dense::hash<aoc::vec2_t<T>> {
    using is_avalanching = void;

    [[nodiscard]] auto operator()(aoc::vec2_t<T> const& v) const noexcept
        -> std::uint64_t
    {
        return hash<std::pair<T, T>>{}(std::pair<T, T>(v.x, v.y));
    }
};

template <typename T, typename CharT>
struct std::formatter<aoc::vec2_t<T>, CharT> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const aoc::vec2_t<T>& v,
                          std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};
