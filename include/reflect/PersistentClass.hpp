#ifndef REFLECT_PERSISTENTCLASS_HPP_
#define REFLECT_PERSISTENTCLASS_HPP_

#include <reflect/PersistentClass.h>
#include <reflect/Class.hpp>
#include <reflect/string/String.h>

#include <reflect/PrimitiveTypes.h>
#include <reflect/property/DirectDataProperty.hpp>
#include <reflect/property/DirectArrayProperty.hpp>
#include <reflect/property/DirectVectorProperty.hpp>
#include <reflect/property/DirectMapProperty.hpp>

#include <reflect/property/AccessorProperty.hpp>
#include <reflect/property/AccessorAccessProperty.hpp>
#include <reflect/property/AccessorDirectProperty.hpp>
#include <reflect/property/AccessorIndirectProperty.hpp>

#include <reflect/property/BaseClassProperty.hpp>
#include <reflect/property/VariantProperty.hpp>

namespace reflect {

// Class: PersistentClass::DescriptionHelper
//
// Helper class for <Persistent> reflection definitions.
//
// See Also:
//   - <PersistentClass>
//   - <Class::DescriptionHelper>
//   - <DEFINE_REFLECTION>
template<typename T>
class PersistentClass::DescriptionHelper : public Class::DescriptionHelper<T>
{
public:
		
	// Function: ReflectConstructor
	//
	// This function is available in the reflection definition.
	// to register the default constructor as a factory.
	// If this is not called, the class will be considered
	// <Class.Abstract> for the purposes of reflection.
	//
	// Calling this function requires (at compile time) a default constructor
	// for the class.
	inline void ReflectConstructor() const
	{
		+ Class::DescriptionHelper<T>::Concrete;
		//Features
		//	+ Concrete
		//	;
	}	

	struct ArrayAnnotation {} Array;
	struct MapAnnotation {} Map;
	struct VariantAnnotation {} Variant;
	
	struct PropertyCollector
	{
		struct PropertyAnnotator;
		
		template<typename BaseObjectType>
		PropertyAnnotator base(const char *name) const
		{
			struct BaseHelper
			{
				static BaseObjectType *Cast(void *persistent)
				{
					return
						static_cast<BaseObjectType *>(
						translucent_cast<T *>(persistent));
				}
			};
			
			return PropertyAnnotator(name,
				new property::BaseClassProperty<BaseObjectType>(&BaseHelper::Cast));
		}
		
		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, MemberType T::*pmember) const
		{
			return PropertyAnnotator(name,
				new property::DirectDataProperty<Persistent, MemberType>(
					static_cast<MemberType Persistent::*>(pmember)));
		}

		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, MemberType T::*pmember, const ArrayAnnotation &) const
		{
			return PropertyAnnotator(name,
				new property::DirectVectorProperty<Persistent, MemberType>(
					static_cast<MemberType Persistent::*>(pmember)));
		}

		template<typename MemberType, unsigned ArraySize>
		PropertyAnnotator operator ()(const char *name, MemberType (T::*pmember)[ArraySize], const ArrayAnnotation &) const
		{
			return PropertyAnnotator(name,
				new property::DirectArrayProperty<Persistent, MemberType>(
					reinterpret_cast<MemberType (Persistent::*)[]>(
					static_cast<MemberType (T::*)[ArraySize]>(pmember)), ArraySize));
		}
	
		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, MemberType T::*pmember, const MapAnnotation &) const
		{
			return PropertyAnnotator(name,
				new property::DirectMapProperty<Persistent, MemberType>(
					static_cast<MemberType Persistent::*>(pmember)));
		}
		
		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, MemberType (T::*getter)() const, void (T::*setter)(MemberType)) const
		{
			return PropertyAnnotator(name,
				new property::AccessorProperty<Persistent, MemberType>(
					static_cast<MemberType (Persistent::*)() const>(getter),
					static_cast<void (Persistent::*)(MemberType)>(setter)));
		}

		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, const MemberType &(T::*getter)() const, MemberType &(T::*setter)()) const
		{
			return PropertyAnnotator(name,
				new property::AccessorAccessProperty<Persistent, MemberType>(
					static_cast<const MemberType &(Persistent::*)() const>(getter),
					static_cast<MemberType &(Persistent::*)()>(setter)));
		}
		
		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, const MemberType &(T::*getter)() const, void (T::*setter)(const MemberType &)) const
		{
			return PropertyAnnotator(name,
				new property::AccessorDirectProperty<Persistent, MemberType>(
					static_cast<const MemberType &(Persistent::*)() const>(getter),
					static_cast<void (Persistent::*)(const MemberType &)>(setter)));
		}
		
		template<typename MemberType>
		PropertyAnnotator operator ()(const char *name, void (T::*getter)(MemberType &) const, void (T::*setter)(const MemberType &)) const
		{
			return PropertyAnnotator(name,
				new property::AccessorIndirectProperty<Persistent, MemberType>(
					static_cast<void (Persistent::*)(MemberType &) const>(getter),
					static_cast<void (Persistent::*)(const MemberType &)>(setter)));
		}
		
		// :DISGUSTING HACK: ReflectorType is always Reflector:
		//     must templatize the method to work around crash in MSVC 2008 Debug compile.
		template<typename ReflectorType>
		PropertyAnnotator operator ()(const char *name, void (T::*vser)(ReflectorType &) const, void (T::*vdeser)(ReflectorType &), const VariantAnnotation &) const
		{
			return PropertyAnnotator(name,
				new property::VariantProperty<Persistent>(
					static_cast<void (Persistent::*)(Reflector &) const>(vser),
					static_cast<void (Persistent::*)(Reflector &)>(vdeser)));
		}
	};

	// Member: Properties
	//  Used to help register
	//  properties in a reflection definition.
	// 
	// For example, the following sample shows how to register three member variables,
	// the first two are plain data such as ints or floats, the third has to be
	// an array or a class that acts like std::vector.
	//
	// The reflection definition is friended in <DECLARE_REFLECTION> to have private
	// access to members. 
	//
	// Usage:
	// (begin code)
	// namespace example {
	// 
	// class MyClass : public reflect::Persistent
	// {
	//     DECLARE_REFLECTION(reflect::Persistent)
	// private:
	//     int mX;
	//     reflect::String mY;
	//     std::vector<int> mThingVector;
	// };
	// 
	// }
	//
	// DEFINE_REFLECTION(example::MyClass, "example::MyClass")
	// {
	//     ReflectConstructor();
	//     
	//     Properties
	//        ("x", &example::MyClass::mX)
	//            ["tooltip"]("left-right location")
	//        ("y", &example::MyClass::mY)
	//        ("things", &example::MyClass::mThingVector, Array)
	//            ["tooltip"]("up-down location")
	//            ["tooltip"]("up-down location")
	//        ;
	//     ...etc...
	// }
	// (end code)
	//
	// The ["key"]("value") syntax can be used to register metadata about properties.
	// They are not used by anything in reflect itself. They can be accessed by
	// <PropertyPath::Annotation>.
	//
	// The following is a list of the types of registration
	// with the type of Property created.
	//
	//   ("name", &MyClass::mVar) - <DirectDataProperty> 
	//   ("name", &MyClass::GetVar, &MyClass::SetVar) - <AccessorProperty>
	//   ("name", &MyClass::GetVarRef, &MyClass::SetVarRef) - <AccessorDirectProperty>
	//   ("name", &MyClass::ReadVar, &MyClass::WriteVar) - <AccessorIndirectProperty>
	//   ("name", &MyClass::VarConst, &MyClass::Var) - <AccessorAccessProperty>
	//   ("name", &MyClass::mArrayVar, Array) - <DirectArrayProperty> 
	//   ("name", &MyClass::mVectorVar, Array) - <DirectVectorProperty>
	//   ("name", &MyClass::mVar, Map) - <DirectMapProperty>
	//   ("name", &MyClass::ser_fun, &MyClass::deser_fun, Variant) - <VariantProperty>
	PropertyCollector Properties;
};

template<typename T>
struct PersistentClass::DescriptionHelper<T>::PropertyCollector::PropertyAnnotator
	: public PropertyCollector
{	
	PropertyAnnotator(const char *name, Property *prop = 0)
		: mName(string::SharedString::Literal(name))
	{
		if(prop)
			T::TheClass()->RegisterProperty(name, prop);
	}

	PropertyAnnotator(string::SharedString name)
		: mName(name)
	{
	}

	struct PropertyNotation
	{
		PropertyNotation(string::SharedString propname, const char *key)
			: mPropname(propname)
			, mNotationKey(string::SharedString::Copy(key))
		{
		}
		
		template<typename ValueT>
		PropertyAnnotator operator ()(const ValueT &value)
		{
			TypeOf<T>()->AnnotateStaticValue(
				mPropname, mNotationKey,
				value);

			return PropertyAnnotator(mPropname);
		}
		
		PropertyAnnotator f(const char *format, ...)
		{
			string::String text;
			std::va_list args;
			va_start(args, format);
			text.vformat(format, args);
			va_end(args);
			
			TypeOf<T>()->Annotate(
				mPropname, mNotationKey,
				text);
			
			return PropertyAnnotator(mPropname);
		}
		
		string::SharedString mPropname, mNotationKey;
	};
	
	PropertyAnnotator &set(const char *, const char *)
	{
		return *this;
	}

	PropertyNotation operator[](const char *name) const
	{
		return PropertyNotation(mName, name);
	}
	
	string::SharedString mName;
	Property *mProp;
};


}

#endif
