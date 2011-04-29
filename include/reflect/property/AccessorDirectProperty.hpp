#ifndef REFLECT_PROPERTY_ACCESSORDIRECTPROPERTY_HPP_
#define REFLECT_PROPERTY_ACCESSORDIRECTPROPERTY_HPP_

#include <reflect/property/AccessorDirectProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
AccessorDirectProperty<ObjectType, MemberType>::AccessorDirectProperty(const MemberType &(ObjectType::*get)() const, void (ObjectType::*set)(const MemberType &))
    : mGetter(get)
    , mSetter(set)
{
}

template<typename ObjectType, typename MemberType>
void AccessorDirectProperty<ObjectType, MemberType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
    if(reflector.Serializing())
    {
        reflector | (translucent_cast<const ObjectType *>(in)->*mGetter)();
    }
    else
    {
        MemberType t;
        reflector | t;
        (translucent_cast<ObjectType *>(out)->*mSetter)(t);
    }
}

template<typename ObjectType, typename MemberType>
Type *AccessorDirectProperty<ObjectType, MemberType>::DataType() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool AccessorDirectProperty<ObjectType, MemberType>::WriteData(
	void *object, const Variant &value) const
{
	if(value.template CanRefAs<MemberType>())
	{
		(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsConstRef<MemberType>());
		return true;
	}
	else if(value.template CanReadAs<MemberType>())
	{
		(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsValue<MemberType>());
		return true;
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
bool AccessorDirectProperty<ObjectType, MemberType>::ReadData(
	const void *object, Variant &value) const
{
	return value.SetValue((translucent_cast<const ObjectType *>(object)->*mGetter)());
}

template<typename ObjectType, typename MemberType>
bool AccessorDirectProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, Variant &value) const
{
	// there's no way of getting a non-const access to the value for this
	// property type.
	if(in)
	{
		return value.ConstRefValue((translucent_cast<const ObjectType *>(in)->*mGetter)());
	}
	else return false;
}

} }

#endif
