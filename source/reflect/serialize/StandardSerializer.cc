#include <reflect/serialize/StandardSerializer.h>
#include <reflect/SerializationTag.h>
#include <reflect/Dynamic.h>
#include <reflect/Reflector.h>
#include <reflect/Property.h>
#include <reflect/OutputStream.h>
#include <reflect/EnumType.h>

#include <reflect/string/String.h>

#include <cstdarg>
#include <cstdio>

namespace reflect { namespace serialize {

StandardSerializer::StandardSerializer(OutputStream &stream)
    : mIndent(0)
    , mBreak(false)
	, mSpace(false)
	, mNextIndex(0)
	, mStream(stream)
{
}

void StandardSerializer::Indent()
{
    mIndent++;
}

void StandardSerializer::Undent()
{
    mIndent--;
}

void StandardSerializer::Space()
{
	mSpace = true;
}

void StandardSerializer::NoSpace()
{
	mSpace = false;
}

void StandardSerializer::Break()
{
    mBreak = true;
}

bool StandardSerializer::Write(const char *fmt, ...)
{
	bool result = true;

#if 1
    if(mBreak)
	{
        mBuffer.format("\r\n%*s", mIndent*2, "");
		result = result 
			&& mStream.Write(mBuffer.data(), mBuffer.size()) 
			   == OutputStream::size_type(mBuffer.size());
		mSpace = false;
		mBreak = false;
	}

	if(mSpace)
	{
		result = result && mStream.Write(" ", 1) == 1;
		mSpace = false;
	}
#else
	if(mBreak || mSpace) mStream.Write(" ", 1);
	mBreak = mSpace = false;
#endif

    va_list args;
    va_start(args, fmt);
    mBuffer.vformat(fmt, args);
    va_end(args);
	
	result = result 
		&& mStream.Write(mBuffer.data(), mBuffer.size()) 
		== OutputStream::size_type(mBuffer.size());

	return result;
}

bool StandardSerializer::Begin(const SerializationTag &tag)
{
	bool result = false;

    switch(tag.Type())
    {
    case SerializationTag::ObjectTag:
        Break();
		result = Write("(%s", tag.Text().c_str());
		Space();
        Indent();
        break;
    case SerializationTag::PropertyTag:
        Break();
		result = Write("$%s=", tag.Text().c_str());
        Indent();
        break;
    case SerializationTag::AttributeTag:
        result = Write("[%s=", tag.Text().c_str());
        break;
    case SerializationTag::ItemTag:
        result = Write("{");
		Indent();
        break;
    case SerializationTag::UnknownTag:
        return false;
    }

    return result;
}

bool StandardSerializer::End(const SerializationTag &tag)
{
	bool result = false;

    switch(tag.Type())
    {
    case SerializationTag::ObjectTag:
        Undent();
		NoSpace();
        result = Write(")");
        Space();
        break;
	case SerializationTag::PropertyTag:
		Undent();
		NoSpace();
        result = Write(";");
        Break();
        break;
    case SerializationTag::AttributeTag:
		NoSpace();
        result = Write("]");
		Space();
        break;
    case SerializationTag::ItemTag:
        Undent();
		NoSpace();
        result = Write("}");
        Space();
        break;
    case SerializationTag::UnknownTag:
        return false;
    }

    return result;
}

bool StandardSerializer::Serialize(bool b)
{
	bool result = Write("%s", b ? "true" : "false");
	Space();
	return result;
}

bool StandardSerializer::Serialize(long i)
{
    bool result = Write("%li", i);
	Space();
    return result;
}

bool StandardSerializer::Serialize(unsigned long i)
{
    bool result = Write("0x%lX", i);
	Space();
    return result;
}


bool StandardSerializer::Serialize(double d)
{
    bool result = Write("%lg", d);
	Space();
    return result;
}

bool StandardSerializer::Serialize(const Dynamic *object)
{
	bool result;

	if(object)
    {
		std::map<const Dynamic *, int>::const_iterator ref = mReferenced.find(object);

		if(ref == mReferenced.end())
		{
			const Class *serialization_class = object->GetClass()->SerializesAs();
			Break();
			result = Write("#%s", serialization_class->Name());
			Indent();
			Space();
			
			Reflector reflector(*this);
			serialization_class->SerializePointer(object, reflector);
			result = result && reflector.Ok();
    
			Undent();
		}
		else
		{
			result = Write("%%%d", ref->second);
			Space();
		}
    }
    else
    {
        result = Write("null");
        Space();
    }

    return result;
}

bool StandardSerializer::Reference(const Dynamic *object)
{
	bool result = false;

	if(mReferenced.insert(std::make_pair(object, mNextIndex)).second)
	{
		result = Write("@%d", mNextIndex);
		Break();
		mNextIndex++;
	}

	return result;
}

bool StandardSerializer::SerializeText(const char *text, unsigned nbytes)
{
	bool result = true;

	result = Write("\"") && result;;
	string::Fragment data(text, nbytes);

	while(false == data.empty())
	{
		string::Fragment::size_type next_special = data.find_first_of(string::Fragment("\"\n\0\\", 4));
		
		if(next_special == string::Fragment::npos)
		{
			next_special = data.size();
		}

		result = Write("%.*s", next_special, data.data()) && result;

		data = data.substr(next_special);

		if(data.size()) switch(*data.data())
		{
		case '"':
			result = Write("\\\"") && result;
			break;
		case '\n':
			result = Write("\\n") && result;
			break;
		case '\\':
			result = Write("\\\\") && result;
			break;
		case '\0':
			result = Write("\\0") && result;
			break;
		}

		data = data.substr(1);
	}

	result = Write("\"") && result;
	Space();

	return result;
}

bool StandardSerializer::SerializeData(const void *data, unsigned nbytes)
{
	bool result = true;
    const unsigned char *ucdata = static_cast<const unsigned char *>(data);

    while(result && nbytes--)
    {
        result = Write("%02X", int(*ucdata++));
        Space();
    }

    return result;
}

bool StandardSerializer::SerializeEnum(int value, const EnumType *clazz)
{
	bool result = false;
	const EnumType::ToNameMap &name_map = clazz->NameMap();
	EnumType::ToNameMap::const_iterator it = name_map.find(value);

	if(it != name_map.end())
	{
		result = Write("%s", it->second.c_str());
		Space();
	}
	else
	{
		result = Write("0x%lX", value);
		Space();
	}
	
	return result;
}

bool StandardSerializer::SerializeProperty(const void *object, const Property *prop)
{
	Reflector reflector(*this);
	prop->Serialize(object, 0, reflector);
	return reflector.Ok();
}

} }
