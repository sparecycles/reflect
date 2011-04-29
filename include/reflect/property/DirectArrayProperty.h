#ifndef REFLECT_PROPERTY_DIRECTARRAYPROPERTY_H_
#define REFLECT_PROPERTY_DIRECTARRAYPROPERTY_H_

#include <reflect/ArrayProperty.h>

namespace reflect { namespace property {

// Class: DirectArrayProperty
//
// An implemetation of <ArrayProperty> using a pointer-to-member
// to access the array.
template<typename ObjectType, typename MemberType>
class DirectArrayProperty : public ArrayProperty
{
public:
    DirectArrayProperty(MemberType (ObjectType::*)[], unsigned size);

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
    MemberType (ObjectType::*mMember)[];

   	// Member: mSize
	unsigned mSize;
};

} }

#endif
