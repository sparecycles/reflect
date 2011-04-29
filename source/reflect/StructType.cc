#include <reflect/StructType.hpp>
#include <reflect/Class.hpp>
#include <reflect/Property.h>

namespace reflect {

StructType::StructType(void (*init_cb)())
	: ObjectType(init_cb)
{
}

void StructType::AddMember(Property *prop)
{
	mMembers.push_back(prop);
}

void StructType::Serialize(const void *in, void *out, Reflector &reflector) const
{	
	for(std::vector<Property *>::const_iterator it = mMembers.begin(); it != mMembers.end(); ++it)
	{
		(*it)->Serialize(in, out, reflector);
	}
}

int StructType::NumMembers() const 
{
	return mMembers.size();
}

const Property *StructType::GetMember(int index) const
{
	if(index >= 0 && index < int(mMembers.size()))
		return mMembers[index];
	return 0;
}

}

DEFINE_REFLECTION(reflect::StructType, "reflect::StructType")
{
}
