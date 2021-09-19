#pragma once

#define $reflectable(self, base)                                                                  \
public:                                                                                           \
    static const rc::reflection::MetaObject &staticMetaObject() { return _metaObject; }           \
    virtual const rc::reflection::MetaObject &metaObject() const override { return _metaObject; } \
    static struct MetaObject : public rc::reflection::MetaObject                                  \
    {                                                                                             \
        using type = self;                                                                        \
        using base_type = base;                                                                   \
        MetaObject() : rc::reflection::MetaObject(#self, base::_metaObject)                       \
        {                                                                                         \
        }                                                                                         \
    } _metaObject;                                                                                \
                                                                                                  \
private:

#define $constructor(...) metaObject().addContructor<__VA_ARGS__>()
#define $method(name, method) metaObject().addMethod(name)
#define $property(name, getter, setter) metaObject().addMethod(name)
#define $event(name, ...) metaObject().addEvent<__VA_ARGS__>(name)
#define $annotation(name) metaObject().addAnnotation(name)