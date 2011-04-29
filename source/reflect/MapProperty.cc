#include <reflect/MapProperty.h>
#include <reflect/Class.hpp>
#include <reflect/SerializationTag.h>
#include <reflect/Reflector.h>
#include <cstdio>

DEFINE_REFLECTION(reflect::MapProperty, "reflect::MapProperty")
{
}

namespace reflect {

void MapProperty::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		struct MapReader
		{
			static void Callback(const Variant &key, void *opaque)
			{
				const MapReader *reader = static_cast<MapReader *>(opaque);
				reader->DoSerialization(key);
			}

			void DoSerialization(const Variant &key) const
			{
				SerializationTag item_tag(0, SerializationTag::ItemTag);
				SerializationTag key_tag("key", SerializationTag::AttributeTag);
				
				mReflector.Check(mReflector.GetSerializer().Begin(item_tag));

				mReflector.Check(mReflector.GetSerializer().Begin(key_tag));
				key.Serialize(mReflector);
				mReflector.Check(mReflector.GetSerializer().End(key_tag));

				Variant value;
				mReflector.Check(mProperty->ReadData(mObject, key, value));
				value.Serialize(mReflector);

				mReflector.Check(mReflector.GetSerializer().End(item_tag));
			}

			MapReader(const MapProperty *prop, const void *object, Reflector &reflector)
				: mProperty(prop)
				, mObject(object)
				, mReflector(reflector)
			{}

			const MapProperty *mProperty;
			const void *mObject;
			Reflector &mReflector;
		} mapReader(this, in, reflector);

		ForEachKey(in, MapReader::Callback, &mapReader);
	}
	else
	{
		SerializationTag item_tag;

		while(reflector.GetDeserializer().Begin(item_tag, SerializationTag::ItemTag))
		{
			Variant key;
			key.Construct(KeyType());
			
			SerializationTag key_tag;
			if(reflector.GetDeserializer().Begin(key_tag, SerializationTag::AttributeTag))
			{
				key.Serialize(reflector);
				reflector.Check(reflector.GetDeserializer().End(key_tag));
				Variant value;
				reflector.Check(value.Construct(ValueType()));
				reflector | value;
				reflector.Check(WriteData(out, key, value));
			}
			else
			{
				reflector.Fail();
			}

			reflector.Check(reflector.GetDeserializer().End(item_tag));
		}
	}
}

}
