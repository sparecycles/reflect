#ifndef REFLECT_SERIALIZE_STANDARDDESERIALIZER_H_
#define REFLECT_SERIALIZE_STANDARDDESERIALIZER_H_

#include <reflect/Deserializer.h>
#include <vector>

#include <reflect/config/config.h>

namespace reflect {
class InputStream;
}

namespace reflect { namespace string {
class MutableString; 
} }

namespace reflect { namespace serialize {

// Class: StandardDeserializer
//     A basic ascii text deserializer.
//
// See Also:
//     - <Deserializer>
//     - <StandardSerializer>
class ReflectExport(reflect) StandardDeserializer : public Deserializer
{
public:
    StandardDeserializer(InputStream &);

protected:
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
	/*virtual*/ bool DeserializeEnum(int &value, const EnumType *clazz);
	/*virtual*/ bool DeserializeProperty(void *object, const Property *prop);

private:
	std::vector<Dynamic *> mReferenced;
	InputStream &mStream;
	char mPeekChar;
	char mDeserializingText;

	char Peek();
	char Read();
	void EatSpace();
	int ReadWord(string::MutableString);
	SerializationTag *mCurrentTag;
};

} }

#endif
