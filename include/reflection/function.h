#pragma once

#include <functional>

namespace rc::reflection
{
    class GenericFunction
    {
    protected:
        GenericFunction() noexcept
        {
        }

    public:
        template <typename R, typename... Ts>
        R invoke(Ts... args) const;
    };

    template <typename R, typename... Ts>
    class SpecificFunction final : public GenericFunction
    {
    public:
        SpecificFunction(std::function<R(Ts...)> fn) noexcept :
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
        // TODO: totally not typesafe, check with is_invocable beforehand
        return static_cast<const SpecificFunction<R, Ts...> *>(this)->invoke(args...);
    }
}
