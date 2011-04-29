#ifndef REFLECT_OBJECTTYPE_HPP_
#define REFLECT_OBJECTTYPE_HPP_

#include <reflect/ObjectType.h>
#include <reflect/Type.hpp>
#include <reflect/function/Function.hpp>
#include <reflect/PrimitiveTypes.h>
#include <reflect/string/SharedString.h>

namespace reflect {

// Class: ObjectType::DescriptionHelper
// Provides reflection definitions for things
// reflected by <ObjectType> and its subclasses.
template<typename T>
class ObjectType::DescriptionHelper : public Type::DescriptionHelper<T>
{
public:
	struct FunctionCollector
	{
		struct FunctionAnnotator;

		template<typename FunctionType>
		const FunctionAnnotator operator ()(const char *name, FunctionType function) const;		
	};
	
	// Member: Functions
	// Used to help register functions in a reflection definition.
	//
	// For example, in a <DEFINE_REFLECTION>
	// (begin code)
	// DEFINE_REFLECTION(Circle)
	// {
	//     Functions
	//        ("ComputeArea", &Circle::ComputeArea)
	//        ("MakeBoundingBox", &Circle::MakeBoundingBox)
	//		  ;
	// }
	// (end code)
	FunctionCollector Functions;
};


template<typename T>
struct ObjectType::DescriptionHelper<T>::FunctionCollector::FunctionAnnotator : public FunctionCollector
{
	FunctionAnnotator(const char *name)
		: mName(string::SharedString::Literal(name))
	{	
	}
	
	FunctionAnnotator(string::SharedString name)
		: mName(name)
	{	
	}

	struct FunctionNotation
	{
		FunctionNotation(string::SharedString name, const char *key)
			: mName(name)
			, mKey(string::SharedString::Literal(key))
		{
		}
		
		template<typename ValueT>
		FunctionAnnotator operator()(const ValueT &value) const
		{
			TypeOf<T>()->AnnotateStaticValue(mName, mKey, value);
			return FunctionAnnotator(mName);
		}
	private:
		string::SharedString mName, mKey;
	};
	
	FunctionNotation operator[](const char *key) const
	{
		return FunctionNotation(mName, key);
	}
	
private:
	string::SharedString mName;
};

template<typename T>
template<typename FunctionType>
const typename ObjectType::DescriptionHelper<T>::FunctionCollector::FunctionAnnotator ObjectType::DescriptionHelper<T>::FunctionCollector::operator ()(const char *name, FunctionType function) const
{
	TypeOf<T>()->RegisterFunction(name,
		function::CreateFunction(name, function));
	
	return FunctionAnnotator(name);
}

}

#endif
