#include <reflect/PrimitiveTypes.h>
#include <reflect/Reflector.h>
#include <reflect/PrimitiveType.hpp>
#include <reflect/string/ConstString.h>
#include <reflect/string/SharedString.h>

namespace reflect { namespace internals {

void VoidSerialize(const void *, void *, reflect::Reflector &)
{
}

} }

DEFINE_STATIC_REFLECTION(void, "void")
{
	SerializeWith<&reflect::internals::VoidSerialize>();
	reflect::TypeOf<void>()->SetParent(0);
}

DEFINE_STATIC_REFLECTION(bool, "bool")
{
	+ Concrete;

	ConversionFrom<char>();
	ConversionFrom<signed char>();
	ConversionFrom<unsigned char>();
	ConversionFrom<signed short>();
	ConversionFrom<unsigned short>();
	ConversionFrom<signed int>();
	ConversionFrom<unsigned int>();
	ConversionFrom<signed long>();
	ConversionFrom<unsigned long>();

	SerializeAs<bool>();
}

DEFINE_STATIC_REFLECTION(char, "char")
{
	+ ConcreteNumeric;

	SerializeAs<long>();
}

DEFINE_STATIC_REFLECTION(signed char, "signed char")
{
	+ ConcreteNumeric;

	SerializeAs<long>();
}

DEFINE_STATIC_REFLECTION(unsigned char, "unsigned char")
{
	+ ConcreteNumeric;

	SerializeAs<unsigned long>();
}

DEFINE_STATIC_REFLECTION(signed short, "short")
{
	+ ConcreteNumeric;

	SerializeAs<long>();
}

DEFINE_STATIC_REFLECTION(unsigned short, "unsigned short")
{
	+ ConcreteNumeric;

	SerializeAs<unsigned long>();
}

DEFINE_STATIC_REFLECTION(signed int, "int")
{
	+ ConcreteNumeric;

	SerializeAs<long>();
}

DEFINE_STATIC_REFLECTION(unsigned int, "unsigned int")
{
	+ ConcreteNumeric;

	SerializeAs<unsigned long>();
}

DEFINE_STATIC_REFLECTION(signed long, "long")
{
	+ ConcreteNumeric;

	SerializeAs<long>();
}

DEFINE_STATIC_REFLECTION(unsigned long, "unsigned long")
{
	+ ConcreteNumeric;

	SerializeAs<unsigned long>();
}

DEFINE_STATIC_REFLECTION(float, "float")
{
	+ ConcreteNumeric;

	SerializeAs<double>();
}

DEFINE_STATIC_REFLECTION(double, "double")
{
	+ ConcreteNumeric;

	SerializeAs<double>();
}

DEFINE_STATIC_REFLECTION(const char *, "c_string*")
{
	+ Concrete;
	
	struct Operations
	{
		static void Serialize(const void *in, void *out, reflect::Reflector &reflector)
		{
			if(reflector.Serializing())
			{
				reflect::string::ConstString value(*reflect::translucent_cast<const char *const *>(in));
				reflector | value;
			}
			else
			{
				reflect::string::SharedString value;
				reflector | value;
				*reflect::translucent_cast<const char **>(out) = value.c_str();
			}
		}
	};
	
	reflect::TypeOf<const char *>()->SetSerializer(&Operations::Serialize);
}
