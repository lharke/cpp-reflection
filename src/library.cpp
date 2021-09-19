#include "reflection/library.h"

#include <unordered_map>
#include <algorithm>

#include "reflection/exceptions.h"

namespace rc::reflection
{
    struct Library::State
    {
        std::unordered_map<std::string, const MetaObject *> metaObjects;
    };

    Library::Library() noexcept : d(std::make_unique<State>())
    {
    }

    Library &Library::global() noexcept
    {
        static Library library;
        return library;
    }

    Library &Library::thread() noexcept
    {
        thread_local Library library;
        return library;
    }

    void Library::add(const MetaObject *_metaObject)
    {
        if (exists(_metaObject->name()))
            throw registration_failed("type " + _metaObject->name() + " is already registered (hashes " + (*_metaObject == metaObject(_metaObject->name()) ? "" : "do not") + " match)");

        d->metaObjects[_metaObject->name()] = _metaObject;
    }

    bool Library::exists(std::string name) const noexcept
    {
        return d->metaObjects.contains(name);
    }

    std::vector<const MetaObject *> Library::metaObjects() const noexcept
    {
        std::vector<const MetaObject *> metaObjects;
        std::ranges::transform(d->metaObjects, metaObjects.begin(), [](const auto &pair) { return pair.second; });
        return metaObjects;
    }

    const MetaObject &Library::metaObject(std::string name) const
    {
        if (auto where = d->metaObjects.find(name); where != d->metaObjects.end())
            return *where->second;

        throw unknown_type(name);
    }
}
