#pragma once

#include <string>
#include <map>
#include <vector>
#include <optional>
#include <functional>

#include "reflection/metaobject.h"

namespace lh::reflection
{
    class Reflectable
    {
    public:
        Reflectable() noexcept = default;

        Reflectable(const Reflectable &) noexcept {}
        Reflectable(Reflectable &&other) noexcept : _subscriptions(std::move(other._subscriptions)) {}

        Reflectable &operator=(const Reflectable &) noexcept { return *this; }
        Reflectable &operator=(Reflectable &&) noexcept { return *this; }

        static const MetaObject &staticMetaObject() noexcept { return _metaObject; };
        virtual const MetaObject &metaObject() const noexcept { return _metaObject; };

        template <typename T>
        T get(std::string propertyName) const
        {
            return metaObject().property(propertyName).get<Reflectable, T>(this);
        }

        template <typename T>
        void set(std::string propertyName, T value)
        {
            metaObject().property(propertyName).set(this, value);
        }

        template <typename R = void, typename... Ts>
        R invoke(std::string methodName, Ts... args)
        {
            return metaObject().method(methodName).invoke<R, Reflectable, Ts...>(this, args..., Method::Qualifier::Mutable);
        }

        template <typename R = void, typename... Ts>
        R invoke(std::string methodName, Ts... args) const
        {
            return metaObject().method(methodName).invoke<R, const Reflectable, Ts...>(this, args..., Method::Qualifier::Immutable);
        }

        template <typename... Ts>
        void event(std::string eventName, Ts... args) const
        {
            if (!_subscriptions)
                return;

            auto &event = metaObject().event(eventName);

            if (auto where = _subscriptions->find(&event); where != _subscriptions->end())
                for (const auto &subscription : where->second)
                    event.invoke(subscription, args...);
        }

        template <typename... Ts>
        const Event::Subscription *subscribe(std::string eventName, std::function<void(Ts...)> eventHandler)
        {
            auto &event = metaObject().event(eventName);
            auto subscription = event.subscribe(eventHandler);

            if (!_subscriptions)
                _subscriptions = subscription_map();

            return &(*_subscriptions)[&event].emplace_back(std::move(subscription));
        }

        void unsubscribe(std::string eventName, const Event::Subscription *subscription)
        {
            if (!_subscriptions)
                return;

            auto &event = metaObject().event(eventName);
            std::erase((*_subscriptions)[&event], *subscription);
        }

        void unsubscribe()
        {
            if (!_subscriptions)
                return;

            _subscriptions.reset();
        }

        virtual Reflectable *clone() const
        {
            return new Reflectable(*this);
        }

        virtual std::size_t hash() const noexcept
        {
            return static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(this));
        }

    private:
        using subscription_map = std::map<const Event*, std::vector<Event::Subscription>>;

        std::optional<subscription_map> _subscriptions;
        static MetaObject _metaObject;
    };
}

namespace std
{
    template <>
    struct hash<lh::reflection::Reflectable>
    {
        std::size_t operator()(const lh::reflection::Reflectable &r) const noexcept
        {
            return r.hash();
        }
    };
}
