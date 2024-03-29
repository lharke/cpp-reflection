#pragma once

#include <functional>

#include "reflection/utility.h"

namespace lh::reflection
{
    class GenericFunction : private utility::non_copyable
    {
    protected:
        explicit GenericFunction(std::size_t hash) noexcept :
            _hash(hash)
        {
        }

    public:
        template <typename R, typename... Ts>
        R invoke(Ts... args) const;

        std::size_t hash() const noexcept
        {
            return _hash;
        }

    private:
        std::size_t _hash;
    };

    template <typename R, typename... Ts>
    class SpecificFunction final : public GenericFunction
    {
    public:
        explicit SpecificFunction(std::function<R(Ts...)> fn) noexcept :
            GenericFunction(utility::signatureHash<Ts...>()),
            _fn(fn)
        {
        }

        R invoke(Ts... args) const
        {
            return _fn(args...);
        }

    private:
        std::function<R(Ts...)> _fn;
    };

    template <typename R, typename... Ts>
    R GenericFunction::invoke(Ts... args) const
    {
        return static_cast<const SpecificFunction<R, Ts...> *>(this)->invoke(args...);
    }
}
