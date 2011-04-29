#ifndef REFLECT_DATAPROPERTY_H_
#define REFLECT_DATAPROPERTY_H_

#include <reflect/Property.h>
#include <reflect/Variant.h>
#include <reflect/config/config.h>

namespace reflect {

class Reflector;

// Class: DataProperty
// 
// Properties represent a single data element.
//
// Implemented By:
//    - <DirectDataProperty> - Pointer to member data property.
//    - <AccessorProperty> - Access through value get-return/set-param functions.
//    - <AccessorDirectProperty> - Access through reference get-return/set-param functions.
//    - <AccessorAccessProperty> - Access through reference get-return/get-return functions.
//    - <AccessorIndirectProperty> - Access through reference get-param/set-param functions
//    - <BaseClassProperty> - Access through upcasting function.
//
// See Also:
//    - <Property> for other property types.
class ReflectExport(reflect) DataProperty : public Property
{
	DECLARE_REFLECTION(Property)
public:
	// Function: DataType
	// The type of data represented by this property.
    virtual Type *DataType() const = 0;
    
    // Function: WriteData
    // Writes this property with the <Variant> *value*.
    virtual bool WriteData(void *object, const Variant &value) const = 0;
    
    // Function: ReadData
    // Reads this property with the <Variant> *value*.
    virtual bool ReadData(const void *, Variant &) const = 0;

    // Function: RefData
    // Refs this property into the <Variant> *value*.
    virtual bool RefData(const void *, void *, Variant &) const = 0;

	// Default implementation of Serialize
	// This would be more efficient to override again in an implementation subclass.
	void Serialize(const void *in, void *out, reflect::Reflector &reflector) const; // virtual
};

}

#endif
