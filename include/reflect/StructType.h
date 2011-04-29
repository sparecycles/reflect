#ifndef REFLECT_STRUCTTYPE_H_
#define REFLECT_STRUCTTYPE_H_

#include <reflect/ObjectType.h>
#include <reflect/PrimitiveType.h>
#include <vector>

namespace reflect {

class Property;

// Class: StructType
// Reflects a struct as a property-tuple.
//
// See Also:
//   - <ObjectType>
//   - <Type>
//   - <DECLARE_STATIC_REFLECTION>
class ReflectExport(reflect) StructType : public ObjectType
{
    DECLARE_REFLECTION(ObjectType)
public:
    StructType(void (*init_cb)() = 0);

	/*virtual*/ void Serialize(const void *, void *, Reflector &) const;
	
	template<typename Type>
	class DescriptionHelper;

	// Function: AddMember
	void AddMember(Property *);

	int NumMembers() const;
	const Property *GetMember(int index) const;
private:
	// Member: mMembers
	std::vector<Property *> mMembers;
};

}

#endif
