#include <reflect/PrimitiveType.hpp>
#include <reflect/Class.hpp>

namespace reflect {

PrimitiveType::PrimitiveType(void (*init_cb)())
	: ObjectType(init_cb)
	, mSerializer(0)
{
}

void PrimitiveType::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(mSerializer) (*mSerializer)(in, out, reflector);
	else reflector.Fail();
}
	
void PrimitiveType::SetSerializer(void (*serializer)(const void *, void *, Reflector &))
{
	mSerializer = serializer;
}

}

DEFINE_REFLECTION(reflect::PrimitiveType, "reflect::PrimitiveType")
{
}
