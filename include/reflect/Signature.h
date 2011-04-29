#ifndef REFLECT_SIGNATURE_H_
#define REFLECT_SIGNATURE_H_

namespace reflect {

class Reflector;

// Class: Signature
//
// Signatures provide a uniform means of accessing the
// <Class> object and the static type of that class
// of any reflected type, dynamic (<Dynamic>) or not.
//
// Since it is a templated struct, it must be specialized for every static
// type it reflects (this is done in <DECLARE_STATIC_REFLECTION>),
// its default implementation covers the dynamic types.
//
// Its main use is for getting the class of a type, which can be done with
// > Signature<Type>::TheClass()
// However, since it is a templated struct, this is somewhat awkward. 
// It is suggested that you use <TypeOf> instead,
// which is easier to use inside a template and saves typeing (pun!).
//
// The functions <opaque_cast> and <translucent_cast> turn typed pointers
// into generic (opaque) pointers and back.  This loses typeinfo, for a generic way 
// to pass typed data around, see <Variant>.
template<typename T>
struct Signature
{
	// Typedef: ClassType
	// This is the public static type of the <Class>
	// represented by this signature type.
	typedef typename T::ClassType ClassType;
	
	// Function: TheClass
	// Returns the <Class> of the type represented by this signature.
	static inline ClassType *TheType() { return T::TheClass(); }
};

// default specialization of pointer types, sometimes overridden.
template<typename T>
struct Signature<T *>
{
	typedef typename T::ClassType::PointerClassType ClassType;
	static inline ClassType *TheType() { return T::TheClass()->PointerType(); }
};

// Section: Helper Functions
// These functions help automate the common uses of the <Signature> type.

// Function: TypeOf
// Templated function, returns the <Class> of a reflected type.
//
// For Example:
// | Class *type = TypeOf<int>();
// | if(TypeOf<MyObject>()->Derives<Persistent>()) ...
template<typename T>
inline typename Signature<T>::ClassType *TypeOf() { return Signature<T>::TheType(); }

// Function: TypeOf (by value)
// Returns the <Class> type of an expression with a reflected type.
template<typename T>
inline typename Signature<T>::ClassType *TypeOf(const T &) { return TypeOf<T>(); }

// Function: TypeOf (by array value)
// Returns a pointer type for an array. (so TypeOf("Hello") works as expected).
template<typename T, unsigned size>
inline typename Signature<T>::ClassType *TypeOf(const T (&)[size]) { return TypeOf<T*>(); }

// Function: opaque_cast
//
// Turns a typed pointer (of type Type *) into an opaque pointer (a void *),
// for use with the class <Signature::TheClass>.
// There are const and non-const versions of this function.
//
// See Also:
//   - <Class::OpaqueCast>
//   - <translucent_cast>
template<typename T>
inline void *opaque_cast(T *value) { return Signature<T>::ClassType::OpaqueCast(value); }

template<typename T>
inline const void *opaque_cast(const T *value) { return Signature<T>::ClassType::OpaqueCast(value); }

// generates dereferenced, possibly unconsted pointer type,
// used in translucent_cast below so that translucent_cast<type *> can be used as a drop-in replacement
// for static_cast<type *>.
template<typename T> struct DereferenceType;
template<typename T> struct DereferenceType<T *> { typedef T Result; };
template<typename T> struct DereferenceType<const T *> { typedef T Result; };

// Function: translucent_cast
//
// Turns an opaque pointer (of type void *) into a
// typed pointer of the specified type.
// There are const and non-const versions of this function.
//
// Implemented by <Class::TranslucentCast>.
// 
// Paired with <opaque_cast>.
template<typename Type>
inline Type translucent_cast(void *value)
{
	typedef typename DereferenceType<Type>::Result DereferencedType;
	return Signature<DereferencedType>::ClassType::template TranslucentCast<DereferencedType>(value);
}

template<typename Type>
inline Type translucent_cast(const void *value)
{
	typedef typename DereferenceType<Type>::Result DereferencedType;
	return Signature<DereferencedType>::ClassType::template TranslucentCast<const DereferencedType>(value);
}

} // namespace reflect

#endif
