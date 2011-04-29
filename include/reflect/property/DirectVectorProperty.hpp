#ifndef REFLECT_PROPERTY_DIRECTVECTORPROPERTY_HPP_
#define REFLECT_PROPERTY_DIRECTVECTORPROPERTY_HPP_

#include <reflect/property/DirectVectorProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
DirectVectorProperty<ObjectType, MemberType>::DirectVectorProperty(MemberType ObjectType::*member)
    : mMember(member)
{
}

template<typename ObjectType, typename MemberType>
void DirectVectorProperty<ObjectType, MemberType>::SerializeItem(
	const void *in, void *out, unsigned index, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		reflector | (translucent_cast<const ObjectType *>(in)->*mMember)[index];
	}
	else
	{
		reflector | (translucent_cast<ObjectType *>(out)->*mMember)[index];
	}
}

template<typename ObjectType, typename MemberType>
unsigned DirectVectorProperty<ObjectType, MemberType>::Size(const void *object) const
{
	return unsigned((translucent_cast<const ObjectType *>(object)->*mMember).size());
}

template<typename ObjectType, typename MemberType>
bool DirectVectorProperty<ObjectType, MemberType>::Resize(void *object, unsigned size) const
{
	(translucent_cast<ObjectType *>(object)->*mMember).resize(size);
	return Size(object) == size;
}

template<typename ObjectType, typename MemberType>
bool DirectVectorProperty<ObjectType, MemberType>::Resizable(const void *) const
{
	return true;
}

template<typename ObjectType, typename MemberType>
Type *DirectVectorProperty<ObjectType, MemberType>::ItemType() const
{
	return TypeOf<typename MemberType::value_type>();
}

template<typename ObjectType, typename MemberType>
bool DirectVectorProperty<ObjectType, MemberType>::ReadData(const void *in, unsigned index, Variant &value) const
{
	return value.SetValue((translucent_cast<const ObjectType *>(in)->*mMember)[index]);
}

template<typename ObjectType, typename MemberType>
bool DirectVectorProperty<ObjectType, MemberType>::WriteData(void *out, unsigned index, const Variant &value) const
{
	return value.ReadValue((translucent_cast<ObjectType *>(out)->*mMember)[index]);
}


template<typename ObjectType, typename MemberType>
bool DirectVectorProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, unsigned index, Variant &value) const
{
	if(out)
	{
		return value.RefValue((translucent_cast<ObjectType *>(out)->*mMember)[index]);
	}
	else if(in)
	{
		return value.ConstRefValue((translucent_cast<const ObjectType *>(in)->*mMember)[index]);
	}
	else return false;
}

} }

#endif
