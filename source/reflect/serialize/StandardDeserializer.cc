#include <reflect/serialize/StandardDeserializer.h>
#include <reflect/InputStream.h>
#include <reflect/SerializationTag.h>
#include <reflect/Reflector.h>
#include <reflect/Property.h>
#include <reflect/autocast.h>
#include <reflect/EnumType.h>
#include <reflect/string/MutableString.h>
#include <cctype>
#include <cstring>
#include <cstdlib>

namespace reflect { namespace serialize {

StandardDeserializer::StandardDeserializer(InputStream &stream)
	: mStream(stream)
	, mPeekChar(0)
	, mDeserializingText('\0')
{
}

char StandardDeserializer::Peek()
{
	if(0 == mPeekChar)
	{
		if(mStream.Read(&mPeekChar, 1) != 1)
			mPeekChar = 0;
	}

	return mPeekChar;
}

char StandardDeserializer::Read()
{
	char result;
	Peek();
	result = mPeekChar;
	mPeekChar = 0;
	return result;
}

void StandardDeserializer::EatSpace()
{
	for(char c; c = Peek(), c && std::isspace(c); Read())
		;
}

bool StandardDeserializer::Begin(SerializationTag &tag, SerializationTag::TagType type)
{
	EatSpace();

	char c = Peek();

	tag.Text() = "";

	switch(c)
	{
	case '(': // OBJECT
		if(SerializationTag::ObjectTag == type
		|| SerializationTag::UnknownTag == type)
		{
			tag.Type() = SerializationTag::ObjectTag;
			Read();
			EatSpace();
			if(ReadWord(tag.Text()))
				return true;
		}
		break;
	case '$': // PROPERTY
		if(SerializationTag::PropertyTag == type
		|| SerializationTag::UnknownTag == type)
		{
			tag.Type() = SerializationTag::PropertyTag;
			Read();
			while(char c = Read())
			{
				if(c == '=') return true;
				tag.Text() += c;
			}
		}
		break;
	case '[': // ATTRIBUTE
		if(SerializationTag::AttributeTag == type
		|| SerializationTag::UnknownTag == type)
		{
			tag.Type() = SerializationTag::AttributeTag;
			Read();
			EatSpace();
			while(char c = Read())
			{
				if(c == '=') return true;
				tag.Text() += c;
			}
		}
		break;
	case '{': // ITEM
		if(SerializationTag::ItemTag == type
		|| SerializationTag::UnknownTag == type)
		{
			tag.Type() = SerializationTag::ItemTag;
			Read();
			return true;
		}
		break;
	}

	return false;
}

bool StandardDeserializer::End(SerializationTag &tag)
{
	bool result = false;

	while(false == result)
	{
		EatSpace();

		char c = Peek();

		switch(tag.Type())
		{
		case SerializationTag::ObjectTag:
			result = c == ')';
			break;
		case SerializationTag::PropertyTag:
			result = c == ';';
			break;
		case SerializationTag::AttributeTag:
			result = c == ']';
			break;
		case SerializationTag::ItemTag:
			result = c == '}';
			break;
		case SerializationTag::UnknownTag:
			break;
		}

		// Oh No: There is unknown Data in the stream
		//  Lets exploit the structure of the data to try to clear it out.
		if(false == result)
		{
			bool retry = false;

			string::ArrayString<0> discard;
			
			// easy stuff: try to discard a word (bool, long or double),
			// or a add a null reference.
			retry = (ReadWord(discard) || Reference(NULL));
			
			if(false == retry)
			{
				if(Peek() == '#')
				{
					Read();

					if(false == (ReadWord(discard)))
						return false;
	
					retry = true;
				}
			}

			SerializationTag tag;

			if(Begin(tag))
			{
				retry = End(tag);
			}

			if(false == retry)
			{
				// total failure, stream is malformed.
				return false;
			}
		}
	}

	Read();

	return true;
}

int StandardDeserializer::ReadWord(string::MutableString s)
{
	// count size seperately, don't use s.size() because s might have 0 capacity!
	int size = 0;
	EatSpace();

	for(char c; (c = Peek(), c) && !isspace(c) && 0 == std::strchr("[]{}()$=#,;", c); Read())
	{
		s += c;
		size++;
	}

	return size;
}

bool StandardDeserializer::Deserialize(bool &value)
{
	string::ArrayString<64> s;
	
	if(ReadWord(s))
	{
		value = (s == "1" || s == "true");
		return true;
	}

	return false;
}

bool StandardDeserializer::Deserialize(long &value)
{	
	string::ArrayString<64> s;
	
	if(ReadWord(s))
	{
		char *endp = 0;
		value = std::strtol(s.c_str(), &endp, 0);
		return endp == s.c_str() + s.size();
	}

	return false;
}

bool StandardDeserializer::Deserialize(unsigned long &value)
{	
	string::ArrayString<64> s;
	
	if(ReadWord(s))
	{
		char *endp = 0;
		value = std::strtoul(s.c_str(), &endp, 0);
		return endp == s.c_str() + s.size();
	}

	return false;
}


bool StandardDeserializer::Deserialize(double &value)
{
	string::ArrayString<64> s;
	
	if(ReadWord(s))
	{
		char *endp = 0;
		value = std::strtod(s.c_str(), &endp);
		return endp == s.c_str() + s.size();
	}

	return false;
}

bool StandardDeserializer::Deserialize(Dynamic *&object)
{
	string::ArrayString<256> s;

	EatSpace();
	char first = Peek();

	if(first == '#')
	{
		Read();
		EatSpace();

		if(ReadWord(s))
		{
			if(Class *clazz = Class::FindType(s.c_str()) % autocast)
			{
				Reflector reflector(*this);
				clazz->DeserializePointer(object, reflector);
				if(reflector.Ok())
					return true;
			}
		}
	}
	else if(first == '%')
	{
		Read();
		long l;
		if(Deserialize(l))
		{
			if(unsigned(l) < mReferenced.size())
			{
				object = mReferenced[unsigned(l)];
				return true;
			}

			object = 0;
			return false;
		}
	}
	else if(ReadWord(s))
	{
		if(s == "null")
		{
			object = NULL;
			return true;
		}
	}

	return false;
}

bool StandardDeserializer::Reference(Dynamic *object)
{
	EatSpace();

	char c = Peek();

	if(c != '@')
		return false;

	Read();

	long id;

	if(Deserialize(id) && id == long(mReferenced.size()))
	{
		mReferenced.push_back(object);
		return true;
	}

	return false;
}

bool StandardDeserializer::DeserializeTextChunk(char *text, unsigned &max_bytes, bool &complete)
{
	if(false == mDeserializingText)
	{
		EatSpace();
		if(Peek() == '\"' || Peek() == 0["'"])
		{
			mDeserializingText = Read();
		}
	}

	char discard;
	unsigned bytesread = 0;
	string::MutableString::size_type output_length = 0;
	string::MutableString output(text ? text : &discard, text ? max_bytes : 1, output_length);

	if(mDeserializingText)
	{
		while(bytesread < max_bytes)
		{
			char c = Read();

			if(c == '\0')
			{
				return false;
			}
			if(c == '\\')
			{
				c = Read();
				switch(c)
				{
				case '0':
					output += '\0';
					break;
				case '\\':
					output += '\\';
					break;
				case 'n':
					output += '\n';
					break;
				case '"':
					output += '"';
					break;
				case '\'':
					output += '\'';
					break;
				default:
					output += c;
					break;
				}
			}
			else if(c != mDeserializingText)
			{
				output += c;
			}
			else
			{
				complete = true;
				mDeserializingText = '\0';
				max_bytes = bytesread;
				return true;
			}

			bytesread++;
		}

		return false; // malformed stream
	}
	else
	{
		return false; // OK, just no text here.
	}
}

bool StandardDeserializer::DeserializeData(void *data, unsigned size)
{	
	bool result = true;
    unsigned char *ucdata = static_cast<unsigned char *>(data);

    while(result && size--)
    {
		string::ArrayString<2> buffer;
		
		if(ReadWord(buffer) != 2)
		{
			result = false;
		}
		
		char *endp = 0;

        *ucdata++ = static_cast<unsigned char>(strtoul(buffer.data(), &endp, 16));

        if(endp != buffer.data() + 2)
        {
			result = false;
        }
    }

    return result;
}

bool StandardDeserializer::DeserializeEnum(int &value, const EnumType *clazz)
{
	const EnumType::ToValueMap &value_map = clazz->ValueMap();
	
	string::ArrayString<256> wordbuf;
	string::MutableString word(wordbuf);
	
	EatSpace();
	
	if(Peek() == '"')
	{
		Reflector reflector(*this);
		reflector | word;
		if(!reflector) return false;
	}
	else
	{
		if(!ReadWord(word))
			return false;
	}
	
	EnumType::ToValueMap::const_iterator it = value_map.find(string::SharedString::Find(word));
	
	if(it != value_map.end())
	{
		value = it->second;
		return true;
	}
	else 
	{
		char *end;
		value = strtol(word.c_str(), &end, 0);
		if(end == word.data() + word.size())
		{
			return true;
		}
		else
		{
			if(word.size())
			{
				value = 0;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

bool StandardDeserializer::DeserializeProperty(void *object, const Property *prop)
{
	Reflector reflector(*this);
	prop->Serialize(object, object, reflector);
	return reflector.Ok();
}

} }
