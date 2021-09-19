#include "reflectable.h"

class Object : public rc::reflection::Reflectable
{
public:
    static const rc::reflection::MetaObject &staticMetaObject() { return _metaObject; }
    virtual const rc::reflection::MetaObject &metaObject() const override { return _metaObject; }
    static struct MetaObject : public rc::reflection::MetaObject
    {
        using type = Object;
        using base_type = Reflectable;
        MetaObject() : rc::reflection::MetaObject("Object", {&Reflectable::staticMetaObject()})
        {
        }
    } _metaObject;

    Object(std::string test)
    {
    }

    void test(std::string test, int asdf)
    {
    }
};

namespace rc::reflection
{
    void test()
    {
        auto ref = Reflectable();
        const auto const_ref = Reflectable();

        ref.subscribe("foo", std::function([](std::string arg1) {}));
        ref.trigger("foo", "bar");

        auto foo = const_ref.get<std::string>("bar");
        ref.set("foobar", foo);

        const_ref.clone();

        std::hash<Reflectable> hash_func;
        auto hash = hash_func(const_ref);
    }
}