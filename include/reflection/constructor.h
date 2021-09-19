#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <typeindex>

#include "reflection/utility.h"
#include "reflection/function.h"

namespace rc::reflection
{
    class Constructor final : private utility::non_copyable
    {
    public:
        template <typename R, typename... Ts>
        explicit Constructor(std::function<R(Ts...)> fn) noexcept :
            _returnType(typeid(R)),
            _argumentTypes(utility::signature<Ts...>()),
            _function(std::make_unique<SpecificFunction<R, Ts...>>(fn))
        {
        }

        std::type_index returnType() const noexcept { return _returnType; }
        std::vector<std::type_index> argumentTypes() const noexcept { return _argumentTypes; }
        std::size_t hash() const noexcept { return _function->hash(); }

        template <typename R, typename... Ts>
        R invoke(Ts... args) const
        {
            return _function->invoke<R>(args...);
        }

    private:
        std::type_index _returnType;
        std::vector<std::type_index> _argumentTypes;
        std::unique_ptr<GenericFunction> _function;
    };
}
