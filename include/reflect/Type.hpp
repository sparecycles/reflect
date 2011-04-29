#ifndef REFLECT_TYPE_HPP_
#define REFLECT_TYPE_HPP_

#include <reflect/Reflection.hpp>
#include <reflect/Type.h>
#include <new> // for placement new

namespace reflect {

// TODO: some of the template/static_assert stuff here is useful, move it somewhere nice
template<bool, typename iftrue, typename iffalse> struct template_if;
template<typename iftrue, typename iffalse> struct template_if<true, iftrue, iffalse> : public iftrue {};
template<typename iftrue, typename iffalse> struct template_if<false, iftrue, iffalse> : public iffalse {};
 
template<typename T> struct AlignOf
{
private:
	struct Align { T t; char pad; };
	typedef struct AlignOf<Align> next;
	typedef struct { enum { Result = sizeof(Align) - sizeof(T) }; } done;
public:
	enum { Result = template_if<(sizeof(Align) > sizeof(T)), done, next>::Result };
};

template<typename Check, typename Base> struct TypeDerives
{
	enum { Result = TypeDerives<typename Check::BaseType, Base>::Result };
};

template<typename Base> struct TypeDerives<Base, Base>
{
	enum { Result = true };
};

template<typename Base> struct TypeDerives<Dynamic, Base>
{
	enum { Result = false };
};

#define unique_static_assert unique_static_assert_helper(__LINE__)
#define unique_static_assert_helper(line) unique_static_assert_helper_(line)
#define unique_static_assert_helper_(line) static_assert_##line
#define static_assert(text, expr) namespace { inline void unique_static_assert(int text##_is_wrong[(expr) ? 1 : 0]) {} }

// Class: Type::DescriptionHelper
//
// Template base class for description helpers, used in reflection definitions.
//
// Features:
//    Concrete - Requires a default constructor, public destructor.  
//      makes the type non-abstract.
//    ConversionTo<TargetType>() - declares conversion to TargetType.
//    ConversionToEx<TargetType, Intermediate>() - conversion to TargetType through Intermediate
//    ConversionFrom<SourceType>() - declares conversion from TargetType.
//
// See Also:
// - <Type>
// - <Type::ConvertValue>
// - <DEFINE_REFLECTION>
template<typename T>
class Type::DescriptionHelper
{
public:
	struct
	{
		void operator +() const
		{
			struct Operations
			{
				static void *Construct(void *data)
				{
					typedef typename Signature<T>::ClassType ClassTypeOfType;
					return ClassTypeOfType::OpaqueCast(new(static_cast<T *>(data)) T);
				}
				
				static void *Destruct(void *data)
				{
					T *pointer = translucent_cast<T *>(data);
					pointer->~T();
					return static_cast<void *>(pointer);
				}
			};
			
			TypeOf<T>()->SetBasicOperations(
				sizeof(T),
				AlignOf<T>::Result, 
				&Operations::Construct,
				&Operations::Destruct);
		}
	} Basic;

	struct {
		void operator +() const
		{
			typedef Type::DescriptionHelper<T> TDHT;
			TDHT tdht;
			+ tdht.Basic;
			+ typename TDHT::template ConversionFrom<T>();
		}
	} Concrete;

	template<typename Target>
	struct ConversionTo
	{
		void operator +() const
		{
			struct Operations
			{
				static void Convert(void *target, const void *source)
				{
					*translucent_cast<Target *>(target) = Target(*translucent_cast<const T *>(source));
				}
			};
			
			TypeOf<Target>()->RegisterConversion(TypeOf<T>(), &Operations::Convert);
		}
	};

	template<typename Target, typename As>
	struct ConversionToEx
	{
		void operator +() const
		{
			struct Operations
			{
				static void Convert(void *target, const void *source)
				{
					Target &target_val = *translucent_cast<Target *>(target);
					const T &source_val = *translucent_cast<const T *>(source);
					target_val = As(source_val);
				}
			};
			
			TypeOf<Target>()->RegisterConversion(TypeOf<T>(), &Operations::Convert);
		}
	};

	template<typename Source>
	struct ConversionFrom
	{
		void operator +() const
		{
			struct Operations
			{
				static void Convert(void *target, const void *source)
				{
					T &target_val = *translucent_cast<T *>(target);
					const Source &source_val = *translucent_cast<const Source *>(source);
					target_val = T(source_val);
				}
			};
			
			TypeOf<T>()->RegisterConversion(TypeOf<Source>(), &Operations::Convert);
		}
	};
	
	DescriptionHelper() {}
};

}

#endif
