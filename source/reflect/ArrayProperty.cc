#include <reflect/ArrayProperty.h>
#include <reflect/Reflector.h>
#include <reflect/Serializer.h>
#include <reflect/Deserializer.h>
#include <reflect/SerializationTag.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/Class.hpp>

DEFINE_REFLECTION(reflect::ArrayProperty, "reflect::ArrayProperty")
{}

namespace reflect {

class Reflector;

void ArrayProperty::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		int size = int(Size(in));

		Serializer &serializer(reflector);

		if(Resizable(in))
		{
			SerializationTag tag("size", SerializationTag::AttributeTag);
			if(reflector.Check(serializer.Begin(tag)))
			{
				reflector | size;
				reflector.Check(serializer.End(tag));
			}
		}

		for(unsigned index = 0; index < unsigned(size); ++index)
		{
			SerializeItem(in, out, index, reflector);
		}
	}
	else
	{
		unsigned size = Size(in);
		SerializationTag tag;
		Deserializer &deserializer(reflector);

		bool has_attribute_tag = deserializer.Begin(tag, SerializationTag::AttributeTag);

		reflector.Check(has_attribute_tag || !Resizable(in));

		if(has_attribute_tag)
		{
			if(reflector.Check(tag == SerializationTag("size", SerializationTag::AttributeTag)))
			{
				reflector | size;
			}
			reflector.Check(deserializer.End(tag));
		}

		reflector.Check(Resize(out, size));

		if(reflector) for(unsigned index = 0; index < size; ++index)
		{
			SerializeItem(in, out, index, reflector);
		}
	}
}

}
