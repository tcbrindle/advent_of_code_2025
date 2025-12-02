
module;

#include <ankerl/unordered_dense.h>

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
export template <std::integral I>
constexpr auto try_parse =
    [](flux::multipass_sequence auto&& f) -> std::optional<I> {
    // constexpr auto is_space = flow::pred::in(' ', '\f', '\n', '\r', '\t',
    // '\v'); constexpr auto is_digit = flow::pred::geq('0') &&
    // flow::pred::leq('9');
    constexpr auto is_space = [](char c) {
        return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
               c == '\v';
    };
    constexpr auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

    auto f2 = flux::ref(f).drop_while(is_space);

    I mult = 1;
    std::optional<I> first{};

    // Deal with the first character
    {
        auto cur = f2.first();
        if (f2.is_last(cur)) { return std::nullopt; }

        char c = f2[cur];

        if (c == '-') {
            mult = -1;
        } else if (c == '+') {
            // pass
        } else if (is_digit(c)) {
            first = c - '0';
        } else {
            return {};
        }
    }

    // Deal with the rest
    auto res = std::move(f2).drop(1).take_while(is_digit).fold(
        [](auto acc, char c) -> std::optional<I> {
            return 10 * acc.value_or(0) + (c - '0');
        },
        std::move(first));
    if (res) { *res *= mult; }
    return res;
};

export template <std::integral I>
constexpr auto parse = [](flux::multipass_sequence auto&& seq) -> I {
    return try_parse<I>(std::forward<decltype(seq)&&>(seq)).value();
};

export template <typename T>
constexpr auto vector_from_file = [](char const* path) {
    std::ifstream file(path);
    return flux::from_istream<T>(file).template to<std::vector<T>>();
};

export constexpr auto string_from_file = [](const char* path) {
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
        if constexpr (std::has_unique_object_representations_v<
                          aoc::vec2_t<T>>) {
            return detail::wyhash::hash(&v, sizeof(v));
        } else {
            return hash<std::pair<T, T>>{}(std::pair<T, T>(v.x, v.y));
        }
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
