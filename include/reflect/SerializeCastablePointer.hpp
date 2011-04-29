#ifndef _REFLECT_SERIALIZECASTABLEPOINTER_HPP_
#define _REFLECT_SERIALIZECASTABLEPOINTER_HPP_

#include <reflect/Castable.h>
#include <reflect/Reflector.h>
#include <reflect/Serializer.h>
#include <reflect/Deserializer.h>

namespace reflect {

template <typename Type>
static inline void SerializeCastablePointer(Type *const *in, Type **out, Reflector &reflector)
{
    if(reflector.Serializing())
    {
        reflector.Check(reflector.GetSerializer().Serialize(*in));
    }
    else
    {
        Castable *result = 0;
        reflector.Check(reflector.GetDeserializer().Deserialize(result));
        *out = static_cast<Type *>(Type::TheClass()->Cast(result));
    }
}

}

#endif
