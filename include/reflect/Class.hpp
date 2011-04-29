#ifndef REFLECT_CLASS_HPP_
#define REFLECT_CLASS_HPP_

#include <reflect/ObjectType.hpp>
#include <reflect/string/String.h>
#include <reflect/string/SharedString.h>

namespace reflect {

// Class: Class::DescriptionHelper
//
// Used in reflection definitions.
// This description helper doesn't
// add any manual features, but it does do
// setup for 
//
// See Also:
//   - <Type>
//   - <DEFINE_REFLECTION>
template<typename T>
class Class::DescriptionHelper : public ObjectType::DescriptionHelper<T>
{
public:
	DescriptionHelper()
	{
		struct DynamicPointerOps
		{
			static Dynamic *FromType(void *opaque)
			{
				T *value = static_cast<T *>(opaque);
				Dynamic *castable = value;
				return castable;
			}

			static void *FromDynamic(Dynamic *castable)
			{
				T *value = static_cast<T *>(castable);
				void *opaque = static_cast<void *>(value);
				return opaque;
			}

			static void *Construct(void *data)
			{
				T *&ptr = *static_cast<T **>(data);
				ptr = 0;
				return opaque_cast(&ptr);
			}
			
			static void *Destruct(void *data)
			{
				T** ptr = translucent_cast<T **>(data);
				*ptr = 0;
				return static_cast<void *>(ptr);
			}
		};
			
		TypeOf<T *>()->Initialize(
			TypeOf<T>(),
			&DynamicPointerOps::FromType,
			&DynamicPointerOps::FromDynamic
			);

		TypeOf<T *>()->SetBasicOperations(
			sizeof(T *),
			AlignOf<T *>::Result, 
			&DynamicPointerOps::Construct,
			&DynamicPointerOps::Destruct);
			
		string::String name;
		name.format("%s *", TypeOf<T>()->Name());
		const char *name_string = string::SharedString::Copy(string::ConstString(name)).c_str();
		TypeOf<T *>()->SetName(name_string);
  
		// bootstrapping hack:
		// flyweight serialization should 
		// likely be handled by base classes,
		// as in this case, where Type handles serialization of all Types.
		if(TypeDerives<T, Type>::Result)
		{
			TypeOf<T>()->SetSerializationClass(TypeOf<Type>());
		}
	}
	
};

}

#endif
