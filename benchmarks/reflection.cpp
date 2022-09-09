#include <benchmark/benchmark.h>
#include <reflection/reflectable.h>

using namespace benchmark;
using namespace lh::reflection;

void reflectable_invoke_mutable(State& state)
{
	Reflectable testee;
	auto& mo = const_cast<MetaObject&>(testee.metaObject());

	int count = 0;

	if (!mo.hasMethod("increment"))
	{
		mo.addMethod("increment", std::function([&count](Reflectable*, int value) -> void {
			count += value;
		}), Method::Qualifier::Mutable);
	}

	for (auto _ : state)
		testee.invoke("increment", 5);
}

void reflectable_invoke_immutable(State& state)
{
	Reflectable testee;
	auto& mo = const_cast<MetaObject&>(testee.metaObject());

	int count = 42;

	if (!mo.hasMethod("compare"))
	{
		mo.addMethod("compare", std::function([&count](Reflectable*, int value) -> bool {
			return count == value;
		}), Method::Qualifier::Immutable);
	}

	for (auto _ : state)
		bool result = testee.invoke<bool>("compare", 42);
}

void reflectable_invoke_static(State& state)
{
	Reflectable testee;
	auto& mo = const_cast<MetaObject&>(testee.metaObject());

	int count = 42;

	if (!mo.hasMethod("static_compare"))
	{
		mo.addMethod("static_compare", std::function([&count](int value) -> bool {
			return count == value;
		}));
	}

	for (auto _ : state)
		mo.method("static_compare").invoke<bool>(42);
}

void reflectable_event(State& state)
{
	Reflectable testee;
	auto& mo = const_cast<MetaObject&>(testee.metaObject());

	int count = 42;

	if (!mo.hasEvent("test"))
	{
		mo.addEvent<int>("test");
	}

	testee.subscribe("test", std::function([&count](int value) -> void {
		if (value == count)
			++count;
	}));

	for (auto _ : state)
		testee.event("test", count);
}

BENCHMARK(reflectable_invoke_mutable);
BENCHMARK(reflectable_invoke_immutable);
BENCHMARK(reflectable_invoke_static);
BENCHMARK(reflectable_event);

BENCHMARK_MAIN();