#include <reflect/string/String.h>
#include <reflect/Reflector.h>
#include <reflect/Deserializer.h>
#include <reflect/PrimitiveType.hpp>
#include <reflect/PrimitiveTypes.h>

#include <cstring>

namespace reflect { namespace string {

String::size_type String::format(const char *format, ...)
{
    va_list args;

    va_start(args, format);
	size_type result = vformat(format, args);
    va_end(args);

	return result;
}

} }
#include <cstdio>
namespace reflect { namespace string { 

String::size_type String::vformat(const char *format, va_list args)
{
	for(;;)
	{
#ifdef va_copy
		va_list args_copy;
		va_copy(args_copy, args);
		size_type formatsize = MutableString(*this).vformat(format, args_copy);
		va_end(args_copy);
#else
		size_type formatsize = MutableString(*this).vformat(format, args);
#endif

		if(formatsize == npos)
			formatsize = capacity()*2;

		if(formatsize > capacity())
		{
			resize(formatsize+1, 0, formatsize+1);
		}
		else
		{
			break;
		}
	}

	return size();
}

String String::formatted(const char *format, ...)
{
	String result;
	
	va_list args;
	va_start(args, format);
	result.vformat(format, args);
	va_end(args);

	return result;
}

String::String()
	: mpBuffer(NULL)
	, mLength(0)
	, mMaxSize(0)
{
}

String::String(const char *s)
	: mpBuffer(NULL)
	, mLength(0)
	, mMaxSize(0)
{
	copy(s, mLength = size_type(std::strlen(s)));
}

String::String(const Fragment &s)
	: mpBuffer(NULL)
	, mLength(0)
	, mMaxSize(0)
{
	copy(s.data(), mLength = s.length());
}

String::String(const String &s)
	: mpBuffer(NULL)
	, mLength(0)
	, mMaxSize(0)
{
	copy(s.data(), mLength = s.length());
}

void String::reserve(size_type sz)
{
	resize(sz, size(), sz);
}

String::operator MutableString() 
{ 
	resize(1, size(), 64);
	return MutableString(mpBuffer, capacity()+1, mLength); 
}

String::iterator String::begin() 
{ 
	return mpBuffer;
}

String::iterator String::end() 
{ 
	return mpBuffer + size(); 
}

String::const_iterator String::begin() const 
{ 
	return ConstString(*this).begin(); 
}

String::const_iterator String::end() const
{ 
	return ConstString(*this).end(); 
}

///////////////////////////////////////////////////////////////////////////////

String::size_type String::find(char ch, String::size_type pos) const 
{
	return ConstString(*this).find(ch, pos);
}

String::size_type String::find(const char *s, String::size_type pos, String::size_type len) const 
{
	return ConstString(*this).find(s, pos, len);
}

String::size_type String::find(const Fragment &s, String::size_type pos) const 
{
	return ConstString(*this).find(s, pos);
}

String::size_type String::find_first_of(char ch, String::size_type pos) const 
{
	return ConstString(*this).find_first_of(ch, pos);
}

String::size_type String::find_first_of(const Fragment &s, String::size_type pos) const 
{
	return ConstString(*this).find_first_of(s, pos);
}

String::size_type String::rfind(char ch, String::size_type pos) const 
{
	return ConstString(*this).rfind(ch, pos);
}

String::size_type String::rfind(const char *s, String::size_type pos, String::size_type len) const 
{
	return ConstString(*this).rfind(s, pos, len);
}

String::size_type String::rfind(const Fragment &s, String::size_type pos) const 
{
	return ConstString(*this).rfind(s, pos);
}

String::size_type String::find_last_of(char ch, String::size_type pos) const 
{
	return ConstString(*this).find_last_of(ch, pos);
}

String::size_type String::find_last_of(const Fragment &s, String::size_type pos) const 
{
	return ConstString(*this).find_last_of(s, pos);
}

Fragment String::substr(String::size_type index, String::size_type amount) const 
{
	return ConstString(*this).substr(index, amount);
}

bool String::empty() const 
{
	return 0 == size();
}

String::operator ConstString() const 
{
	return ConstString(c_str());
}

String::operator Fragment() const 
{
	return Fragment(c_str(), size());
}

bool String::operator ==(const Fragment &other) const 
{
	return ConstString(*this) == other;
}

bool String::operator <=(const Fragment &other) const 
{
	return ConstString(*this) <= other;
}

bool String::operator >=(const Fragment &other) const 
{
	return ConstString(*this) >= other;
}

bool String::operator < (const Fragment &other) const 
{
	return ConstString(*this) <  other;
}

bool String::operator > (const Fragment &other) const 
{
	return ConstString(*this) >  other;
}

bool String::operator !=(const Fragment &other) const 
{
	return ConstString(*this) != other;
}

void String::replace(size_type offset, size_type sz, Fragment value)
{
	if(offset + sz > length()) return;

	size_type required_size = length() - sz + value.length();
	
	resize(required_size, length(), required_size*2);

	MutableString(*this).replace(offset, sz, value);
}

const String &String::operator =(const String &other)
{
	return operator =(ConstString(other));
}

const String &String::operator =(const char *other)
{
	return operator =(ConstString(other));
}

const String &String::operator =(const Fragment &other)
{
	resize(other.length(), 0, other.length());

	MutableString(*this) = other;

	return *this;
}

const String &String::operator +=(const Fragment &other)
{
	size_type len = length();
	size_type olen = other.length();
	resize(len + olen, len, len*2 + olen);

	MutableString(*this) += other;

	return *this;
}

const String &String::operator +=(const char *other)
{
	return operator +=(ConstString(other));
}

const String &String::operator +=(char c)
{
	return operator +=(Fragment(&c, 1));
}


String &String::operator <<(const Fragment &other)
{
	*this += other;
	return *this;
}

String &String::operator <<(const char *other)
{
	*this += other;
	return *this;
}

String &String::operator <<(char other)
{
	*this += other;
	return *this;
}

void String::resize(size_type required, size_type keep, size_type newsize)
{
	if(mMaxSize < required)
	{
		char *newbuffer = new char[newsize + 1];

		if(mpBuffer)
		{
			if(keep != 0)
			{
				if(keep > newsize) keep = newsize;

				std::memmove(newbuffer, mpBuffer, keep);
				newbuffer[mLength = keep] = 0;
			}
			else
			{
				newbuffer[mLength = 0] = 0;
			}

			delete [] mpBuffer;
		}

		mpBuffer = newbuffer;

		mMaxSize = newsize;

		mpBuffer[length()] = '\0';
	}
}

void String::copy(const char *s, size_type len)
{
	resize(len, 0, len);
	if(0 != mMaxSize)
	{
		std::strncpy(mpBuffer, s, len);
		mpBuffer[len] = '\0';
	}
}

String::~String() 
{
	if(mpBuffer)
	{
		delete [] mpBuffer;
		mpBuffer = NULL;
	}
}

const char *String::data() const
{
	return mpBuffer;
}

const char *String::c_str() const
{
	return mpBuffer ? mpBuffer : "";
}

String::size_type String::length() const
{
	return mLength;
}

String::size_type String::size() const
{
	return mLength;
}

String::size_type String::capacity() const 
{
	return mMaxSize;
}

void String::Serialize(Reflector &reflector) const
{
	reflector | ConstString(*this);
}

void String::Deserialize(Reflector &reflector)
{
	char buffer[64];
	unsigned maxsize = sizeof(buffer)-1;
	bool complete = false;
	Deserializer &deserializer = reflector;
	
	*this = "";

	while(false == complete 
		&& reflector.Check(deserializer.DeserializeTextChunk(buffer, maxsize, complete)).Ok())
	{
		*this += Fragment(buffer, maxsize);
		maxsize = sizeof(buffer)-1;
	}
}

void String::Serialize(const String *in, String *out, Reflector &reflector)
{
	if(reflector.Serializing())
	{
		in->Serialize(reflector);
	}
	else
	{
		out->Deserialize(reflector);
	}
}

}  }

DEFINE_STATIC_REFLECTION(reflect::string::String, "string")
{
	using reflect::string::String;

	SerializeWith<&String::Serialize>();
	
	+ Concrete;
	+ ConversionFrom<const char *>();
	+ ConversionFrom<reflect::string::ConstString>();
	+ ConversionFrom<reflect::string::MutableString>();
	+ ConversionFrom<reflect::string::Fragment>();
	+ ConversionTo<reflect::string::ConstString>();
	+ ConversionTo<reflect::string::Fragment>();

	
	const String &(String::*append)(const reflect::string::Fragment &);
	append = &String::operator+=;
	Functions
		("+=", append)
		;
}
