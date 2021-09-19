#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <functional>
#include <typeindex>

#include "reflection/utility.h"
#include "reflection/function.h"
#include "reflection/exceptions.h"

namespace rc::reflection
{
    class Method final : private utility::non_copyable
    {
    public:
        enum class Qualifier
        {
            Mutable,
            Immutable,
            Static
        };

        class Overload final : private utility::non_copyable
        {
        public:
            template <typename R, typename C, typename... Ts>
            explicit Overload(std::function<R(C *, Ts...)> fn, Qualifier qualifier) noexcept :
                _returnType(typeid(R)),
                _argumentTypes(utility::signature<Ts...>()),
                _function(std::make_unique<SpecificFunction<R, C *, Ts...>>(fn)),
                _qualifier(qualifier)
            {
            }

            template <typename R, typename... Ts>
            explicit Overload(std::function<R(Ts...)> fn) noexcept :
                _returnType(typeid(R)),
                _argumentTypes(utility::signature<Ts...>()),
                _function(std::make_unique<SpecificFunction<R, Ts...>>(fn)),
                _qualifier(Method::Qualifier::Static)
            {
            }

            std::type_index returnType() const noexcept { return _returnType; }
            std::vector<std::type_index> argumentTypes() const noexcept { return _argumentTypes; }
            std::size_t hash() const noexcept { return _function->hash(); };

            Qualifier qualifier() const noexcept { return _qualifier; };

            template <typename R, typename C, typename... Ts>
            R invoke(C *instance, Ts... args) const
            {
                return _function->invoke<R>(instance, args...);
            }

            template <typename R, typename... Ts>
            R invoke(Ts... args) const
            {
                return _function->invoke<R>(args...);
            }

        private:
            std::type_index _returnType;
            std::vector<std::type_index> _argumentTypes;
            std::unique_ptr<GenericFunction> _function;
            Qualifier _qualifier;
        };

        explicit Method(std::string name) noexcept :
            _name(name)
        {
        }

        std::string name() const noexcept { return _name; };

        std::vector<const Overload *> overloads() const noexcept
        {
            std::vector<const Overload *> overloads(_overloads.size(), nullptr);
            std::ranges::transform(_overloads, overloads.begin(), [](const auto &pair) { return &pair.second; });
            return overloads;
        }

        const Overload &overload(std::size_t hash, Qualifier qualifier) const
        {
            if (auto where = _overloads.find({hash, qualifier}); where != _overloads.end())
                return where->second;
            else if(qualifier == Qualifier::Mutable) // const methods can be called on non-const objects
                if (auto where = _overloads.find({hash, Qualifier::Immutable}); where != _overloads.end())
                    return where->second;

            throw unknown_method("unknown signature " + std::to_string(hash) + " for method " + name());
        }

        template <typename R, typename C, typename... Ts>
        void addOverload(std::function<R(C *, Ts...)> fn, Method::Qualifier qualifier)
        {
            const auto hash = utility::signatureHash<Ts...>();

            if (_overloads.contains({hash, qualifier}))
                throw registration_failed("method overload with signature " + name() + "(" + utility::signatureString<Ts...>() + ") is already registered");

            _overloads.emplace(overload_index{hash, qualifier}, Overload(fn, qualifier));
        }

        template <typename R, typename... Ts>
        void addOverload(std::function<R(Ts...)> fn)
        {
            const auto hash = utility::signatureHash<Ts...>();

            if (_overloads.contains({hash, Method::Qualifier::Static}))
                throw registration_failed("method overload with signature " + name() + "(" + utility::signatureString<Ts...>() + ") is already registered");

            _overloads.emplace(overload_index{hash, Method::Qualifier::Static}, Overload(fn));
        }

        template <typename R, typename C, typename... Ts>
        R invoke(C *instance, Ts... args, Qualifier qualifier) const try
        {
            // TODO: use is_invocable to find overload for more flexibility
            static const auto hash = utility::signatureHash<Ts...>();
            return overload(hash, qualifier).invoke<R>(instance, args...);
        }
        catch (const unknown_method &)
        {
            throw unknown_method("unknown signature " + name() + "(" + utility::signatureString<Ts...>() + ") for method " + name());
        }

        template <typename R, typename... Ts>
        R invoke(Ts... args) const try
        {
            // TODO: use is_invocable to find overload for more flexibility
            static const auto hash = utility::signatureHash<Ts...>();
            return overload(hash, Method::Qualifier::Static).invoke<R>(args...);
        }
        catch (const unknown_method&)
        {
            throw unknown_method("unknown signature " + name() + "(" + utility::signatureString<Ts...>() + ") for method " + name());
        }

    private:
        struct overload_index
        {
            std::size_t hash;
            Qualifier context;
        };

        struct overload_index_hash
        {
            std::size_t operator()(struct overload_index const& c) const noexcept
            {
                const auto h1 = std::hash<std::size_t>{}(c.hash);
                const auto h2 = std::hash<Qualifier>{}(c.context);
                return h1 ^ (h2 << 1);
            }
        };

        struct overload_index_equal
        {
            bool operator()(struct overload_index const& a, struct overload_index const& b) const noexcept
            {
                return a.hash == b.hash && a.context == b.context;
            }
        };

        std::string _name;
        std::unordered_map<overload_index, Overload, overload_index_hash, overload_index_equal> _overloads;
    };
}
