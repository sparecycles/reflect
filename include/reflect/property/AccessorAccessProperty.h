#ifndef REFLECT_PROPERTY_ACCESSORACCESSPROPERTY_H_
#define REFLECT_PROPERTY_ACCESSORACCESSPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {

// Class: AccessorAccessProperty
//
// An implemetation of <DataProperty> using a pair of callbacks
// to access a reference (const and non-const).
//
// The callbacks must be methods that look like:
// >  const Type &GetMember() const;
// >  Type &GetMember();
template<typename ObjectType, typename Type>
class AccessorAccessProperty : public DataProperty
{
public:
	// Constructor: AccessorProperty
	// Creates an accessor property capable of reading and writing a property
	// through the getter and setter pointer-to-member functions provided.
    AccessorAccessProperty(const Type &(ObjectType::*)() const, Type &(ObjectType::*)());
	
	// virtual
    void Serialize(const void *in, void *out, Reflector &reflector) const;
    
	// virtual
    Class *DataClass() const;
    
	// virtual
    bool WriteData(void *object, const Variant &value) const;
    
	// virtual
    bool ReadData(const void *object, Variant &value) const;

	// virtual
    bool RefData(const void *in, void *out, Variant &value) const;

private:
	// Member: mConstAccess
    const Type &(ObjectType::*mConstAccess)() const;

	// Member: mMutableAccess
    Type &(ObjectType::*mMutableAccess)();
};

} }

#endif
