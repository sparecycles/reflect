#ifndef REFLECT_PROPERTY_DIRECTARRAYPROPERTY_HPP_
#define REFLECT_PROPERTY_DIRECTARRAYPROPERTY_HPP_

#include <reflect/property/DirectArrayProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
DirectArrayProperty<ObjectType, MemberType>::DirectArrayProperty(MemberType (ObjectType::*member)[], unsigned size)
    : mMember(member)
	, mSize(size)
{
}

template<typename ObjectType, typename MemberType>
void DirectArrayProperty<ObjectType, MemberType>::SerializeItem(
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
unsigned DirectArrayProperty<ObjectType, MemberType>::Size(const void *) const
{
	return mSize;
}

template<typename ObjectType, typename MemberType>
bool DirectArrayProperty<ObjectType, MemberType>::Resize(void *, unsigned size) const
{
	return size == mSize;
}

template<typename ObjectType, typename MemberType>
bool DirectArrayProperty<ObjectType, MemberType>::Resizable(const void *) const
{
	return false;
}

template<typename ObjectType, typename MemberType>
Type *DirectArrayProperty<ObjectType, MemberType>::ItemType() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool DirectArrayProperty<ObjectType, MemberType>::ReadData(const void *obj, unsigned index, Variant &variant) const
{
	if(index < Size(obj))
	{
		return variant.SetValue((translucent_cast<const ObjectType *>(obj)->*mMember)[index]);
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
bool DirectArrayProperty<ObjectType, MemberType>::WriteData(void *obj, unsigned index, const Variant &variant) const
{
	if(index < Size(obj))
	{
		return variant.ReadValue((translucent_cast<ObjectType *>(obj)->*mMember)[index]);
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
bool DirectArrayProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, unsigned index, Variant &value) const
{
	if(index >= Size(in))
		return false;

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
