#ifndef REFLECT_UTILITY_INOUTREFLECTOR_H_
#define REFLECT_UTILITY_INOUTREFLECTOR_H_

#include <reflect/string/StringOutputStream.h>
#include <reflect/string/StringInputStream.h>

#include <reflect/Reflector.h>
#include <reflect/serialize/StandardSerializer.h>
#include <reflect/serialize/StandardDeserializer.h>
#include <reflect/serialize/ShallowSerializer.h>
#include <reflect/serialize/ShallowDeserializer.h>

namespace reflect { namespace utility {

template<
	typename SerializerType = serialize::StandardSerializer,
	typename DeserializerType = serialize::StandardDeserializer>
class InOutReflector
{
public:
	InOutReflector(string::Fragment initial_data = string::Fragment())
		: mData(initial_data)
		, mIn(mData)
		, mOut(mData)
		, mSerializer(mOut)
		, mDeserializer(mIn)
		, mOk(true)
	{}
	
	DeserializerType &GetDeserializer()
	{
		return mDeserializer;
	}
	
	SerializerType &GetSerializer()
	{
		return mSerializer;
	}
	
	void Fail()
	{
		mOk = false;
	}
	
	void Check(bool result) 
	{
		if(false == result)
			Fail();
	}

	template<typename DataType>
	InOutReflector &operator >>(DataType &data)
	{
		Reflector reflector(GetDeserializer());
		reflector | data;
		
		Check(reflector.Ok());
		
		return *this;
	}
	
	template<typename DataType>
	InOutReflector &operator <<(const DataType &data)
	{
		Reflector reflector(GetSerializer());
		reflector | data;

		Check(reflector.Ok());
		
		return *this;
	}
	
	operator bool() const { return mOk; }

	bool Ok() const { return mOk; }	

	const string::String &Data() const { return mData; }
private:
	class SharedStringInputStream : public InputStream
	{
	public:
		using InputStream::size_type;

		SharedStringInputStream(string::String &s)
			: mString(s)
		{}

		size_type Read(void *data, size_type size)
		{
			if(size > mString.size())
			{
				size = mString.size();
			}

			std::memcpy(data, mString.data(), size);
			mString = mString.substr(size);
			return size;
		}
	private:
		string::String &mString;
	};
	
	class SharedStringOutputStream : public OutputStream
	{
	public:
		using OutputStream::size_type;
	
		SharedStringOutputStream(string::String &s)
			: mString(s)
		{}

		size_type Write(const void *data, size_type size)
		{
			mString += string::Fragment(static_cast<const char *>(data), size);
			return size;
		}
	private:
		string::String &mString;
	};
	
	string::String mData;
	SharedStringInputStream mIn;
	SharedStringOutputStream mOut;
	SerializerType mSerializer;
	DeserializerType mDeserializer;
	bool mOk;
};

template<typename BaseSerializer = serialize::StandardSerializer>
class ComposedShallowSerializer
{
public:
	ComposedShallowSerializer(OutputStream &output)
		: mSerializer(output)
		, mShallowSerializer(mSerializer)
	{
	}
	
	operator Serializer &() { return mShallowSerializer; }
	
private:
	BaseSerializer mSerializer;
	serialize::ShallowSerializer mShallowSerializer;
};

template<typename BaseDeserializer = serialize::StandardDeserializer>
class ComposedShallowDeserializer
{
public:
	ComposedShallowDeserializer(InputStream &input)
		: mDeserializer(input)
		, mShallowDeserializer(mDeserializer)
	{
	}
	
	operator Deserializer &() { return mShallowDeserializer; }
	
private:
	BaseDeserializer mDeserializer;
	serialize::ShallowDeserializer mShallowDeserializer;
};

typedef InOutReflector<
	serialize::StandardSerializer,
	serialize::StandardDeserializer> StandardInOutReflector;

typedef InOutReflector<
	ComposedShallowSerializer<serialize::StandardSerializer>,
	ComposedShallowDeserializer<serialize::StandardDeserializer> >
	ShallowInOutReflector;

} }

#endif
