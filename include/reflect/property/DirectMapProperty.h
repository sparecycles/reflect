#ifndef REFLECT_DIRECTMAPPROPERTY_H_
#define REFLECT_DIRECTMAPPROPERTY_H_

#include <reflect/MapProperty.h>

namespace reflect { namespace property {

// Class: DirectMapProperty
//
// An implemetation of <MapProperty> using a pointer-to-member
// to access the map.
template<typename ObjectType, typename MemberType>
class DirectMapProperty : public MapProperty
{
public:
    DirectMapProperty(MemberType ObjectType::*);

	// virtual
	void ForEachKey(const void *in, void (*)(const Variant &key, void *ctx), void *ctx = 0) const;

	// virtual
	bool HasKey(const void *in, const Variant &key) const;

	// virtual
	bool ReadData(const void *in, const Variant &key, Variant &value) const;

	// virtual
	bool WriteData(void *out, const Variant &key, const Variant &value) const;

	// virtual
	bool RefData(const void *in, void *out, const Variant &key, Variant &value) const;
	
	// virtual
	const Type *KeyType() const;

	// virtual
	const Type *ValueType() const;
	
private:
	// Member: mMember
    MemberType ObjectType::*mMember;
};

} }

#endif
