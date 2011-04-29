#ifndef REFLECT_VARIANT_H_
#define REFLECT_VARIANT_H_

#include <reflect/Signature.h>
#include <reflect/Type.h>
#include <reflect/config/config.h>

namespace reflect { namespace string {
class String;
class Fragment;
} }

namespace reflect {

// Class: Variant
//
// Variants represent objects of any reflected type.
// Some variants own their data, others reference data.
// They may be blank, have a type, a type and a const value,
// or a type and a mutable value,
// or be alias another variant of another type (see: <Alias>).
//
// Variants are used in the property system for reading
// and writing data without a serializer, and by the function 
// call system for passing arguments.  Variants can be used
// any time an interface with specific typeinfo needs to be
// exposed with a generic interface.
//
// Variants share data.  A copy of a reference variant
// (created with <Variant::Ref> or <Variant::ConstRef>)
// will refer to the same data.
// A copy of a constructed variant (created with <Variant::Copy> or
// a blank one after <SetValue> or <Construct>) will share
// the object, the last one to destruct will deallocate it.
// This pattern makes it possible to return
// variants from functions.
// Operations like <operator=> or <Construct> unshares the variant.
//
// Reference counting is achieved using a circular singly-linked list.
class ReflectExport(reflect) Variant
{
public:
	// Constructor: Variant() (constructor)
	// constructs a blank variant.
	Variant();
	
	// Constructor: Variant() (copy constructor)
	// clones another variant.
	Variant(const Variant &other);
	
	~Variant();
	
	// Function: operator=
	// copies another variant
	const Variant &operator=(const Variant &other);

	// Function: FromRef
	// Builds a Variant which can references a value.
	// The value must persist as long as this Variant
	// (or copies of this variant) does.
	template<typename T>
	static Variant FromRef(T &value);

	// Function: FromConstRef
	// Builds a Variant which wraps a const value.
	// The value must persist as long as this Variant
	// (or copies of this variant) does.
	template<typename T>
	static Variant FromConstRef(const T &value);
    
	// Function: ConstRef
	// Builds a Variant which wraps a const value array (as a pointer).
	// This is function is primarily intended to be used with string literals,
	// which (without casting) are statically typed as const arrays.
	//
	// NOTE: This allocates a pointer to point to the array.
	template<typename T, unsigned size>
	static Variant FromConstRef(const T (&value)[size])
	{
		Variant array_variant;
		const T *&pointer = *reinterpret_cast<const T **>(array_variant.mAllocation = new char[sizeof(const T *)]);
		pointer = value;
		array_variant.mConstData = opaque_cast(&pointer);
		array_variant.mType = TypeOf<T *>();
		return array_variant.Downcast();
    }

	// Function: FromValue
	// Creates a variant holding a copy of the value.
	template<typename T>
	static Variant FromValue(const T &value) 
	{
		Variant var;
		var.SetValue(value);
		return var.Downcast();
    }

	// Function: FromStaticValue
	// Creates a variant holding a copy of the value, without using
	// reflection at all.
	template<typename T>
	static Variant FromStaticValue(const T &value) 
	{
		Variant var;
		var.BindType(TypeOf<T>());
		var.mConstData = var.mData = opaque_cast(new T(value));
		return var;
    }

	template<typename T, unsigned size>
	static Variant FromStaticValue(const T (&value)[size])
	{
		Variant array_variant;
		const T *&pointer = *reinterpret_cast<const T **>(array_variant.mAllocation = new char[sizeof(const T *)]);
		pointer = value;
		array_variant.mConstData = opaque_cast(&pointer);
		array_variant.mType = TypeOf<T *>();
		return array_variant;
    }

	// Function: FromOpaque
	// Builds a variant from an Type *, opaque pair.
	static Variant FromOpaque(const Type *type, void *opaque); 

	// Function: FromConstOpaque
	// Builds a variant from an Type *, opaque pair.
	static Variant FromConstOpaque(const Type *type, const void *opaque); 

	// Function: BindType
	//
	// Sets the class type for this variant.
	// 
	// The bound class determins the datatype to construct
	// after <Set> or <Alias>
	void BindType(const Type *type);

	// Function: GetType
	// Check the type of this variant.	
	const Type *GetType() const;

	// Function: Construct
	//
	// Ensures that this variant can be <Set>.
	//
	// Parameters:
	//   *type* - the class type to construct as, if not specified
	//      the currently bound type will be used.
	//   *own_data* - if true, this variants data will not be shared with
	//      any other variant or object.
	bool Construct(const Type *type = 0, bool own_data = false);
	
	// Function: ConvertToType
	// Converts this variant to another type,
	// keeping the same (or "similar") value.
	bool ConvertToType(const Type *type);
	
	// Function: Set
	// Assigns a value to the variant.
	// 
	// - constructs and assigns new data (of the type specified in <BindType> if available, or *value*)
	//   if this variant does not refer to another value.
	// - assigns current data if this variant refers to a non-const value.
	// - fails otherwise.
	bool Set(const Variant &value);

	// Function: Ref
	// Assigns a reference to the variant.
	// 
	// - requires a typed or blank, empty (no data) variant.
	// - if the variant is typed, this does not change the type
	bool Ref(const Variant &value);

	// Function: ConstRef
	// Assigns a const reference to the variant.
	// 
	// - requires a typed or blank, empty (no data) variant.
	// - if the variant is typed, this does not change the type
	bool ConstRef(const Variant &value);
	
	// Function: Alias
	// This is a special function, for reflected parameters to reflected function calls.
	//
	// This variant must have it's type set with <BindType>, as the visible
	// type of the function parameter.  The *value* must be a derived type or convertable to
	// the bound type and back. In the latter case (just assume it), the aliased value will not be
	// updated until <UpdateAliases> is called.  See <VariantAliasRef>, which does that.
	bool Alias(Variant &value);
	
	// Function: Alias
	// This is a special function, for reflected parameters to reflected function calls.
	//
	// This variant must have it's type set with <BindType>, as the visible
	// type of the function parameter.  The *value* must be a derived type or convertable to
	// to bound type.  This will be the same as a ConstRef() when possible or a Set() when necessary.
	bool ConstAlias(const Variant &value);
	
	// Function: CanRefAs
	// checks if this variant can be used as a reference to *type*.
	bool CanRefAs(const Type *type) const;
	
	// Function: CanConstRefAs
	// checks if this variant can be used as a const reference to *type*.
	bool CanConstRefAs(const Type *type) const;
	
	// Function: CanReadAs
	// checks if this variant is convertable to type.
	bool CanReadAs(const Type *type) const;
	
	// Function: CanWriteAs
	// checks if this variant is convertable from type.
	bool CanWriteAs(const Type *type) const;
	
	// Function: Serialize
	// reads or writes this variant with a reflector.
	// Requires a type and should not be const.
	void Serialize(Reflector &reflector) const;

	// Function: ToString
	// Serializes with the <StandardSerializer>.
	string::String ToString() const;

	// Function: FromString
	// Deserializes with the <StandardSerializer>,
	// at least a <Type> must be bound.
	//
	// Returns:
	//   true - if deserialization succeeded.
	bool FromString(string::Fragment);

	// Function: BindType()
	// Template version of <BindType>.
	template<typename T>
	void BindType() { BindType(TypeOf<T>()); }
	
	// Function: Construct()
	// Template version of <Construct>.
	template<typename T>
	bool Construct(bool own_data = false) { return Construct(TypeOf<T>(), own_data); }

	// Function: ConvertToType()
	// Template version of <ConvertToType>.
	template<typename T>
	bool ConvertToType() { return ConvertToType(TypeOf<T>()); }

	// Function: CanRefAs()
	// Template version of <CanRefAs>.
	template<typename T>
	bool CanRefAs() const { return CanRefAs(TypeOf<T>()); }

	// Function: CanRefAs()
	// Template version of <CanRefAs>.
	template<typename T>
	bool CanConstRefAs() const { return CanConstRefAs(TypeOf<T>()); }

	// Function: CanReadAs()
	// Template version of <CanReadAs>.
	template<typename T>
	bool CanReadAs() const { return CanReadAs(TypeOf<T>()); }

	// Function: CanWriteAs()
	// Template version of <CanWriteAs>.
	template<typename T>
	bool CanWriteAs() const { return CanWriteAs(TypeOf<T>()); }
	
	// Function: SetValue
	// Template version of <Set>.
	template<typename T>
	bool SetValue(const T &value)
	{
		return Set(Variant::FromConstRef(value));
	}

	// Function: RefValue
	// Template version of <Ref>.
	template<typename T>
	bool RefValue(T &value);

	// Function: ConstRefValue
	// Template version of <ConstRef>.
	template<typename T>
	bool ConstRefValue(const T &value);
	
	// Function: ReadValue
	// Reads this variant as typeof(*value*).  Can be used to export the
	// value of this variant as a particular type.
	template<typename T>
	bool ReadValue(T &value) const;

	// Function: AsValue()
	// Returns the value of this variant as the requested *type*.
	// Check that <CasReadAs> is true before calling this function.
	template<typename T>
	T AsValue() const;

	// Function: AsConstRef()
	// Returns the value of this variant as the requested *type*.
	// Check that <CasConstRefAs> is true before calling this function.
	template<typename T>
	const T &AsConstRef() const;
	
	// Function: AsRef()
	// Returns the value of this variant as the requested *type*.
	// Check that <CasRefAs> is true before calling this function.
	template<typename T>
	T &AsRef() const;
	
	// Function: UpdateAliases
	// Updates aliased variants with this one's value.
	// Used with invoking function calls with slightly wrongly
	// typed output parameters.
	void UpdateAliases() const;

	// Function: Downcast
	// Convert type and pointer to most derived form
	// (when pointing to some kind of <Dynamic>).
	Variant &Downcast();

    // Function: Void
    // Returns the variant representing a void "value." 
	static Variant &Void();
	
    // Function: Opaque
	void *Opaque() const;

    // Function: ConstOpaque
	const void *ConstOpaque() const;
private:
	Variant(void *opaque, const Type *type);
	Variant(const void *opaque, const Type *type);

	void Assimilate(Variant &value);
	void Share(const Variant &other);
	void Release();
	void ClearAllocation();
	
	char *mAllocation;
	const Type *mType;
	const void *mConstData;
	void *mData;
	mutable Variant *mLink;
};

template<>
inline bool Variant::SetValue(const Variant &value)
{
	return Set(value);
}

/////////////////////////////////////
class VariantCast
{
public:
	VariantCast(const reflect::Variant &value) : mValue(value) {}

	template<typename T> operator T() const;
private:
	const reflect::Variant &mValue;
};

static const struct variant_caster { variant_caster(){} } variant_cast;

inline 
VariantCast operator %(const reflect::Variant &variant, const variant_caster &)
{
	return VariantCast(variant);
}

template<typename T>
inline 
Variant Variant::FromRef(T &value) 
{ 
	return Variant(opaque_cast(&value), static_cast<Type *>(TypeOf(value))).Downcast(); 
}

template<typename T>
inline
bool Variant::ReadValue(T &value) const
{ return Variant::FromRef(value).Set(*this); }

template<typename T>
inline
T Variant::AsValue() const
{
	T value;
	if(!ReadValue(value) && !TypeOf<T>()->Abstract())
	{
		TypeOf<T>()->Destruct(opaque_cast(&value));
		TypeOf<T>()->Construct(opaque_cast(&value));
	}
	return value;
}

template<typename T> 
inline
VariantCast::operator T() const
{
	if(mValue.CanReadAs<T>())
	{
		return mValue.AsValue<T>();
	}
	else
	{
		Variant v;
		v.BindType(TypeOf<T>());
		v.FromString(mValue.AsValue<reflect::string::Fragment>());
		return v.AsConstRef<T>();
	}
}

template<typename T>
inline
const T &Variant::AsConstRef() const
{
	const T &value = *translucent_cast<const T *>(mConstData);
	return value;
}

template<typename T>
inline
bool Variant::RefValue(T &value)
{
	return Ref(Variant::FromRef(value));
}

template<typename T>
inline
T &Variant::AsRef() const
{
	T &value = *translucent_cast<T *>(mData);
	return value;
}

template<typename T>
inline
Variant Variant::FromConstRef(const T &value) 
{ 
	return Variant(opaque_cast(&value), TypeOf(value)).Downcast(); 
}

template<typename T>
inline
bool Variant::ConstRefValue(const T &value)
{
	return Ref(Variant::FromConstRef(value));
}

inline
void *Variant::Opaque() const { return mData; }

inline
const void *Variant::ConstOpaque() const { return mConstData; }

}

#endif
