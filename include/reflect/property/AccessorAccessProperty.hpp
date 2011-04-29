#ifndef REFLECT_PROPERTY_ACCESSORACCESSPROPERTY_HPP_
#define REFLECT_PROPERTY_ACCESSORACCESSPROPERTY_HPP_

#include <reflect/property/AccessorAccessProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
AccessorAccessProperty<ObjectType, MemberType>::AccessorAccessProperty(const MemberType &(ObjectType::*get)() const, MemberType &(ObjectType::*set)())
    : mConstAccess(get)
    , mMutableAccess(set)
{
}

template<typename ObjectType, typename MemberType>
void AccessorAccessProperty<ObjectType, MemberType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
    if(reflector.Serializing())
    {
        reflector | (translucent_cast<const ObjectType *>(in)->*mConstAccess)();
    }
    else
    {
        MemberType t;
        reflector | t;
        (translucent_cast<ObjectType *>(out)->*mMutableAccess)() = t;
    }
}

template<typename ObjectType, typename MemberType>
Class *AccessorAccessProperty<ObjectType, MemberType>::DataClass() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool AccessorAccessProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, Variant &value) const
{
	if(out)
	{
		return value.RefValue((translucent_cast<ObjectType *>(out)->*mMutableAccess)());
	}
	else if(in)
	{
		return value.ConstRefValue((translucent_cast<const ObjectType *>(in)->*mConstAccess)());
	}
	else return false;
}

template<typename ObjectType, typename MemberType>
bool AccessorAccessProperty<ObjectType, MemberType>::WriteData(
	void *object, const Variant &value) const
{
	return value.ReadValue((translucent_cast<ObjectType *>(object)->*mMutableAccess)());
}

template<typename ObjectType, typename MemberType>
bool AccessorAccessProperty<ObjectType, MemberType>::ReadData(
	const void *object, Variant &value) const
{
	return value.SetValue((translucent_cast<const ObjectType *>(object)->*mConstAccess)());
}

} }

#endif
