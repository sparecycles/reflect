#ifndef REFLECT_PROPERTY_ACCESSORINDIRECTPROPERTY_H_
#define REFLECT_PROPERTY_ACCESSORINDIRECTPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {

// Class: AccessorIndirectProperty
//
// An implemetation of <DataProperty> using a pair of callbacks
// to read and write an indirect reference.
//
// This property is indirect because the internal data type is
// never exposed (a reference is never returned).
//
// The callbacks must be methods that look like:
// > void Get(Type &) const;   
// > void Set(const Type &);
//
// See Also:
//   - <PersistentClass::DescriptionHelper::Properties>
//      to register a property of this type.
template<typename ObjectType, typename MemberType>
class AccessorIndirectProperty : public DataProperty
{
public:
	// Constructor: AccessorIndirectProperty
	// Constructs an AccessorIndirectProperty
    AccessorIndirectProperty(void (ObjectType::*)(MemberType &) const, void (ObjectType::*)(const MemberType &));
    
    // virtual
    void Serialize(const void *in, void *out, Reflector &reflector) const;
    
    // virtual
    Type *DataType() const;
    
    // virtual
    bool WriteData(void *object, const Variant &) const;
    
    // virtual
    bool ReadData(const void *object, Variant &) const;

	// virtual
    bool RefData(const void *in, void *out, Variant &value) const;

private:
	// Member: mGetter
    void (ObjectType::*mGetter)(MemberType &) const;
    
	// Member: mSetter
    void (ObjectType::*mSetter)(const MemberType &);
};

} }

#endif
