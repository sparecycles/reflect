#include <reflect/EnumType.hpp>
#include <reflect/Class.hpp>
#include <reflect/Property.h>
#include <reflect/string/String.h>

namespace reflect {

EnumType::EnumType(void (*init_cb)())
	: Type(init_cb)
{
}

void EnumType::AddValue(int value, const char *name)
{
	string::SharedString sname = string::SharedString::Literal(name);
	mValues.insert(std::make_pair(sname, value));
	mNames.insert(std::make_pair(value, sname));
}

void EnumType::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		int value = (*mReadValue)(in);
		reflector.Check(reflector.GetSerializer().SerializeEnum(value, this));
	}
	else
	{
		int value = 0;
		reflector.Check(reflector.GetDeserializer().DeserializeEnum(value, this));		
		mWriteValue(out, value);
	}
}

void EnumType::SetAccessors(int (*reader)(const void *), void (*writer)(void *, int i))
{
	mReadValue = reader;
	mWriteValue = writer;
}

int EnumType::ReadEnum(const void *object)
{
	if(mReadValue)
	{
		return (*mReadValue)(object);
	}
	else
	{
		return -1;
	}
}

void EnumType::WriteEnum(void *object, int value)
{
	if(mWriteValue)
		(*mWriteValue)(object, value);
}

}

DEFINE_REFLECTION(reflect::EnumType, "reflect::EnumType")
{
}
