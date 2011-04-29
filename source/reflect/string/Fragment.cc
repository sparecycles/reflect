#include <reflect/string/Fragment.h>
#include <reflect/string/ConstString.h>
#include <reflect/string/SharedString.h>
#include <reflect/Reflector.h>
#include <reflect/Serializer.h>
#include <reflect/PrimitiveType.hpp>
#include <reflect/PrimitiveTypes.h>

#include <cstring>

namespace reflect { namespace string {

Fragment::Fragment(const char *s)
	: mpString(s ? s : "")
	, mLength(size_type(std::strlen(mpString)))
{
}

Fragment::Fragment(const char *s, Fragment::size_type length)
	: mpString(s)
	, mLength(length)
{
}

Fragment::Fragment()
	: mpString(NULL)
	, mLength(0)
{
}

const char *Fragment::data() const { return mpString; }
Fragment::size_type Fragment::length() const { return mLength; }
Fragment::size_type Fragment::size() const { return mLength; }

Fragment::const_iterator Fragment::begin() const { return data(); }
Fragment::const_iterator Fragment::end() const { return data() + size(); }

bool Fragment::empty() const { return 0 == size(); }
	
bool Fragment::operator ==(const Fragment &other) const { return compare(other) == 0; }
bool Fragment::operator <=(const Fragment &other) const { return compare(other) <= 0; }
bool Fragment::operator >=(const Fragment &other) const { return compare(other) >= 0; }
bool Fragment::operator < (const Fragment &other) const { return compare(other) <  0; }
bool Fragment::operator > (const Fragment &other) const { return compare(other) >  0; }
bool Fragment::operator !=(const Fragment &other) const { return compare(other) != 0; }

Fragment Fragment::substr(Fragment::size_type index, Fragment::size_type amount) const
{
	if(length() < index || index == npos) index = length();
	if(npos == amount || amount > length() - index) amount = length() - index;
	return Fragment(data() + index, amount);
}

Fragment::size_type Fragment::find(char ch, Fragment::size_type pos) const
{
	while(pos < length())
	{
		if(mpString[pos] == ch) return pos;
		pos++;
	}

	return npos;
}

Fragment::size_type Fragment::find(const char *s, Fragment::size_type pos, Fragment::size_type len) const
{
	if(len == npos) len = size_type(std::strlen(s));
	while(pos < length())
	{
		if(std::strncmp(mpString + pos, s, len) == 0) return pos;
		pos++;
	}
	return npos;
}

Fragment::size_type Fragment::find(const Fragment &s, Fragment::size_type pos) const
{
	return find(s.data(), pos, s.length());
}

Fragment::size_type Fragment::rfind(const Fragment &s, Fragment::size_type pos) const
{
	return rfind(s.data(), pos, s.length());
}

Fragment::size_type Fragment::rfind(char ch, Fragment::size_type pos) const
{
	if(empty()) return npos;

	if(npos == pos)
	{
		pos = length() - 1;
	}

	do
	{
		if(mpString[pos] == ch) return pos;
		pos--;
	}
	while(pos != 0);

	return npos;
}

Fragment::size_type Fragment::rfind(const char *s, Fragment::size_type pos, Fragment::size_type len) const
{
	if(empty()) return npos;

	if(npos == pos)
	{
		pos = length() - 1;
	}

	if(len == npos) len = size_type(std::strlen(s));

	do
	{
		if(std::strncmp(mpString + pos, s, len) == 0) return pos;
		pos--;
	}
	while(pos != 0);

	return npos;
}

Fragment::size_type Fragment::find_first_of(char ch, Fragment::size_type pos) const
{
	return find(ch, pos);
}

Fragment::size_type Fragment::find_last_of(char ch, Fragment::size_type pos) const
{
	return rfind(ch, pos);
}

Fragment::size_type Fragment::find_first_of(const Fragment &s, Fragment::size_type pos) const
{
	while(pos < length())
	{
		if(std::memchr(s.data(), mpString[pos], s.size())) return pos;
		pos++;
	}
	return npos;
}

Fragment::size_type Fragment::find_last_of(const Fragment &s, Fragment::size_type pos) const
{
	if(npos == pos)
	{
		pos = length() - 1;
	}

	while(pos < size())
	{
		if(std::memchr(s.data(), mpString[pos], s.size())) return pos;
		pos--;
	}

	return npos;
}

int Fragment::compare(const Fragment &other) const
{
	size_type common_length = length() < other.length() ? length() : other.length();
	int compare = std::memcmp(data(), other.data(), common_length);
	if(compare == 0) compare = int(length() - other.length());
	return compare < 0 ? -1 : compare > 0 ? 1 : 0;
}

void Fragment::Serialize(const string::Fragment *in, string::Fragment *out, Reflector &reflector)
{
	if(reflector.Serializing())
	{
		reflector.Check(reflector.GetSerializer().SerializeText(in->data(), in->size()));
	}
	else
	{
		string::SharedString pool_string;
		reflector | pool_string;
		*out = pool_string.c_str();
	}
}

} }

DEFINE_STATIC_REFLECTION(reflect::string::Fragment, "fragment")
{
	SerializeWith<&reflect::string::Fragment::Serialize>();

	+ Concrete;
	+ ConversionFrom<const char *>();
	+ ConversionFrom<reflect::string::ConstString>();
}
