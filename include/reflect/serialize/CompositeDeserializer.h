#ifndef REFLECT_SERIALIZE_COMPOSITEDESERIALIZER_H_
#define REFLECT_SERIALIZE_COMPOSITEDESERIALIZER_H_

#include <reflect/Deserializer.h>

namespace reflect { namespace serialize {

// Class: CompositeDeserializer
// 
// Base class for implementing one serializer in terms of another.
// For example <ShallowDeserializer> which overrides the 
// treatment of pointers.
class ReflectExport(reflect) CompositeDeserializer
	: public Deserializer
{
public:
	CompositeDeserializer(Deserializer &);
	
    /*virtual*/ bool Begin(SerializationTag &, SerializationTag::TagType = SerializationTag::UnknownTag);
    /*virtual*/ bool End(SerializationTag &);
    /*virtual*/ bool Deserialize(bool &);
    /*virtual*/ bool Deserialize(long &);
    /*virtual*/ bool Deserialize(unsigned long &);
    /*virtual*/ bool Deserialize(double &);
    /*virtual*/ bool Deserialize(Dynamic *&object);
    /*virtual*/ bool Reference(Dynamic *object);
    /*virtual*/ bool DeserializeTextChunk(char *text, unsigned &max_bytes, bool &complete);
    /*virtual*/ bool DeserializeData(void *data, unsigned nbytes);
    /*virtual*/ bool DeserializeEnum(int &value, const EnumType *);
	/*virtual*/ bool DeserializeProperty(void *object, const Property *prop);

protected:
	Deserializer &mDeserializer;

	~CompositeDeserializer();
};

} }

#endif
