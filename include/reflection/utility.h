#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <typeindex>

namespace lh::reflection::utility
{
    /* Concepts */

    struct non_copyable
    {
        non_copyable() = default;
        non_copyable(const non_copyable &) = delete;
        non_copyable &operator=(const non_copyable &) = delete;
        non_copyable(non_copyable&&) = default;
        non_copyable& operator= (non_copyable&&) = default;
    };

    /* Algorithms */

    template <std::input_iterator iterator_t>
    std::string join(iterator_t begin, iterator_t end, std::string sep) noexcept
    {
        std::stringstream result;

        if (begin != end)
            result << *begin++;

        while (begin != end)
        {
            result << sep;
            result << *begin++;
        }

        return result.str();
    }

    template <std::ranges::common_range range_t>
    std::string join(range_t range, std::string sep) noexcept
    {
        return join(range.begin(), range.end(), sep);
    }

    template <class... Ts>
    constexpr std::vector<std::type_index> signature() noexcept
    {
        return std::vector<std::type_index>({typeid(Ts)...});
    }

    std::string signatureString(const std::vector<std::type_index> &arguments) noexcept;

    template <class... Ts>
    std::string signatureString() noexcept
    {
        return join(std::vector<std::string>{std::string(typeid(Ts).name())...}, ", ");
    }

    size_t signatureHash(const std::vector<std::type_index> &arguments) noexcept;

    template <class... Ts>
    size_t signatureHash() noexcept
    {
        return signatureHash(signature<Ts...>());
    }
}
