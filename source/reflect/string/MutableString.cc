#include <reflect/string/MutableString.h>
#include <reflect/PrimitiveType.hpp>
#include <reflect/PrimitiveTypes.h>
#include <cstdio>
#include <cstring>

#include <reflect/config/config.h>

namespace reflect { namespace string {

MutableString::MutableString(char *buffer, size_type buffersize, size_type &length)
	: mpBuffer(buffer)
	, mLength(length)
	, mMaxSize(buffersize - 1)
{
	buffer[mMaxSize] = '\0';
}

MutableString::iterator MutableString::begin() 
{ 
	return mpBuffer;
}

MutableString::iterator MutableString::end() 
{ 
	return mpBuffer + size(); 
}

MutableString::const_iterator MutableString::begin() const 
{ 
	return Fragment(*this).begin(); 
}

MutableString::const_iterator MutableString::end() const
{ 
	return Fragment(*this).end(); 
}

const char *MutableString::data() const 
{ 
	return mpBuffer; 
}

const char *MutableString::c_str() const 
{ 
	return mpBuffer; 
}

MutableString::size_type MutableString::length() const 
{
	return mLength;
}

MutableString::size_type MutableString::size() const 
{
	return mLength;
}

MutableString::size_type MutableString::capacity() const 
{
	return mMaxSize;
}

MutableString::size_type MutableString::format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
	size_type result = vformat(format, args);
    va_end(args);

	return result;
}

MutableString::size_type MutableString::vformat(const char *format, va_list args)
{
	int result = vsnprintf(mpBuffer, capacity(), format, args);

	if(result >= 0 && size_type(result) >= capacity())
	{
		mpBuffer[capacity()] = '\0';
		mLength = capacity();
		return result;
	}
	else if(result < 0)
	{
		mLength = 0;
		mpBuffer[mLength] = '\0';
		// HACK because MSVC can't do partial
		// writes and return requested sizes.
		return MutableString::npos;
	}
	else
	{
		mLength = (size_type)result;
		return result;
	}

}


void MutableString::replace(size_type offset, size_type sz, Fragment value)
{
	if(offset + sz > length()) return;

	size_t tail_size = length() - (offset + sz);
	
	if(tail_size + offset + value.length() > capacity())
	{
		if(offset + value.length() < capacity())
			tail_size = capacity() - ( offset + value.length() );
		else
			tail_size = 0;
	}

	if(offset + value.length() > capacity())
	{
		value = value.substr(0, capacity() - offset);
	}

	if(tail_size > 0)
		std::memmove(mpBuffer + offset + value.size(), mpBuffer + offset + sz, tail_size);

	std::memmove(mpBuffer + offset, value.data(), value.length());

	mpBuffer[mLength = offset + value.length() + tail_size] = '\0';
}

///////////////////////////////////////////////////////////////////////////////////////

MutableString::size_type MutableString::find(char ch, MutableString::size_type pos) const 
{
	return Fragment(*this).find(ch, pos);
}

MutableString::size_type MutableString::find(const char *s, MutableString::size_type pos, MutableString::size_type len) const 
{
	return Fragment(*this).find(s, pos, len);
}

MutableString::size_type MutableString::find(const Fragment &s, MutableString::size_type pos) const 
{
	return Fragment(*this).find(s, pos);
}

MutableString::size_type MutableString::find_first_of(char ch, MutableString::size_type pos) const 
{
	return Fragment(*this).find_first_of(ch, pos);
}

MutableString::size_type MutableString::find_first_of(const Fragment &s, MutableString::size_type pos) const 
{
	return Fragment(*this).find_first_of(s, pos);
}

MutableString::size_type MutableString::rfind(char ch, MutableString::size_type pos) const 
{
	return Fragment(*this).rfind(ch, pos);
}

MutableString::size_type MutableString::rfind(const char *s, MutableString::size_type pos, MutableString::size_type len) const 
{
	return Fragment(*this).rfind(s, pos, len);
}

MutableString::size_type MutableString::rfind(const Fragment &s, MutableString::size_type pos) const 
{
	return Fragment(*this).rfind(s, pos);
}

MutableString::size_type MutableString::find_last_of(char ch, MutableString::size_type pos) const 
{
	return Fragment(*this).find_last_of(ch, pos);
}

MutableString::size_type MutableString::find_last_of(const Fragment &s, MutableString::size_type pos) const 
{
	return Fragment(*this).find_last_of(s, pos);
}

Fragment MutableString::substr(MutableString::size_type index, MutableString::size_type amount) const 
{
	return Fragment(*this).substr(index, amount);
}

bool MutableString::empty() const 
{
	return 0 == size();
}

MutableString::operator Fragment() const 
{
	return Fragment(data(), length());
}

MutableString::operator ConstString() const 
{
	return ConstString(c_str());
}

bool MutableString::operator ==(const Fragment &other) const 
{
	return Fragment(*this) == other;
}

bool MutableString::operator <=(const Fragment &other) const 
{
	return Fragment(*this) <= other;
}

bool MutableString::operator >=(const Fragment &other) const 
{
	return Fragment(*this) >= other;
}

bool MutableString::operator < (const Fragment &other) const 
{
	return Fragment(*this) <  other;
}

bool MutableString::operator > (const Fragment &other) const 
{
	return Fragment(*this) >  other;
}

bool MutableString::operator !=(const Fragment &other) const 
{
	return Fragment(*this) != other;
}

const MutableString &MutableString::operator =(const MutableString &other)
{
	return *this = Fragment(other);
}

const MutableString &MutableString::operator =(const Fragment &other)
{
	Fragment copyable = other.substr(0, capacity());

	std::memmove(mpBuffer, copyable.data(), copyable.length());
	mLength = copyable.length();
	mpBuffer[mLength] = '\0';

	return *this;
}

const MutableString &MutableString::operator +=(const Fragment &other)
{
	Fragment copyable = other.substr(0, capacity() - length());

	std::memmove(mpBuffer + length(), copyable.data(), copyable.length());

	mLength += copyable.length();
	mpBuffer[mLength] = '\0';

	return *this;
}

const MutableString &MutableString::operator +=(char c)
{
	Fragment s(&c, 1);

	return *this += s;
}

namespace internals {
void SerializeMutableString(const MutableString *in, MutableString *out, Reflector &reflector)
{
	if(reflector.Serializing())
	{
		reflector | ConstString(*in);
	}
	else
	{
		char buffer[64];
		unsigned maxsize = sizeof(buffer)-1;
		bool complete = false;
		Deserializer &deserializer = reflector;
		
		*out = "";

		while(false == complete 
			&& reflector.Check(deserializer.DeserializeTextChunk(buffer, maxsize, complete)).Ok())
		{
			*out += Fragment(buffer, maxsize);
			maxsize = sizeof(buffer)-1;
		}
	}
}
} // namespace {}

} }

DEFINE_STATIC_REFLECTION(reflect::string::MutableString, "mutable_string")
{
	SerializeWith<&reflect::string::internals::SerializeMutableString>();
}
