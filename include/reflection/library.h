#pragma once

#include <string>
#include <vector>
#include <memory>

#include "reflection/metaobject.h"

namespace lh::reflection
{
    class Library : private utility::non_copyable
    {
        Library() noexcept;

    public:
        static Library &global() noexcept;
        static Library &thread() noexcept;

        void add(const MetaObject *metaObject);
        bool exists(std::string name) const noexcept;

        template <typename R, typename... Ts>
        R *instantiate(std::string name, Ts... args)
        {
            return metaObject(name).instantiate(args);
        }

        std::vector<const MetaObject *> metaObjects() const noexcept;
        const MetaObject &metaObject(std::string name) const;

    private:
        struct State;
        std::unique_ptr<State> d;
    };
}
