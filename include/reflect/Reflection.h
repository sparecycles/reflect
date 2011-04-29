// File: Reflection.h
//
// Defines helper macros for reflecting types.
// The helper macros install typedefs and function stubs.
// The definitions are built by macros in <Reflection.hpp>.
// 
// It is possible to do this with template injection but it makes it difficult to
// describe DLL boundaries correctly, so we don't do that.

#ifndef REFLECT_REFLECTION_H_
#define REFLECT_REFLECTION_H_

#include <reflect/Signature.h>

namespace reflect {

// Macro: DECLARE_REFLECTION
//   Declares reflection for a class.
// 
// Place this macro somewhere in the class to be reflected to enable reflection on it.
// The reflection must be defined using <DEFINE_REFLECTION> in an implementation file somewhere.
// This macro adds both private and public symbols, so put a public/private/protected
// label after it.
//
// Parameters:
//    BASE - The base class this class inherits from.
//
// Usage:
// > class MyClass : public MyBase
// > {
// >    DECLARE_REFLECTION(MyBase)
// > public:
// >    ...
# define DECLARE_REFLECTION(BASE) \
  DECLARE_REFLECTION_EX(BASE, ::reflect::Signature<BASE>::ClassType)

// Macro: DECLARE_TEMPLATE_REFLECTION
//   Declares reflection for a class with a templated base type.
//
// See Also:
//    - <DECLARE_REFLECTION>
//    - <DECLARE_REFLECTION_EX>
# define DECLARE_TEMPLATE_REFLECTION(BASE) \
  DECLARE_REFLECTION_EX(BASE, typename ::reflect::Signature<BASE>::ClassType)

// Macro: DECLARE_REFLECTION_EX
// Declares reflection for a class, changing the ClassType used to reflect the class.
// This is an advanced declaration for introducing new root kinds of dynamic classes.
//
// Parameters:
//    BASE - The base class this class inherits from.
//    CLASSTYPE - The new reflection class.
//
// Usage:
// > class Persistent : public Dynamic, public Serializable
// > {
// >    DECLARE_REFLECTION_EX(MyBase, PersistentClass)
// > public:
// >    ...
# define DECLARE_REFLECTION_EX(BASE, CLASSTYPE) \
public: \
    typedef BASE BaseType; \
    typedef CLASSTYPE ClassType; \
public: \
    static ClassType *TheClass(); \
    /*virtual*/ ClassType *GetClass() const; \
    class Descriptor; \
    friend class Descriptor; \
private: 

// Macro: DECLARE_STATIC_REFLECTION
// Declares reflection for a static type.
// Use this macro outside of any namespace scope.
//
// Parameters:
//    MODULE - The module for <ReflectExport> or "local" if the 
//             type reflection doesn't need to cross dll boundaries.
//    TYPE - The type to reflect.
//    CLASSTYPE - The class type to use to reflect this type.
//                (should be a *Type, not a *Class).
//
// Template specializes <Signature> for the type.
#define DECLARE_STATIC_REFLECTION(MODULE, TYPE, CLASSTYPE) \
        namespace reflect { \
                template<> struct ReflectExport(MODULE) Signature<TYPE> \
                { \
                        typedef CLASSTYPE ClassType; \
                        static ClassType *TheType(); \
                }; \
        }

// Macro: DECLARE_REFLECTION_ALIAS
// Causes one static type to share reflection with another.
// 
// See Also:
//   - <DECLARE_STATIC_REFLECTION>
#define DECLARE_REFLECTION_ALIAS(NEW_TYPE, KNOWN_TYPE) \
        namespace reflect { \
                template<> struct Signature<NEW_TYPE> : public Signature<KNOWN_TYPE> \
                { \
                private: typedef Signature<KNOWN_TYPE> Base; \
                public: \
                        typedef Base::ClassType ClassType; \
                        static ClassType *TheType() { return Base::TheType(); } \
                }; \
        }

// Macro: DECLARE_TEMPLATE_REFLECTION_ALIAS
// Causes a templated static type to share reflection with another.
// 
// See Also:
//   - <DECLARE_STATIC_REFLECTION>
#define DECLARE_TEMPLATE_REFLECTION_ALIAS(TEMPLATE_PARAM, NEW_TYPE, KNOWN_TYPE) \
        namespace reflect { \
                template<TEMPLATE_PARAM> struct Signature<NEW_TYPE> \
                { \
                private: typedef Signature<KNOWN_TYPE> Base; \
                public: \
                        typedef Base::ClassType ClassType; \
                        static ClassType *TheType() { return Base::TheType(); } \
                }; \
        }

} // namespace reflect

// Macro: DECLARE_STATIC_EXTENSION
#define DECLARE_STATIC_EXTENSION(module__, type__, base__) \
	DECLARE_STATIC_REFLECTION(module__, type__, reflect::Signature<base__>::ClassType)


// Macro: DEFINE_LOCAL_STATIC_REFLECTION
#define DEFINE_LOCAL_STATIC_REFLECTION(TYPE, CLASSTYPE, NAME) \
	DECLARE_STATIC_REFLECTION(local, TYPE, CLASSTYPE) \
	DEFINE_STATIC_REFLECTION(TYPE, NAME)

#endif
