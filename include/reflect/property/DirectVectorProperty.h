#ifndef REFLECT_DIRECTVECTORPROPERTY_H_
#define REFLECT_DIRECTVECTORPROPERTY_H_

#include <reflect/ArrayProperty.h>

namespace reflect { namespace property {


// Class: DirectVectorProperty
//
// An implemetation of <ArrayProperty> using a pointer-to-member
// to access the vector.
template<typename ObjectType, typename MemberType>
class DirectVectorProperty : public ArrayProperty
{
public:
    DirectVectorProperty(MemberType ObjectType::*);

	// virtual
    void SerializeItem(const void *in, void *out, unsigned index, Reflector &reflector) const;

	// virtual
	unsigned Size(const void *) const;

	// virtual
	bool Resize(void *, unsigned) const;

	// virtual
	bool Resizable(const void *) const;

	// virtual
	Type *ItemType() const;

	// virtual
	bool ReadData(const void *in, unsigned index, Variant &variant) const;

	// virtual
	bool WriteData(void *out, unsigned index, const Variant &variant) const;

	// virtual
	bool RefData(const void *in, void *out, unsigned index, Variant &value) const;

private:
	// Member: mMember
    MemberType ObjectType::*mMember;
};

} }

#endif
