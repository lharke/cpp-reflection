#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

#include "reflection/utility.h"
#include "reflection/constructor.h"
#include "reflection/method.h"
#include "reflection/property.h"
#include "reflection/event.h"

#ifdef WIN32
#define CLASS_CALLING_CONVENTION __thiscall
#else
#define CLASS_CALLING_CONVENTION
#endif

namespace rc::reflection
{
    class MetaObject final : private utility::non_copyable
    {
    public:
        explicit MetaObject(std::string name, std::type_index type, std::vector<const MetaObject *> bases = {}) noexcept :
            _name(name),
            _type(type),
            _bases(bases)
        {
        }

        std::string name() const noexcept { return _name; }
        std::type_index type() const noexcept { return _type; }
        std::size_t hash() const noexcept { return _type.hash_code(); }

        bool operator==(const MetaObject &other) const noexcept { return hash() == other.hash(); }

        /* Base Classes */

        std::vector<const MetaObject *> bases() const noexcept { return _bases; }

        const MetaObject &base(std::string name) const
        {
            if (auto where = std::ranges::find_if(_bases, [&name](const auto &base) { return base->name() == name; }); where != _bases.end())
                return **where;

            throw unknown_type(name);
        }

        bool hasBase(std::string name) const noexcept
        {
            return std::ranges::any_of(_bases, [&name](const auto& base) { return base->name() == name; });
        }

        /* Constructors */

        std::vector<const Constructor *> constructors() const noexcept
        {
            std::vector<const Constructor *> constructors(_constructors.size(), nullptr);
            std::ranges::transform(_constructors, constructors.begin(), [](const auto &pair) { return &pair.second; });
            return constructors;
        }

        const Constructor &constructor(std::size_t hash) const
        {
            if (auto where = _constructors.find(hash); where != _constructors.end())
                return where->second;

            throw unknown_constructor(std::to_string(hash));
        }

        bool hasConstructor(std::size_t hash) const noexcept
        {
            return _constructors.contains(hash);
        }

        template <typename R, typename... Ts>
        void addConstructor(std::function<R(Ts...)> constructor)
        {
            const auto hash = utility::signatureHash<Ts...>();
            if (_constructors.contains(hash))
                throw registration_failed("constructor " + name() + "(" + utility::signatureString<Ts...>() + ") is already registered");

            _constructors.emplace(hash, Constructor(constructor));
        }

        /* Methods */

        std::vector<const Method *> methods() const noexcept
        {
            std::vector<const Method *> methods(_methods.size(), nullptr);
            std::ranges::transform(_methods, methods.begin(), [](const auto &pair) { return &(pair.second); });
            return methods;
        }

        const Method &method(std::string name) const
        {
            if (auto where = _methods.find(name); where != _methods.end())
                return where->second;

            throw unknown_method(name);
        }

        bool hasMethod(std::string name) const noexcept
        {
            return _methods.contains(name);
        }

        template <typename R, typename C, typename... Ts>
        void addMethod(std::string name, std::function<R(C *, Ts...)> fn, Method::Qualifier qualifier)
        {
            if (!_methods.contains(name))
                _methods.emplace(name, Method(name));

            _methods.at(name).addOverload(fn, qualifier);
        }

        template <typename R, typename... Ts>
        void addMethod(std::string name, std::function<R(Ts...)> fn)
        {
            if (!_methods.contains(name))
                _methods.emplace(name, Method(name));

            _methods.at(name).addOverload(fn);
        }

        template <typename R, typename C, typename... Ts>
        void addMethod(std::string name, R (CLASS_CALLING_CONVENTION C::*fn_ptr)(Ts...))
        {
            addMethod(name, std::function<R(C *, Ts...)>(fn_ptr), Method::Qualifier::Mutable);
        }

        template <typename R, typename C, typename... Ts>
        void addMethod(std::string name, R (CLASS_CALLING_CONVENTION C::*fn_ptr)(Ts...) const)
        {
            addMethod(name, std::function<R(C *, Ts...)>(fn_ptr), Method::Qualifier::Immutable);
        }

        template <typename R, typename... Ts>
        void addMethod(std::string name, R fn_ptr(Ts...))
        {
            addMethod(name, std::function<R(Ts...)>(fn_ptr), Method::Qualifier::Static);
        }

        /* Properties */

        std::vector<const Property *> properties() const noexcept
        {
            std::vector<const Property *> properties(_properties.size(), nullptr);
            std::ranges::transform(_properties, properties.begin(), [](const auto &pair) { return &pair.second; });
            return properties;
        }

        const Property &property(std::string name) const
        {
            if (auto where = _properties.find(name); where != _properties.end())
                return where->second;

            throw unknown_property(name);
        }

        bool hasProperty(std::string name) const noexcept
        {
            return _properties.contains(name);
        }

        template <typename C, typename T>
        void addProperty(std::string name, std::function<T(const C *)> getter, std::function<void(C *, T)> setter)
        {
            if (_properties.contains(name))
                throw registration_failed("property " + name + " is already registered");

            _properties.emplace(name, Property(name, getter, setter));
        }

        /* Events */

        std::vector<const Event *> events() const noexcept
        {
            std::vector<const Event *> events(_events.size(), nullptr);
            std::ranges::transform(_events, events.begin(), [](const auto &pair) { return &pair.second; });
            return events;
        }

        const Event &event(std::string name) const
        {
            if (auto where = _events.find(name); where != _events.end())
                return where->second;

            throw unknown_event(name);
        }

        bool hasEvent(std::string name) const noexcept
        {
            return _events.contains(name);
        }

        template <typename... Ts>
        void addEvent(std::string name)
        {
            if (_events.contains(name))
                throw registration_failed("event " + name + " is already registered");

            _events.emplace(name, Event(name, utility::signature<Ts...>()));
        }

        /* Factory */

        template <typename R, typename... Ts>
        R *instantiate(Ts... args) const try
        {
            static const auto hash = utility::signatureHash<Ts...>();
            return constructor(hash)->invoke(args);
        }
        catch (const unknown_constructor &)
        {
            throw unknown_constructor(name() + "(" + utility::signatureString<Ts...>() + ")");
        }

    private:
        std::string _name;
        std::type_index _type;

        std::vector<const MetaObject *> _bases;

        std::unordered_map<std::size_t, Constructor> _constructors;
        std::unordered_map<std::string, Method> _methods;
        std::unordered_map<std::string, Property> _properties;
        std::unordered_map<std::string, Event> _events;
    };
}

namespace std
{
    template <>
    struct hash<rc::reflection::MetaObject>
    {
        std::size_t operator()(const rc::reflection::MetaObject &m) const noexcept
        {
            return m.hash();
        }
    };
}
