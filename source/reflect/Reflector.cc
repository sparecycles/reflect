#include <reflect/Reflector.h>

namespace reflect {

Reflector::Reflector(::reflect::Serializer &serializer)
    : mSerializer(&serializer)
    , mDeserializer(0)
    , mOK(true)
{}

Reflector::Reflector(::reflect::Deserializer &deserializer)
    : mSerializer(0)
    , mDeserializer(&deserializer)
    , mOK(true)
{}

bool Reflector::Serializing() const
{ 
    return mSerializer != 0; 
}

bool Reflector::Deserializing() const
{ 
    return mDeserializer != 0; 
}

Serializer &Reflector::GetSerializer() const
{ 
	return *mSerializer;
}

Deserializer &Reflector::GetDeserializer() const 
{
	return *mDeserializer; 
}

void Reflector::Fail()
{
    mOK = false;
}

Reflector &Reflector::Check(bool result)
{
    if(result == false)
        Fail();

    return *this;
}

bool Reflector::Ok() const
{
    return mOK;
}

Reflector &Reflector::operator |(Variant &data)
{
	data.Serialize(*this);

    return *this;
}

}
