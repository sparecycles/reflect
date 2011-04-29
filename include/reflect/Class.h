#ifndef REFLECT_CLASS_H_
#define REFLECT_CLASS_H_

#include <reflect/ObjectType.h>

namespace reflect {

// Class: DynamicPointerType
class ReflectExport(reflect) DynamicPointerType : public Type
{
	DECLARE_REFLECTION(Type)
public:
	DynamicPointerType();
	
	const Class *ValueClass() const;
	
	/*virtual*/ bool CanConvertFrom(const Class *other) const;
	/*virtual*/ bool ConvertValue(void *opaque, const void *source, const Type *from) const;
	
	void Initialize(const Class *type, Dynamic *(*t2c)(void *), void *(*c2t)(Dynamic *));
	
	/*virtual*/ void Serialize(const void *in, void *out, Reflector &reflector) const;
private:
	Dynamic *(*mTypedToDynamic)(void *);
	void *(*mDynamicToTyped)(Dynamic *);
	const Class *mValueClass;
};

// Class: Class
class ReflectExport(reflect) Class : public ObjectType
{
	DECLARE_REFLECTION(ObjectType)
public:
	template<typename Type>
    class DescriptionHelper;

    Class(void (*init_cb)() = 0);

	// Function: Cast 
	//
	// The object cast to a <Dynamic> if it is a subclass of this,
	// or NULL otherwise. (there are const and non-const versions of this function)
	//
	// There is no little to use this directly.
	//
	// See Also:
	//  - <autocast>.
    Dynamic *Cast(Dynamic *object) const;

    const Dynamic *Cast(const Dynamic *object) const;

	// Function: OpaqueCast
	// Casts a type represented by this Class to a void *, used by
	// <Serialize>. (there are const and non-const versions of this function)
	//
	// See Also:
	// - <OpaqueSerialize>
	// - <TranslucentCast>
	template<typename T>
	static void *OpaqueCast(T *ptr)
	{
		return static_cast<Dynamic *>(ptr);
	}

	template<typename T>
	static const void *OpaqueCast(const T *ptr)
	{
		return static_cast<const Dynamic *>(ptr);
	}

	// Function: TranslucentCast
	// Casts a void * from <OpaqueCast (template/static)> back to a T *.
	// (there are const and non-const versions of this function)
	//
	// See Also:
	// - <OpaqueCast (template/static)>
	template<typename T>
	static T *TranslucentCast(void *ptr)
	{
		return static_cast<T* >(static_cast<Dynamic *>(ptr));
	}

	template<typename T>
	static const T *TranslucentCast(const void *ptr)
	{
		return static_cast<const T *>(static_cast<const Dynamic *>(ptr));
	}
	
	typedef DynamicPointerType PointerClassType;

	PointerClassType *PointerType() const;
	const Class *SerializesAs() const;
	void SetSerializationClass(const Class *);

	// Function: SerializePointer
	virtual void SerializePointer(const Dynamic *in, Reflector &reflector) const;
	
	// Function: DeserializePointer
	virtual void DeserializePointer(Dynamic *&out, Reflector &reflector) const;
protected:
	// virtual
	void Initialize();

private:
	
	// Member: mDynamicPointerType
	// The <Type> object representing
	// pointers to this type.
	mutable DynamicPointerType mDynamicPointerType;
	
	// Member: mSerializationClass
	// The Class that pointers use when serializing.
	// (*this* <Type.Derives> mSerializationClass).
	const Class *mSerializationClass;
};

}

#endif
