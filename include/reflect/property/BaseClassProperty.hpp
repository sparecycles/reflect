#ifndef REFLECT_PROPERTY_BASECLASSPROPERTY_HPP_
#define REFLECT_PROPERTY_BASECLASSPROPERTY_HPP_

#include <reflect/property/BaseClassProperty.h>
#include <reflect/Reflector.h>

namespace reflect { namespace property {

template<typename BaseObjectType>
BaseClassProperty<BaseObjectType>::BaseClassProperty(BaseObjectType *(*base_jumper)(void *))
	: mBaseJumper(base_jumper)
{
}

template<typename BaseObjectType>
void BaseClassProperty<BaseObjectType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
	const BaseObjectType *bin = (*mBaseJumper)(const_cast<void *>(in));
	BaseObjectType *bout = (*mBaseJumper)(out);
	
	TypeOf<BaseObjectType>()->Serialize(
		opaque_cast(bin),
		opaque_cast(bout),
		reflector);
}

template<typename BaseObjectType>
Type *BaseClassProperty<BaseObjectType>::DataType() const
{
	return TypeOf<BaseObjectType>();
}

template<typename BaseObjectType>
bool BaseClassProperty<BaseObjectType>::WriteData(void *object, const Variant &value) const
{
	BaseObjectType *bout = (*mBaseJumper)(object);
	return value.ReadValue(*bout);
}

template<typename BaseObjectType>
bool BaseClassProperty<BaseObjectType>::ReadData(const void *object, Variant &value) const
{
	const BaseObjectType *bin = (*mBaseJumper)(const_cast<void *>(object));
	return value.SetValue(*bin);
}

template<typename BaseObjectType>
bool BaseClassProperty<BaseObjectType>::RefData(
	const void *in, void *out, Variant &value) const
{
	if(out)
	{
		BaseObjectType *bout = (*mBaseJumper)(out);
		return value.RefValue(*bout);
	}
	else if(in)
	{
		BaseObjectType *bin = (*mBaseJumper)(const_cast<void *>(in));
		return value.ConstRefValue(*bin);
	}
	else return false;
}

} }

#endif
