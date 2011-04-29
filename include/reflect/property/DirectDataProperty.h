#ifndef REFLECT_PROPERTY_DIRECTDATAPROPERTY_H_
#define REFLECT_PROPERTY_DIRECTDATAPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {


// Class: DirectDataProperty
//
// An implemetation of a <DataProperty> using a pointer-to-member
// to access the data.
template<typename ObjectType, typename MemberType>
class DirectDataProperty : public DataProperty
{
public:
    DirectDataProperty(MemberType ObjectType::*);
	
	// virtual
    Type *DataType() const;
	
	// virtual
    bool WriteData(void *object, const Variant &) const;
	
	// virtual
    bool ReadData(const void *object, Variant &) const;

	// virtual
    bool RefData(const void *in, void *out, Variant &value) const;
	
	// virtual
    void Serialize(const void *in, void *out, Reflector &reflector) const;

private:
	// Member: mMember
    MemberType ObjectType::*mMember;
};

} }

#endif
