#pragma once

#include <stdexcept>

namespace rc::reflection
{
    class reflection_error : public std::runtime_error
    {
    public:
        reflection_error(std::string msg) : runtime_error("reflection error: " + msg) {}
    };

    class illegal_prototype : public reflection_error
    {
    public:
        illegal_prototype(std::string msg) : reflection_error("illegal prototype: " + msg) {}
    };

    class unknown_constructor : public reflection_error
    {
    public:
        unknown_constructor(std::string msg) : reflection_error("unknown constructor: " + msg) {}
    };

    class unknown_method : public reflection_error
    {
    public:
        unknown_method(std::string msg) : reflection_error("unknown method: " + msg) {}
    };

    class invalid_method_type : public reflection_error
    {
    public:
        invalid_method_type(std::string msg) : reflection_error("invalid method type: " + msg) {}
    };

    class unknown_property : public reflection_error
    {
    public:
        unknown_property(std::string msg) : reflection_error("unknown property: " + msg) {}
    };

    class invalid_property_type : public reflection_error
    {
    public:
        invalid_property_type(std::string msg) : reflection_error("invalid property type: " + msg) {}
    };

    class unknown_event : public reflection_error
    {
    public:
        unknown_event(std::string msg) : reflection_error("unknown event: " + msg) {}
    };

    class invalid_event_type : public reflection_error
    {
    public:
        invalid_event_type(std::string msg) : reflection_error("invalid event type: " + msg) {}
    };

    class unknown_type : public reflection_error
    {
    public:
        unknown_type(std::string msg) : reflection_error("unknown type: " + msg) {}
    };

    class registration_failed : public reflection_error
    {
    public:
        registration_failed(std::string msg) : reflection_error("registration failed: " + msg) {}
    };

    class instantiation_failed : public reflection_error
    {
    public:
        instantiation_failed(std::string msg) : reflection_error("instantiation failed: " + msg) {}
    };
}
