#ifndef REFLECT_PROPERTY_H_
#define REFLECT_PROPERTY_H_

#include <reflect/config/config.h>
#include <reflect/Dynamic.h>
#include <reflect/Reflection.h>
#include <reflect/string/SharedString.h>
#include <map>

namespace reflect {

namespace string { class SharedString; }

class Reflector;

// Class: Property
//
// Properties represent data in a reflected type, and can be
// used to <Serialize> that data through a <Reflector>.
//
// Some reflected subclasses of Property provide more detailed
// access.
// 
//   - <DataProperty> represents a single element.
//   - <ArrayProperty> represents a indexed sequence of elements of the same type.
//                     this may be resizable like a vector, or fixed like an array.
//   - <MapProperty> represents a key/value map of elements.
class ReflectExport(reflect) Property : public Dynamic
{
	DECLARE_REFLECTION(Dynamic)
public:
	// Function: Serialize
	//
	// Reads or writes the represented data using a <Reflector>.
	// 
	// Parameters:
	//   in - const object for the reflector to read if <reflector.Serializing()>
	//   out - mutable object for the reflector to write to if <reflector.Deserializing()>
    virtual void Serialize(const void *in, void *out, Reflector &reflector) const = 0;
};

}

#endif
