#include <reflect/serialize/CompositeSerializer.h>
#include <reflect/Reflector.h>
#include <reflect/Property.h>

namespace reflect { namespace serialize {

CompositeSerializer::CompositeSerializer(Serializer &serializer)
	: mSerializer(serializer)
{
}

bool CompositeSerializer::Begin(const SerializationTag &tag)
{ return mSerializer.Begin(tag); }

bool CompositeSerializer::End(const SerializationTag &tag)
{ return mSerializer.End(tag); }

bool CompositeSerializer::Serialize(bool value)
{ return mSerializer.Serialize(value); }

bool CompositeSerializer::Serialize(long value)
{ return mSerializer.Serialize(value); }

bool CompositeSerializer::Serialize(unsigned long value)
{ return mSerializer.Serialize(value); }

bool CompositeSerializer::Serialize(double value)
{ return mSerializer.Serialize(value); }

bool CompositeSerializer::Serialize(const Dynamic *object)
{ return mSerializer.Serialize(object); }

bool CompositeSerializer::Reference(const Dynamic *object)
{ return mSerializer.Reference(object); }

bool CompositeSerializer::SerializeText(const char *text, unsigned nbytes)
{ return mSerializer.SerializeText(text, nbytes); }

bool CompositeSerializer::SerializeData(const void *data, unsigned nbytes)
{ return mSerializer.SerializeData(data, nbytes); }

bool CompositeSerializer::SerializeEnum(int value, const EnumType *clazz)
{ return mSerializer.SerializeEnum(value, clazz); }

bool CompositeSerializer::SerializeProperty(const void *object, const Property *prop)
{ 
	Reflector reflector(*this);
	prop->Serialize(object, 0, reflector);
	return reflector.Ok();
}

CompositeSerializer::~CompositeSerializer()
{
}

} } // namespace reflect::serialize
