#include <reflect/serialize/CompositeDeserializer.h>
#include <reflect/Reflector.h>
#include <reflect/Property.h>

namespace reflect { namespace serialize {

CompositeDeserializer::CompositeDeserializer(Deserializer &deserializer)
	: mDeserializer(deserializer)
{
}

bool CompositeDeserializer::Begin(SerializationTag &tag, SerializationTag::TagType type)
{ return mDeserializer.Begin(tag, type); }

bool CompositeDeserializer::End(SerializationTag &tag)
{ return mDeserializer.End(tag); }

bool CompositeDeserializer::Deserialize(bool &value)
{ return mDeserializer.Deserialize(value); }

bool CompositeDeserializer::Deserialize(long &value)
{ return mDeserializer.Deserialize(value); }

bool CompositeDeserializer::Deserialize(unsigned long &value)
{ return mDeserializer.Deserialize(value); }

bool CompositeDeserializer::Deserialize(double &value)
{ return mDeserializer.Deserialize(value); }

bool CompositeDeserializer::Deserialize(Dynamic *&object)
{ return mDeserializer.Deserialize(object); }

bool CompositeDeserializer::Reference(Dynamic *object)
{ return mDeserializer.Reference(object); }

bool CompositeDeserializer::DeserializeTextChunk(char *text, unsigned &max_bytes, bool &complete)
{ return mDeserializer.DeserializeTextChunk(text, max_bytes, complete); }

bool CompositeDeserializer::DeserializeData(void *data, unsigned nbytes)
{ return mDeserializer.DeserializeData(data, nbytes); }

bool CompositeDeserializer::DeserializeEnum(int &value, const EnumType *clazz)
{ return mDeserializer.DeserializeEnum(value, clazz); }


bool CompositeDeserializer::DeserializeProperty(void *object, const Property *prop)
{ 
	Reflector reflector(*this);
	prop->Serialize(object, object, reflector);
	return reflector.Ok();
}


CompositeDeserializer::~CompositeDeserializer()
{
}

} } // namespace reflect::serialize
