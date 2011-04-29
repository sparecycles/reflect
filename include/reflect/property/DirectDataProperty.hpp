#ifndef REFLECT_PROPERTY_DIRECTDATAPROPERTY_HPP_
#define REFLECT_PROPERTY_DIRECTDATAPROPERTY_HPP_

#include <reflect/property/DirectDataProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
DirectDataProperty<ObjectType, MemberType>::DirectDataProperty(MemberType ObjectType::*member)
    : mMember(member)
{
}

template<typename ObjectType, typename MemberType>
void DirectDataProperty<ObjectType, MemberType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
    reflector(translucent_cast<const ObjectType *>(in), translucent_cast<ObjectType *>(out)) | mMember;
}

template<typename ObjectType, typename MemberType>
Type *DirectDataProperty<ObjectType, MemberType>::DataType() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool DirectDataProperty<ObjectType, MemberType>::WriteData(void *out, const Variant &value) const
{
	MemberType &member = translucent_cast<ObjectType *>(out)->*mMember;
	return value.ReadValue(member);
}

template<typename ObjectType, typename MemberType>
bool DirectDataProperty<ObjectType, MemberType>::ReadData(const void *in, Variant &value) const
{
	const MemberType &member = translucent_cast<const ObjectType *>(in)->*mMember;
	return value.SetValue(member);
}

template<typename ObjectType, typename MemberType>
bool DirectDataProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, Variant &value) const
{
	if(out)
	{
		MemberType &member = translucent_cast<ObjectType *>(out)->*mMember;
		return value.RefValue(member);
	}
	else if(in)
	{
		const MemberType &member = translucent_cast<const ObjectType *>(in)->*mMember;
		return value.ConstRefValue(member);
	}
	else return false;
}

} }

#endif
