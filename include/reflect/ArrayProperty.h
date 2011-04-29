#ifndef REFLECT_ARRAYPROPERTY_H_
#define REFLECT_ARRAYPROPERTY_H_

#include <reflect/Property.h>
#include <reflect/config/config.h>

namespace reflect {

class Reflector;
class Variant;

// Class: ArrayProperty
//
// ArrayProperties represent numerically indexed data.
// The data represented may be either resizable, like a vector,
// or not, like a plain array.
//
// See Also:
//   - <Property>
class ReflectExport(reflect) ArrayProperty : public Property
{
	DECLARE_REFLECTION(Property)
public:
	// Function: SerializeItem
	// Reads/writes a single item to the reflector.
	// Used by the <ArrayProperty::Serialize>.
    virtual void SerializeItem(const void *in, void *out, 
      unsigned index, Reflector &reflector) const = 0;

	// Function: Size
	// Gets the current size of the array/vector in the *object*.
	virtual unsigned Size(const void *object) const = 0;

	// Function: Resizable
	// Checks if the array/vector represented by this property is resizable.
	virtual bool Resizable(const void *) const = 0;

	// Function: Resize
	// Resizes the array/vector in the *object*.
	//
	// Returns:
	//    true - if the array size became *size* (even if not resizable)
	//    false - if the array is not resiable and not of size *size*.
	virtual bool Resize(void *, unsigned size) const = 0;

	// Function: ReadData
	// Reads the array and stores the value into the <Variant>.
	//
	// Returns:
	//    true - when the index is valid and the value is read/converted successfully.
	//
	// Example (read an int from a numeric array):
	// > int i;
	// > array_prop->ReadData(opaque_cast(object), 3, Variant::RefValue(i));
	//
	// Example (read an anything and display it):
	// > Variant value;
	// > array_prop->ReadData(opaque_cast(object), 3, value);
	// > printf("object->array_prop[3] is %s of type %s\n", value.ToString().c_str(), value.GetType()->Name());
    virtual bool ReadData(const void *, unsigned item_index, Variant &) const = 0;
	
	// Function: WriteData
	// Writes the <Variant> value to the array.
	//
	// Returns:
	//    true - when the index is valid and the value is written/converted successfully.
    virtual bool WriteData(void *object, unsigned item_index, const Variant &value) const = 0;


	// Function: RefData
	// Refs the <Variant> to the array index.
	//
	// Returns:
	//    true - when the index is valid and the value can be referenced
	//    false - otherwise
    virtual bool RefData(const void *in, void *out, unsigned item_index, Variant &) const = 0;

	// Function: ItemType
	// returns the type of the items in the property.
	virtual Type *ItemType() const = 0;

	// Function: Serialize
	// Serializes the entire array/vector.
	//
	// If the property is <Resizable> then a [size=%d] <SerializationTag::AttributeTag> 
	// will be emitted here.
	//
	// Implements <Property::Serialize>
	void Serialize(const void *in, void *out, Reflector &reflector) const; // virtual
};

}

#endif
