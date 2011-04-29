#ifndef REFLECT_PROPERTY_VARIANTPROPERTY_HPP_
#define REFLECT_PROPERTY_VARIANTPROPERTY_HPP_

#include <reflect/property/VariantProperty.h>

namespace reflect { class Reflector; }

namespace reflect { namespace property {

template<typename ObjectType>
VariantProperty<ObjectType>::VariantProperty(void (ObjectType::*serializer)(Reflector &) const, void (ObjectType::*deserializer)(Reflector &))
	: mSerializer(serializer)
	, mDeserializer(deserializer)
{
}

template<typename ObjectType>
void VariantProperty<ObjectType>::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		const ObjectType *object = translucent_cast<const ObjectType *>(in);
		(object->*mSerializer)(reflector);
	}
	else
	{
		ObjectType *object = translucent_cast<ObjectType *>(out);
		(object->*mDeserializer)(reflector);
	}
}

} }

#endif
