#include <reflect/string/ConstString.h>
#include <reflect/string/SharedString.h>
#include <reflect/string/Fragment.h>
#include <reflect/Reflector.h>
#include <reflect/PrimitiveType.hpp>
#include <reflect/PrimitiveTypes.h>
#include <cstring>

namespace reflect { namespace string {

ConstString::ConstString(const char *s)
	: mpString(s ? s : "")
	, mLength(size_type(std::strlen(mpString)))
{
}

ConstString::ConstString()
	: mpString("")
	, mLength(0)
{
}

const char *ConstString::c_str() const { return mpString; }
const char *ConstString::data() const { return mpString; }
ConstString::size_type ConstString::length() const { return mLength; }
ConstString::size_type ConstString::size() const { return mLength; }

///////////////////////////////////////////

ConstString::size_type ConstString::find(char ch, ConstString::size_type pos) const 
{
	return Fragment(*this).find(ch, pos);
}

ConstString::size_type ConstString::find(const char *s, ConstString::size_type pos, ConstString::size_type len) const 
{
	return Fragment(*this).find(s, pos, len);
}

ConstString::size_type ConstString::find(const Fragment &s, ConstString::size_type pos) const 
{
	return Fragment(*this).find(s, pos);
}

ConstString::size_type ConstString::find_first_of(char ch, ConstString::size_type pos) const 
{
	return Fragment(*this).find_first_of(ch, pos);
}

ConstString::size_type ConstString::find_first_of(const Fragment &s, ConstString::size_type pos) const 
{
	return Fragment(*this).find_first_of(s, pos);
}

ConstString::size_type ConstString::rfind(char ch, ConstString::size_type pos) const 
{
	return Fragment(*this).rfind(ch, pos);
}

ConstString::size_type ConstString::rfind(const char *s, ConstString::size_type pos, ConstString::size_type len) const 
{
	return Fragment(*this).rfind(s, pos, len);
}

ConstString::size_type ConstString::rfind(const Fragment &s, ConstString::size_type pos) const 
{
	return Fragment(*this).rfind(s, pos);
}

ConstString::size_type ConstString::find_last_of(char ch, ConstString::size_type pos) const 
{
	return Fragment(*this).find_last_of(ch, pos);
}

ConstString::size_type ConstString::find_last_of(const Fragment &s, ConstString::size_type pos) const 
{
	return Fragment(*this).find_last_of(s, pos);
}

Fragment ConstString::substr(ConstString::size_type index, ConstString::size_type amount) const 
{
	return Fragment(*this).substr(index, amount);
}

bool ConstString::empty() const 
{
	return 0 == size();
}

ConstString::operator Fragment() const 
{
	return Fragment(data(), length());
}

bool ConstString::operator ==(const Fragment &other) const 
{
	return Fragment(*this) == other;
}

bool ConstString::operator <=(const Fragment &other) const 
{
	return Fragment(*this) <= other;
}

bool ConstString::operator >=(const Fragment &other) const 
{
	return Fragment(*this) >= other;
}

bool ConstString::operator < (const Fragment &other) const 
{
	return Fragment(*this) <  other;
}

bool ConstString::operator > (const Fragment &other) const 
{
	return Fragment(*this) >  other;
}

bool ConstString::operator !=(const Fragment &other) const 
{
	return Fragment(*this) != other;
}

ConstString::const_iterator ConstString::begin() const
{
	return data();
}

ConstString::const_iterator ConstString::end() const
{
	return data() + size();
}

namespace {
void SerializeConstString(const ConstString *in, ConstString *out, Reflector &reflector)
{
	if(reflector.Serializing())
	{
		reflector | string::Fragment(*in);
	}
	else
	{
		string::SharedString pool_string;
		reflector | pool_string;
		*out = pool_string.c_str();
	}
}

}

} }



DEFINE_STATIC_REFLECTION(reflect::string::ConstString, "const_string")
{
	+ Concrete;
	+ ConversionFrom<const char *>();

	SerializeWith<&reflect::string::SerializeConstString>();
}
