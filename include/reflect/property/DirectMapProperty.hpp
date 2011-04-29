#ifndef REFLECT_PROPERTY_DIRECTMAPPROPERTY_HPP_
#define REFLECT_PROPERTY_DIRECTMAPPROPERTY_HPP_

#include <reflect/property/DirectMapProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename ObjectType, typename MemberType>
DirectMapProperty<ObjectType, MemberType>::DirectMapProperty(MemberType ObjectType::*member)
    : mMember(member)
{
}

template<typename ObjectType, typename MemberType>
bool DirectMapProperty<ObjectType, MemberType>::ReadData(const void *in, const Variant &key, Variant &value) const
{
	const MemberType &map = translucent_cast<const ObjectType *>(in)->*mMember;
	typedef typename MemberType::key_type KeyType;
	typedef typename MemberType::mapped_type ValueType;
	
	typename MemberType::const_iterator it;
	
	if(key.CanConstRefAs<KeyType>()) it = map.find(key.AsConstRef<KeyType>());
	else if(key.CanReadAs<KeyType>()) it = map.find(key.AsValue<KeyType>());
	
	if(it == map.end())
		return false;
	
	return value.SetValue(it->second);
}

template<typename ObjectType, typename MemberType>
bool DirectMapProperty<ObjectType, MemberType>::WriteData(void *out, const Variant &key, const Variant &value) const
{
	MemberType &map = translucent_cast<ObjectType *>(out)->*mMember;
	typedef typename MemberType::key_type KeyType;
	typedef typename MemberType::mapped_type ValueType;

	if(key.CanReadAs<KeyType>())
	{
		ValueType &target = map[key.AsValue<KeyType>()];
		return value.ReadValue(target);
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
void DirectMapProperty<ObjectType, MemberType>::ForEachKey(const void *in, void (*callback)(const Variant &key, void *ctx), void *ctx) const
{
	const MemberType &map = translucent_cast<const ObjectType *>(in)->*mMember;

	for(typename MemberType::const_iterator it = map.begin(); 
		it != map.end();
		++it)
	{
		(*callback)(Variant::FromConstRef(it->first), ctx);
	}
}

template<typename ObjectType, typename MemberType>
bool DirectMapProperty<ObjectType, MemberType>::HasKey(const void *in, const Variant &key) const
{
	const MemberType &map = translucent_cast<const ObjectType *>(in)->*mMember;
	typedef typename MemberType::key_type KeyType;
	
	if(key.CanConstRefAs<KeyType>())
	{
		return map.find(key.AsConstRef<KeyType>()) != map.end();
	}
	else if(key.CanReadAs<KeyType>())
	{
		return map.find(key.AsValue<KeyType>()) != map.end();
	}
	
	return false;
}

template<typename ObjectType, typename MemberType>
const Type *DirectMapProperty<ObjectType, MemberType>::KeyType() const
{
	return TypeOf<typename MemberType::key_type>();
}


template<typename ObjectType, typename MemberType>
const Type *DirectMapProperty<ObjectType, MemberType>::ValueType() const
{
	return TypeOf<typename MemberType::mapped_type>();
}

template<typename ObjectType, typename MemberType>
bool DirectMapProperty<ObjectType, MemberType>::RefData(
	const void *in, void *out, const Variant &key, Variant &value) const
{
	typedef typename MemberType::key_type KeyType;

	if(out)
	{
		MemberType &map = translucent_cast<ObjectType *>(out)->*mMember;
		
		typename MemberType::const_iterator it;
		
		if(key.CanConstRefAs<KeyType>()) return value.RefValue(map[key.AsConstRef<KeyType>()]);
		else if(key.CanReadAs<KeyType>()) return value.RefValue(map[key.AsValue<KeyType>()]);
		else return false;
	}
	else if(in)
	{
		const MemberType &map = translucent_cast<const ObjectType *>(in)->*mMember;
		
		typename MemberType::const_iterator it;
		
		if(key.CanConstRefAs<KeyType>()) it = map.find(key.AsConstRef<KeyType>());
		else if(key.CanReadAs<KeyType>()) it = map.find(key.AsValue<KeyType>());

		if(it == map.end())
			return false;

		return value.ConstRefValue(it->second);
	}
	else return false;
}

} }

#endif
