#ifndef REFLECT_PROPERTY_ACCESSORDIRECTPROPERTY_H_
#define REFLECT_PROPERTY_ACCESSORDIRECTPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {

// Class: AccessorDirectProperty
//
// An implemetation of <DataProperty> using a pair of callbacks
// to read and write a reference.
//
// This is a direct property because the get function returns a const
// reference.
//
// The callbacks must be methods that look like:
// >  const Type &GetMember() const;
// >  void SetMember(const Type &);
template<typename ObjectType, typename MemberType>
class AccessorDirectProperty : public DataProperty
{
public:
	// Constructor: AccessorProperty
	// Creates an accessor property capable of reading and writing a property
	// through the getter and setter pointer-to-member functions provided.
    AccessorDirectProperty(const MemberType &(ObjectType::*)() const, void (ObjectType::*)(const MemberType &));
    
    // virtual 
    void Serialize(const void *in, void *out, Reflector &reflector) const;
    
    // virtual 
    Type *DataType() const;
    
    // virtual 
    bool WriteData(void *object, const Variant &opaque) const;
    
    // virtual 
    bool ReadData(const void *object, Variant &opaque) const;

	// virtual
    bool RefData(const void *in, void *out, Variant &value) const;

private:
	// Member: mGetter
    const MemberType &(ObjectType::*mGetter)() const;

	// Member: mSetter
    void (ObjectType::*mSetter)(const MemberType &);
};

} }

#endif
