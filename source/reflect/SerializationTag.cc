#include <cstring>
#include <reflect/SerializationTag.h>

namespace reflect {

SerializationTag::SerializationTag(const char *text, TagType type)
    : mType(type)
{
	Text() = text;
}

SerializationTag::SerializationTag(TagType type)
    : mType(type)
{
}

string::MutableString SerializationTag::Text()
{
	return mText;
}

string::ConstString SerializationTag::Text() const
{
	return mText;
}

SerializationTag::TagType SerializationTag::Type() const
{
    return mType;
}

SerializationTag::TagType &SerializationTag::Type()
{
    return mType;
}

bool SerializationTag::operator ==(const SerializationTag &tag) const
{
	return Type() == tag.Type() && Text() == tag.Text();
}

}
