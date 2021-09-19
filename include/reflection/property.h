#pragma once

#include <string>
#include <vector>
#include <typeindex>

#include "reflection/utility.h"
#include "reflection/function.h"

namespace rc::reflection
{
    class Property final
    {
    public:
        template <typename C, typename T>
        Property(std::string name, std::function<T(const C *)> getter, std::function<void(C *, T)> setter) noexcept :
            _name(name),
            _type(typeid(T)),
            _getter(std::make_unique<SpecificFunction<T, const C *>>(getter)),
            _setter(std::make_unique<SpecificFunction<void, C *, T>>(setter))
        {
        }

        std::string name() const noexcept { return _name; }
        std::type_index type() const noexcept { return _type; }
        std::size_t hash() const noexcept { return _type.hash_code(); }

        template <typename C, typename T>
        T get(const C *instance) const
        {
            // TODO: check types
            return _getter->invoke<T>(instance);
        }

        template <typename C, typename T>
        void set(C *instance, T value) const
        {
            // TODO: check types
            _setter->invoke<void>(instance, value);
        }

    private:
        std::string _name;
        std::type_index _type;
        std::unique_ptr<GenericFunction> _getter;
        std::unique_ptr<GenericFunction> _setter;
    };
}
