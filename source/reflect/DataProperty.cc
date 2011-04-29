#include <reflect/DataProperty.h>
#include <reflect/Class.hpp>

DEFINE_REFLECTION(reflect::DataProperty, "reflect::DataProperty")
{
}

namespace reflect {

void DataProperty::Serialize(const void *in, void *out, reflect::Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		Variant value;
		reflector.Check(ReadData(in, value));
		value.Serialize(reflector);
	}
	else
	{
		Variant value;
		reflector.Check(value.Construct(DataType()));
		if(reflector)
			value.Serialize(reflector);
		if(reflector)
			reflector.Check(WriteData(out, value));
	}
}

}
