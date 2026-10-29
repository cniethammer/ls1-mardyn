/** @file PluginFactoryTest.cpp
 * @brief Catch2 unit tests for the generic PluginFactory<T>.
 *
 * Uses dummy plugin types defined in this file only, so the tests do not depend
 * on any real plugin or on a simulation setup.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <memory>
#include <string>
#include <vector>

#include "plugins/PluginFactory.h"
#include "utils/Logger.h"

namespace {

/** @brief Provide the global logger used by PluginFactory for the whole test run. */
class GlobalLogListener : public Catch::EventListenerBase {
public:
	using Catch::EventListenerBase::EventListenerBase;

	void testRunStarting(Catch::TestRunInfo const&) override {
		Log::global_log = std::make_unique<Log::Logger>(Log::Error);
	}

	void testRunEnded(Catch::TestRunStats const&) override { Log::global_log.reset(); }
};

/** @brief Plugin interface used for testing; mimics the requirements on T. */
class DummyPluginBase {
public:
	virtual ~DummyPluginBase() = default;
	virtual std::string getPluginName() = 0;
};

class DummyPluginA : public DummyPluginBase {
public:
	std::string getPluginName() override { return "DummyPluginA"; }
	static DummyPluginBase* createInstance() { return new DummyPluginA(); }
};

class DummyPluginB : public DummyPluginBase {
public:
	std::string getPluginName() override { return "DummyPluginB"; }
	static DummyPluginBase* createInstance() { return new DummyPluginB(); }
};

/** @brief Different class reporting the same name as DummyPluginA (name clash). */
class DummyPluginC : public DummyPluginBase {
public:
	std::string getPluginName() override { return "DummyPluginA"; }
	static DummyPluginBase* createInstance() { return new DummyPluginC(); }
};

/** @brief Derived factory registering plugins via REGISTER_PLUGIN, as ObjectFactory does. */
class DummyFactory : public PluginFactory<DummyPluginBase> {
public:
	DummyFactory() {
		REGISTER_PLUGIN(DummyPluginA);
		REGISTER_PLUGIN(DummyPluginB);
	}
};

}  // namespace

CATCH_REGISTER_LISTENER(GlobalLogListener)

TEST_CASE("A new factory has no plugins", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;

	CHECK(factory.getPluginNames().empty());
}

TEST_CASE("Factories do not share registered plugins", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factoryA;
	factoryA.registerPlugin(&DummyPluginA::createInstance);

	PluginFactory<DummyPluginBase> factoryB;

	CHECK(factoryB.getPluginNames().empty());
	CHECK_THROWS_AS(factoryB.create("DummyPluginA"), PluginNotFoundError);
}

TEST_CASE("create() returns an instance of the registered plugin", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);

	std::unique_ptr<DummyPluginBase> plugin(factory.create("DummyPluginA"));

	REQUIRE(plugin != nullptr);
	CHECK(dynamic_cast<DummyPluginA*>(plugin.get()) != nullptr);
	CHECK(plugin->getPluginName() == "DummyPluginA");
}

TEST_CASE("create() throws PluginNotFoundError for unknown plugin names", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);

	CHECK_THROWS_AS(factory.create("Unknown"), PluginNotFoundError);
	CHECK_THROWS_AS(factory.create(""), PluginNotFoundError);
	CHECK_THROWS_AS(factory.create("dummyplugina"), PluginNotFoundError);  // names are case sensitive
}

TEST_CASE("PluginNotFoundError provides the requested and the available plugin names", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);
	factory.registerPlugin(&DummyPluginB::createInstance);

	try {
		std::unique_ptr<DummyPluginBase> plugin(factory.create("Unknown"));
		FAIL("create() did not throw");
	} catch (const PluginNotFoundError& e) {
		CHECK(e.pluginName() == "Unknown");
		CHECK_THAT(e.availablePlugins(),
		           Catch::Matchers::UnorderedEquals(std::vector<std::string>{"DummyPluginA", "DummyPluginB"}));
	}
}

TEST_CASE("create() returns a new instance on every call", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);

	std::unique_ptr<DummyPluginBase> first(factory.create("DummyPluginA"));
	std::unique_ptr<DummyPluginBase> second(factory.create("DummyPluginA"));

	REQUIRE(first != nullptr);
	REQUIRE(second != nullptr);
	CHECK(first.get() != second.get());
}

TEST_CASE("getPluginNames() lists all registered plugins", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);
	factory.registerPlugin(&DummyPluginB::createInstance);

	// order is unspecified
	CHECK_THAT(factory.getPluginNames(),
	           Catch::Matchers::UnorderedEquals(std::vector<std::string>{"DummyPluginA", "DummyPluginB"}));
}

TEST_CASE("Registering a name twice keeps the first plugin", "[PluginFactory]") {
	PluginFactory<DummyPluginBase> factory;
	factory.registerPlugin(&DummyPluginA::createInstance);

	factory.registerPlugin(&DummyPluginC::createInstance);

	CHECK(factory.getPluginNames() == std::vector<std::string>{"DummyPluginA"});

	std::unique_ptr<DummyPluginBase> plugin(factory.create("DummyPluginA"));
	REQUIRE(plugin != nullptr);
	CHECK(dynamic_cast<DummyPluginA*>(plugin.get()) != nullptr);
}

TEST_CASE("REGISTER_PLUGIN registers plugins in a derived factory", "[PluginFactory]") {
	DummyFactory factory;

	CHECK_THAT(factory.getPluginNames(),
	           Catch::Matchers::UnorderedEquals(std::vector<std::string>{"DummyPluginA", "DummyPluginB"}));

	std::unique_ptr<DummyPluginBase> plugin(factory.create("DummyPluginB"));
	REQUIRE(plugin != nullptr);
	CHECK(plugin->getPluginName() == "DummyPluginB");
}
