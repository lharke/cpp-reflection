#pragma once

#include <string>
#include <vector>
#include <functional>
#include <typeindex>

#include "reflection/utility.h"
#include "reflection/function.h"
#include "reflection/exceptions.h"

namespace lh::reflection
{
    class Event final : private utility::non_copyable
    {
    public:
        class Subscription final : private utility::non_copyable
        {
            friend class Event;

        protected:
            template <typename... Ts>
            explicit Subscription(std::function<void(Ts...)> fn) noexcept :
                _handler(new SpecificFunction<void, Ts...>(fn))
            {
            }

            template <typename... Ts>
            void invoke(Ts... args) const
            {
                _handler->invoke<void>(args...);
            }

        public:
            bool operator==(const Subscription& other) const noexcept { return _handler == other._handler; }

        private:
            std::unique_ptr<GenericFunction> _handler;
        };

        explicit Event(std::string name, std::vector<std::type_index> argumentTypes) noexcept :
            _name(name),
            _argumentTypes(argumentTypes),
            _hash(utility::signatureHash(argumentTypes))
        {
        }

        std::string name() const noexcept { return _name; }
        std::vector<std::type_index> argumentTypes() const noexcept { return _argumentTypes; }
        std::size_t hash() const noexcept { return _hash; }

        template <typename... Ts>
        Subscription subscribe(std::function<void(Ts...)> handler) const
        {
            static const auto signatureHash = utility::signatureHash<Ts...>();
            if (signatureHash != hash())
                throw invalid_event_type("handler with arguments " + utility::signatureString<Ts...>() + " is incompatible with event arguments " + utility::signatureString(argumentTypes()) + " of event " + name());

            return Subscription(handler);
        }

        template <typename... Ts>
        void invoke(const Subscription &subscription, Ts... args) const
        {
            static const auto signatureHash = utility::signatureHash<Ts...>();
            if (signatureHash != hash())
                throw invalid_event_type("passed arguments " + utility::signatureString<Ts...>() + " are incompatible with event arguments " + utility::signatureString(argumentTypes()) + " of event " + name());

            subscription.invoke(args...);
        }

    private:
        std::string _name;
        std::vector<std::type_index> _argumentTypes;
        std::size_t _hash;
    };
}
