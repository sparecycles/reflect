#ifndef REFLECT_SERIALIZE_COMPOSITESERIALIZER_H_
#define REFLECT_SERIALIZE_COMPOSITESERIALIZER_H_

#include <reflect/Serializer.h>

namespace reflect { namespace serialize {

// Class: CompositeSerializer
// 
// Base class for implementing one serializer in terms of another.
// For example <ShallowSerializer> which overrides the 
// treatment of pointers.
class ReflectExport(reflect) CompositeSerializer
	: public Serializer
{
public:
	CompositeSerializer(Serializer &serializer);
	
	/*virtual*/ bool Begin(const SerializationTag &tag);
    /*virtual*/ bool End(const SerializationTag &);
    /*virtual*/ bool Serialize(bool);
    /*virtual*/ bool Serialize(long);
    /*virtual*/ bool Serialize(unsigned long);
    /*virtual*/ bool Serialize(double);
    /*virtual*/ bool Serialize(const Dynamic *object);
    /*virtual*/ bool Reference(const Dynamic *object);
    /*virtual*/ bool SerializeText(const char *text, unsigned nbytes);
    /*virtual*/ bool SerializeData(const void *data, unsigned nbytes);
    /*virtual*/ bool SerializeEnum(int value, const EnumType *);
	/*virtual*/ bool SerializeProperty(const void *object, const Property *prop);

protected:
    ~CompositeSerializer();
	Serializer &mSerializer;
};

} } // namespace reflect::serialize

#endif
