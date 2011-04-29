#ifndef REFLECT_STRUCTTYPE_HPP_
#define REFLECT_STRUCTTYPE_HPP_

#include <reflect/StructType.h>
#include <reflect/ObjectType.hpp>

#include <reflect/property/DirectDataProperty.hpp>
#include <reflect/property/DirectArrayProperty.hpp>
#include <reflect/property/DirectVectorProperty.hpp>

#include <reflect/property/AccessorProperty.hpp>
#include <reflect/property/AccessorDirectProperty.hpp>
#include <reflect/property/AccessorIndirectProperty.hpp>

#include <reflect/property/BaseClassProperty.hpp>

namespace reflect {

// Class: StructType::DescriptionHelper
template<typename T>
class StructType::DescriptionHelper
	: public ObjectType::DescriptionHelper<T>
{
public:
	DescriptionHelper()
	{
	}

protected:
	struct ArrayAnnotation {};
	ArrayAnnotation Array;
	
	struct MemberHelper
	{
		template<typename SubType>
		const MemberHelper &base() const
		{
			struct BaseHelper
			{
				static SubType *Cast(void *object)
				{
					return static_cast<SubType *>(static_cast<T *>(object));
				}
			};
			
			TypeOf<T>()->AddMember(
				new property::BaseClassProperty<SubType>(&BaseHelper::Cast));
	
			return *this;
		}

		template<typename MemberType>
		const MemberHelper &operator ()(MemberType T::*pmember) const
		{
			TypeOf<T>()->AddMember(
				new property::DirectDataProperty<T, MemberType>(pmember));
			
			return *this;
		}
		
		template<typename MemberType, unsigned size>
		const MemberHelper &operator ()(MemberType (T::*pmember)[size], const ArrayAnnotation &) const
		{
			TypeOf<T>()->AddMember(
				new property::DirectArrayProperty<T, MemberType>(
					reinterpret_cast<MemberType (T::*)[]>(pmember), size));
			
			return *this;
		}
		
		template<typename MemberType>
		const MemberHelper &operator ()(MemberType T::*pmember, const ArrayAnnotation &) const
		{
			TypeOf<T>()->AddMember(
				new property::DirectVectorProperty<T, MemberType>(pmember));
			
			return *this;
		}
				
		template<typename MemberType>
		const MemberHelper &operator ()(MemberType (T::*getter)() const,
                                        void (T::*setter)(MemberType)) const
		{
			TypeOf<T>()->AddMember(
				new property::AccessorProperty<T, MemberType>(getter, setter));

			return *this;
		}
		
		template<typename MemberType>
		const MemberHelper &operator ()(const MemberType &(T::*getter)() const,
                                        void (T::*setter)(const MemberType &)) const
		{
			TypeOf<T>()->AddMember(
				new property::AccessorDirectProperty<T, MemberType>(getter, setter));

			return *this;
		}
		
		template<typename MemberType>
		const MemberHelper &operator ()(void (T::*getter)(MemberType &) const,
                                        void (T::*setter)(const MemberType &)) const
		{
			TypeOf<T>()->AddMember(
				new property::AccessorIndirectProperty<T, MemberType>(getter, setter));
					
			return *this;
		}
	};

	MemberHelper Members;	
};

}

#endif
