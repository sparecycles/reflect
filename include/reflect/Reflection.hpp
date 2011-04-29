#ifndef REFLECT_REFLECTION_HPP_
#define REFLECT_REFLECTION_HPP_

#include <reflect/Reflection.h>
#include <reflect/Reflector.h>
#include <reflect/Class.h>

// File: Reflection.hpp
//
// Support for defining reflection for types.
//
// All reflected classes need <DECLARE_REFLECTION> in their declaration,
// (non-<Dynamic> types are reflected using <DECLARE_STATIC_REFLECTION> outside),
// these macros are provided in <Reflection.h>.
//
// These macros unconventionally end in a function declaration without braces, 
// the braces, and contents, are expected to be provided to define properties and 
// otherwise initialize the class. The function will be run to setup the class
// when <Class.LoadClasses> is called.
// The function is a member function of the <Class> types <Class::DescriptionHelper>,
// which are designed to simplify registering contructors, properties, and metadata
// about the type. See <PersistentClass::DescriptionHelper.Properties> for the
// prime example of how and why this is useful.
//
// Note: _You must *#include* the .hpp files of any classes you are defining
// reflection for to get the definitions of their DescriptionHelpers._

namespace reflect {

// Macro: DEFINE_REFLECTION
//
// Defines the reflection support for a type using its class' DescriptionHelper.
//
// Parameters:
//   TYPE - The fully qualified type (namespaces and everything).
//   NAME - The name the class will be registered as.
//
// Usage:
// > #include <reflect/Class.hpp>
// >
// > DEFINE_REFLECTION(MyClass, "MyClass")
// > {
// >    ReflectConstructor();
// >    ...
// > }
//
// See Also:
//    - <Dynamic> the base class of all reflected classes.
//    - <DEFINE_TEMPLATE_REFLECTION> for a templated class.
//    - <DEFINE_STATIC_REFLECTION> for a non-dynamic type (struct/primitive).
//    - <DEFINE_CLASS_REFLECTION> to define reflection for a new <Class>.
# define DEFINE_REFLECTION(TYPE, NAME) \
        BUILD_REFLECTION__(TYPE, NAME, TYPE::ClassType, REFLECT_NON_TEMPLATE)

        
// Macro: DEFINE_TEMPLATE_REFLECTION
//
// Defines the reflection support for a templated type using its class' DescriptionHelper. 
//
// The difference between this macro and DEFINE_REFLECTION is the
// template<> specialization markers required for template class defintions.
//
// Parameters:
//   TYPE - The fully qualified type (namespaces and template args).
//   NAME - The name the class will be registered as.
//
// Usage:
// > #include <reflect/Class.hpp>
// >
// > DEFINE_TEMPLATE_REFLECTION(MyClass<3>, "MyClass")
// > {
// >    ReflectConstructor();
// >    ...
// > }
//
// See Also:
//   - <Dynamic> the base class of all reflected classes.
//   - <DEFINE_REFLECTION> to define reflection for a non-templated dynamic type.
//   - <DEFINE_STATIC_REFLECTION> to define reflection for a non-dynamic type.
# define DEFINE_TEMPLATE_REFLECTION(TYPE, NAME) \
        BUILD_REFLECTION__(TYPE, NAME, TYPE::ClassType, REFLECT_TEMPLATE)

// Macro: DEFINE_PRIVATE_REFLECTION
//
// Defines the reflection support for a dynamic type, allowing the reflecting
// (class object) type to be privately specialized.
//
// This is usually used when defining the reflection for subclass of <Class>,
// because the class <Category> of the class does not need to be defined publicly. 
//
// Parameters:
//   TYPE - The class type (fully qualified)
//   NAME - The name the class will be registered as.
//   CLASSTYPE - The type of class reflecting this one.
//     (must subclass the type inferred from the declaration)
//
// See Also:
//   - <DEFINE_REFLECTION> for non-Class dynamic classes.
# define DEFINE_PRIVATE_REFLECTION(TYPE, NAME, CLASSTYPE) \
        BUILD_REFLECTION__(TYPE, NAME, CLASSTYPE, REFLECT_NON_TEMPLATE)

// Macro: DEFINE_STATIC_REFLECTION
//
// Defines reflection for a non-dynamic type.
//
// Parameters:
//   TYPE - The class/struct/primitive type (fully qualified)
//   NAME - The name it will be registered as.
//
// Usage:
// >  DEFINE_STATIC_REFLECTION(MyStruct, "MyStruct")
// >  {
// >    
// >  }

// See Also:
//   - <DEFINE_REFLECTION> for dynamic types.
# define DEFINE_STATIC_REFLECTION(TYPE, NAME) \
        namespace { class REFLECT_UNIQUENAME(Descriptor) \
                : private reflect::Signature<TYPE>::ClassType::DescriptionHelper<TYPE> \
        { \
        public: \
                void Describe() const; \
        }; } \
        static void REFLECT_UNIQUENAME(ClassInitializer__)(); \
        static reflect::Signature<TYPE>::ClassType REFLECT_UNIQUENAME(sClass)(&REFLECT_UNIQUENAME(ClassInitializer__)); \
        namespace reflect { \
                reflect::Signature<TYPE>::ClassType *Signature<TYPE>::TheType() \
                { return &REFLECT_UNIQUENAME(sClass); } \
        } \
        static void REFLECT_UNIQUENAME(ClassInitializer__)() \
        { reflect::TypeOf<TYPE>()->SetName(NAME); \
          REFLECT_UNIQUENAME(Descriptor)().Describe(); } \
        void REFLECT_UNIQUENAME(Descriptor)::Describe() const // ...

# define REFLECT_UNIQUENAME(X) REFLECT_CONCAT_(X, __LINE__)
# define REFLECT_CONCAT_(X,Y) REFLECT_CONCAT__(X,Y)
# define REFLECT_CONCAT__(X,Y) X ## Y

# define REFLECT_NON_TEMPLATE
# define REFLECT_TEMPLATE template <>

# define BUILD_REFLECTION__(TYPE, NAME, CLASSOVERRIDE, TEMPLATE_SPEC) \
        TEMPLATE_SPEC class TYPE::Descriptor : private TYPE::ClassType::DescriptionHelper<TYPE> \
        { \
        public: \
                void Describe() const; \
        }; \
        static void REFLECT_UNIQUENAME(ClassInitializer__)() \
        { reflect::TypeOf<TYPE>()->SetParent(reflect::TypeOf<TYPE::BaseType>()); \
          reflect::TypeOf<TYPE>()->SetName(NAME); \
          TYPE::Descriptor().Describe(); } \
    static CLASSOVERRIDE REFLECT_UNIQUENAME(sClass)(&REFLECT_UNIQUENAME(ClassInitializer__)); \
    TEMPLATE_SPEC TYPE::ClassType *TYPE::TheClass() { return &REFLECT_UNIQUENAME(sClass); } \
    TEMPLATE_SPEC TYPE::ClassType *TYPE::GetClass() const { return TheClass(); } \
    TEMPLATE_SPEC void TYPE::Descriptor::Describe() const // ...

}
#endif
