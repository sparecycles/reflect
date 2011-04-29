#ifndef REFLECT_SERIALIZER_H_
#define REFLECT_SERIALIZER_H_

#include <reflect/config/config.h>

namespace reflect {

class SerializationTag;
class Dynamic;
class Property;
class Category;
class EnumType;

// Class: Serializer
//
//   Defines the interface for serialization.
class ReflectExport(reflect) Serializer
{
public:
	// Function: Begin
	//
	// Starts a group, annotated by the tag's type and text.
	// If Begin returns true, then it is the callers responsibility to
	// call End with the *same* reference later.
	virtual bool Begin(const SerializationTag &) = 0;

	// Function: End
	//
	// Ends a group started by the tag with Begin.
    virtual bool End(const SerializationTag &) = 0;

	//virtual bool Serialize(const Variant &variant) = 0;

	// Function: Serialize(bool)
	// Emits a bool.
    virtual bool Serialize(bool) = 0;

	// Function: Serialize(long)
	// Emits an integer.
    virtual bool Serialize(long) = 0;

	// Function: Serialize(unsigned long)
	// Emits an integer, maybe in hex.
    virtual bool Serialize(unsigned long) = 0;

	// Function: Serialize(double)
	// Emits a float/double.
    virtual bool Serialize(double) = 0;

	// Function: Serialize(const Dynamic *)
	//
	// The object is serialized using its <Category>
	// (see <Category.SerializeDynamic>), or, if the object has been <Reference>d,
	// then the serializer should emit a backreference instead.
	//
	// Shallow serialization could emit the pointer value (as an integer)
	// for immediate deserialization.
    virtual bool Serialize(const Dynamic *object) = 0;

	// Function: Reference
	// Adds the object to the list of objects known to this serializer.
    //
    // <Reference> should be called in the <Class.SerializePointer>
    // method for classes that construct new, unshared, objects 
    // when deserializing (as opposed to flyweights like <Type> pointers).
    //
    // See Also:
    //    - <Deserializer.Reference>
    virtual bool Reference(const Dynamic *object) = 0;

	// Function: SerializeText
	// Emits a string.
    virtual bool SerializeText(const char *text, unsigned nbytes) = 0;
    
	// Function: SerializeText
	// Emits arbitrary data.
    virtual bool SerializeData(const void *data, unsigned nbytes) = 0;

	// Function: SerializeEnum
	// Emits an enum value or string.
    virtual bool SerializeEnum(int value, const EnumType *type) = 0;

	// Function: SerializeProperty
	virtual bool SerializeProperty(const void *object, const Property *prop) = 0;
	
protected:
    // Destructor: ~Serializer
    virtual ~Serializer();
};

}

#endif
