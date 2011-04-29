#ifndef REFLECT_PRIMITIVETYPE_HPP_
#define REFLECT_PRIMITIVETYPE_HPP_

#include <reflect/PrimitiveType.h>
#include <reflect/ObjectType.hpp>
#include <reflect/Reflector.h>

namespace reflect {

template<typename T>
class PrimitiveType::DescriptionHelper
	: public ObjectType::DescriptionHelper<T>
{
public:
	DescriptionHelper()
	{
	}

	template<typename TargetType>
	void SerializeAs() const
	{
		struct Target
		{
			Target(T &value)
				: mTargetValue(0)
				, mValue(value)
			{}

			operator TargetType &() const { return mTargetValue; }
			~Target() { mValue = T(mTargetValue); }

			mutable TargetType mTargetValue;
			T &mValue;
		};

		struct Helper
		{
			static void SerializeType(const void *in, void *out, Reflector &reflector)
			{
				reflector.Check(reflector.Serializing() 
					? reflector.GetSerializer().Serialize(TargetType(*translucent_cast<const T *>(in))) 
					: reflector.GetDeserializer().Deserialize(Target(*translucent_cast<T *>(out))));
			}
		};
		
		TypeOf<T>()->SetSerializer(&Helper::SerializeType);
	}

	template<typename As, void (*SerializationFunction)(const As *in, As *out, Reflector &reflector)>
	void SerializeAsWith() const
	{
		struct Helper
		{
			static void SerializeType(const void *in, void *out, Reflector &reflector)
			{
				SerializationFunction(
					static_cast<const As *>(translucent_cast<const T *>(in)),
					static_cast<As *>(translucent_cast<T *>(out)),
					reflector);
			}
		};		

		TypeOf<T>()->SetSerializer(&Helper::SerializeType);
	}


	template<void (*SerializationFunction)(const T *in, T *out, Reflector &reflector)>
	void SerializeWith() const
	{
		struct Helper
		{
			static void SerializeType(const void *in, void *out, Reflector &reflector)
			{
				SerializationFunction(
					translucent_cast<const T *>(in),
					translucent_cast<T *>(out),
					reflector);
			}
		};		

		TypeOf<T>()->SetSerializer(&Helper::SerializeType);
	}
	
	struct
	{
		void operator +() const
		{
			struct Operations
			{
				static void *Construct(void *data)
				{
					T *datum = new(data) T;
					*datum = 0;
					return opaque_cast(datum);
				}
				
				static void *Destruct(void *data)
				{
					T *datum = translucent_cast<T *>(data);
					*datum = T(-1);
					return static_cast<void *>(datum);
				}
			};
			
			TypeOf<T>()->SetBasicOperations(
				sizeof(T),
				AlignOf<T>::Result, 
				&Operations::Construct,
				&Operations::Destruct);
			
			typedef Type::DescriptionHelper<T> TDHT;
			+ typename TDHT::template ConversionFrom<char>();
			+ typename TDHT::template ConversionFrom<signed char>();
			+ typename TDHT::template ConversionFrom<unsigned char>();
			+ typename TDHT::template ConversionFrom<signed short>();
			+ typename TDHT::template ConversionFrom<unsigned short>();
			+ typename TDHT::template ConversionFrom<signed int>();
			+ typename TDHT::template ConversionFrom<unsigned int>();
			+ typename TDHT::template ConversionFrom<signed long>();
			+ typename TDHT::template ConversionFrom<unsigned long>();
			+ typename TDHT::template ConversionFrom<float>();
			+ typename TDHT::template ConversionFrom<double>();
		}
	} ConcreteNumeric;	
};

}

#endif
