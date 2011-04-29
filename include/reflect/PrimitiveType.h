#ifndef REFLECT_PRIMITIVETYPE_H_
#define REFLECT_PRIMITIVETYPE_H_

#include <reflect/ObjectType.h>

namespace reflect {

class Property;

class ReflectExport(reflect) PrimitiveType : public ObjectType
{
    DECLARE_REFLECTION(Type)
public:
    PrimitiveType(void (*init_cb)() = 0);

	/*virtual*/ void Serialize(const void *, void *, Reflector &) const;
	
	template<typename T>
	class DescriptionHelper;

	void SetSerializer(void (*serializer)(const void *, void *, Reflector &));
	
private:
	void (*mSerializer)(const void *, void *, Reflector &);
};

#define DECLARE_PRIMITIVE_REFLECTION(MODULE, TYPE) \
	DECLARE_STATIC_REFLECTION(MODULE, TYPE, reflect::PrimitiveType)

}

#endif
