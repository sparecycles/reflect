#ifndef REFLECT_AUTOCAST_H_
#define REFLECT_AUTOCAST_H_

#include <reflect/Class.h>
#include <reflect/Dynamic.h>

namespace reflect {

class AutoCaster
{
public:
    AutoCaster(Dynamic *object)
        : mObject(object) 
    {}

    template<typename T> operator T *()
    {
        return static_cast<T *>(T::TheClass()->Cast(mObject));
    }
private:
    Dynamic *mObject;
};

class AutoCasterConst
{
public:
    AutoCasterConst(const Dynamic *object)
        : mObject(object) 
    {}

    template<typename T> operator const T *()
    {
        return static_cast<const T *>(T::TheClass()->Cast(mObject));
    }
private:
    const Dynamic *mObject;
};

// Variable: autocast
//
// Provides an easy way to downcast Dynamics
// without having to specify the downcast target type, and with minimal
// interruption to the legible program flow.
//
// Usage:
//
// > Type *typed_ptr = dynamic_ptr % autocast;
//
// With template functions like static_cast or dynamic_cast, the target type
// must be specified twice, i.e.,
//
// > MyClass *object = dynamic_cast<MyClass *>(ptr);
//
// "ptr % autocast" returns an object with a template<T> operator T*(), so the target
// type only needs to be specified once, and the intent of "object = dynamic" is
// not as interrupted up with silly syntax.
static const struct autocast_token { autocast_token(){} } autocast;

static inline AutoCaster operator %(Dynamic *object, const autocast_token &)
{
	return AutoCaster(object);
}

static inline AutoCasterConst operator %(const Dynamic *object, const autocast_token &)
{
	return AutoCasterConst(object);
}

}

#endif
