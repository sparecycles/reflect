#ifndef REFLECT_PROPERTY_ACCESSORINDIRECTPROPERTY_HPP_
#define REFLECT_PROPERTY_ACCESSORINDIRECTPROPERTY_HPP_

#include <reflect/property/AccessorIndirectProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
AccessorIndirectProperty<ObjectType, MemberType>::AccessorIndirectProperty(void (ObjectType::*get)(MemberType &) const, void (ObjectType::*set)(const MemberType &))
    : mGetter(get)
    , mSetter(set)
{
}

template<typename ObjectType, typename MemberType>
void AccessorIndirectProperty<ObjectType, MemberType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
    if(reflector.Serializing())
    {
		MemberType data;
		(translucent_cast<const ObjectType *>(in)->*mGetter)(data);
		reflector | data;
    }
    else
    {
        MemberType t;
        reflector | t;
        (translucent_cast<ObjectType *>(out)->*mSetter)(t);
    }
}

template<typename ObjectType, typename MemberType>
Type *AccessorIndirectProperty<ObjectType, MemberType>::DataType() const
{
	return TypeOf<MemberType>();
}

template<typename ObjectType, typename MemberType>
bool AccessorIndirectProperty<ObjectType, MemberType>::WriteData(
	void *object, const Variant &value) const
{
	if(value.CanReadAs<MemberType>())
	{
		if(value.CanRefAs<MemberType>())
			(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsConstRef<MemberType>());
		else
			(translucent_cast<ObjectType *>(object)->*mSetter)(value.AsValue<MemberType>());
		
		return true;
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
bool AccessorIndirectProperty<ObjectType, MemberType>::ReadData(
	const void *object, Variant &value) const
{
	Variant ref_variant;
	
	ref_variant.BindType<MemberType>();
	if(ref_variant.Alias(value))
	{
		(translucent_cast<const ObjectType *>(object)->*mGetter)(ref_variant.AsRef<MemberType>());
		return true;
	}
	
	return false;	
}

template<typename ObjectType, typename MemberType>
bool AccessorIndirectProperty<ObjectType, MemberType>::RefData(
	const void *, void *, Variant &) const
{
	return false;
}

} }

#endif
