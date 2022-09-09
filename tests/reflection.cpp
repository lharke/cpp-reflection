#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <reflection/reflectable.h>

using namespace lh::reflection;

TEST_CASE("Default Construct")
{
    Reflectable testee;

    auto& mo = const_cast<MetaObject&>(testee.metaObject());
    auto hash = mo.hash();

    int count = 42;

    mo.addMethod("check", std::function([&count](Reflectable*, int value) -> bool {
        return count == value;
    }), Method::Qualifier::Immutable);

    mo.addMethod("check", std::function([&count](int value) -> bool {
        return count == value;
    }));

    mo.addProperty("count",
        std::function([&count](const Reflectable*) -> int {
            return count;
        }),
        std::function([&count](Reflectable*, int value) -> void {
            count = value;
        })
    );

    mo.addEvent<int>("test");

    testee.subscribe("test", std::function([&count](int value) {
        if(value == 42)
            count = 43;
    }));

    testee.event("test", count);

    bool result = testee.invoke<bool>("check", 43);

    testee.set("count", 42);
    int test = testee.get<int>("count");

    result = mo.method("check").invoke<bool>(42);
}

TEST_CASE("MetaObject Instantiation")
{
    MetaObject meta("Reflectable", typeid(Reflectable));

    REQUIRE(meta.name() == "Reflectable");
    REQUIRE(meta.type() == typeid(Reflectable));
    REQUIRE(meta.hash() == typeid(Reflectable).hash_code());
}

TEST_CASE("MetaObject Methods")
{
    MetaObject meta("Reflectable", typeid(Reflectable));

    REQUIRE_NOTHROW(meta.addMethod("check", std::function([](Reflectable*, int value) -> bool {
        return value == 42;
    })));

    REQUIRE_NOTHROW(meta.addMethod("check", std::function([](Reflectable*, int value, int test) -> bool {
        return value == test;
    })));

    REQUIRE_NOTHROW(meta.addMethod("check", std::function([](Reflectable*) -> void {
    })));

    REQUIRE(meta.methods().size() == 1);
    REQUIRE_NOTHROW(meta.method("check"));
}
