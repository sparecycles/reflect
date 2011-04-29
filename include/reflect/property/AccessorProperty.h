#ifndef REFLECT_PROPERTY_ACCESSORPROPERTY_H_
#define REFLECT_PROPERTY_ACCESSORPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {

// Class: AccessorProperty
//
// An implemetation of <DataProperty> using a pair of callbacks
// to read and write a direct reference.
//
// The callbacks must be methods that look like:
// > Type GetMember() const;
// > void SetMember(Type);
template<typename ObjectType, typename MemberType>
class AccessorProperty : public DataProperty
{
public:
    AccessorProperty(MemberType (ObjectType::*)() const, void (ObjectType::*)(MemberType));
    
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
    MemberType (ObjectType::*mGetter)() const;

	// Member: mSetter
    void (ObjectType::*mSetter)(MemberType);
};

} }

#endif
