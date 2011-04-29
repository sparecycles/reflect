#ifndef REFLECT_PROPERTY_ACCESSORPROPERTY_HPP_
#define REFLECT_PROPERTY_ACCESSORPROPERTY_HPP_

#include <reflect/property/AccessorProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
AccessorProperty<ObjectType, MemberType>::AccessorProperty(MemberType (ObjectType::*get)() const, void (ObjectType::*set)(MemberType))
    : mGetter(get)
    , mSetter(set)
{
}

template<typename ObjectType, typename MemberType>
void AccessorProperty<ObjectType, MemberType>::Serialize(const void *in, void *out, Reflector &reflector) const
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
Type *AccessorProperty<ObjectType, MemberType>::DataType() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool AccessorProperty<ObjectType, MemberType>::WriteData(
	void *object, const Variant &value) const
{
	if(value.CanRefAs<MemberType>())
	{
		(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsConstRef<MemberType>());
		return true;
	}
	else if(value.CanReadAs<MemberType>())
	{
		(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsValue<MemberType>());
		return true;
	}
	
	return false;
}


template<typename ObjectType, typename MemberType>
bool AccessorProperty<ObjectType, MemberType>::ReadData(
	const void *object, Variant &value) const
{
	return value.SetValue((translucent_cast<const ObjectType *>(object)->*mGetter)());
}

template<typename ObjectType, typename MemberType>
bool AccessorProperty<ObjectType, MemberType>::RefData(
	const void *, void *, Variant &) const
{
	return false;
}


} }

#endif
