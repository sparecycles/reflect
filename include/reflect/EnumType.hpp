// File: EnumType.hpp

#ifndef REFLECT_ENUMTYPE_HPP_
#define REFLECT_ENUMTYPE_HPP_

#include <reflect/EnumType.h>
#include <reflect/Type.hpp>

namespace reflect {

// Class: EnumType::DescriptionHelper
template<typename T>
class EnumType::DescriptionHelper
	: public Type::DescriptionHelper<T>
{
public:
	DescriptionHelper()
	{
		struct ValueHelper
		{
			static int Read(const void *data) { return int(*static_cast<const T *>(data)); }
			static void Write(void *data, int value) { *static_cast<T *>(data) = T(value); }
		};
		
		TypeOf<T>()->SetAccessors(&ValueHelper::Read, &ValueHelper::Write);
	}

protected:
	struct ValueHelper
	{
		const ValueHelper &operator ()(T value, const char *name) const
		{
			TypeOf<T>()->AddValue(int(value), name);
	
			return *this;			
		}
	};
	
	// Member: Values
	//  This member of EnumType::DescriptionHelper is used to help register
	//  enums in a reflection definition.
	//
	// Example:
	// > DEFINE_STATIC_REFLECTION(EnumType, "EnumTypeName")
	// > {
	// >     Values
	// >         (A, "A")
	// >         (B, "B")
	// >         (C, "C")
	// >         ;
	// > }
	ValueHelper Values;	
};

}

#endif
