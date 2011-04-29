#ifndef REFLECT_MAPPROPERTY_H_
#define REFLECT_MAPPROPERTY_H_

#include <reflect/Property.h>
#include <reflect/Variant.h>
#include <reflect/config/config.h>

namespace reflect {

class Reflector;
class Serializer;
class Deserializer;

// Class: MapProperty
//
// MapProperties represent key indexed data.
//
// MapProperties provide key-based access and
// complete iteration through a callback.
//
// See Also:
//   - <Property>
class ReflectExport(reflect) MapProperty : public Property
{
	DECLARE_REFLECTION(Property)
public:
	// virtual
	void Serialize(const void *in, void *out, Reflector &reflector) const;

    // Function: ForEachKey
    // Calls a callback with every key in the map.
    // This is the only way to iterate over the map.
	virtual void ForEachKey(const void *in, void (*)(const Variant &key, void *ctx), void *ctx = 0) const = 0;

    // Function: HasKey
	// Checks if the map has an element for the given *key*.
	virtual bool HasKey(const void *in, const Variant &key) const = 0;
	
	// Function: ReadData
	// Reads the value for *key*.
	//
	// Returns:
	//    true - if the value is successfully read.
	//    false - if the key doesn't exist, if the key is incompatible,
	//            or if the value can't be copied into value.
	virtual bool ReadData(const void *in, const Variant &key, Variant &value) const = 0;
	
	// Function: WriteData
	// Writes a (possibly new) map entry.
	//
	// Returns:
	//    true - if the value is successfully written.
	//    false - if the key is incompatible or the value is incompatible,
	//            or if the map cannot be extended with the new key.
	virtual bool WriteData(void *in, const Variant &key, const Variant &value) const = 0;
	
	// Function: KeyType
	// Retrieves the type of keys represented by this map property.
	virtual const Type *KeyType() const = 0; 
	
	// Function: ValueType
	// Retrieves the type of values represented by this map property.
	virtual const Type *ValueType() const = 0; 
};

}

#endif
