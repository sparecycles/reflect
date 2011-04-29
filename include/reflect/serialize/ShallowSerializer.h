#ifndef REFLECT_SERIALIZE_SHALLOWSERIALIZER_H_
#define REFLECT_SERIALIZE_SHALLOWSERIALIZER_H_

#include <reflect/serialize/CompositeSerializer.h>

namespace reflect { namespace serialize {

// Class: ShallowSerializer
// 
// A composite serializer that serializes pointers as raw data
// rather than traversing them.
// 
// See Also:
//    - <ShallowDeserializer>
//    - <CompositeSerializer>
class ReflectExport(reflect) ShallowSerializer : public CompositeSerializer
{
public:
	ShallowSerializer(Serializer &base)
		: CompositeSerializer(base)
	{
	}
	
protected:
	/*virtual*/ bool Serialize(const Dynamic *castable)
	{
		bool result = false;
		
		SerializationTag tag(SerializationTag::ItemTag);
		
		if(Begin(tag))
		{
			const void *ptrvalue = reinterpret_cast<const void *>(castable);
			result = CompositeSerializer::SerializeData(&ptrvalue, sizeof(void *));
			result = End(tag) && result;
		}
		
		return result;
	}
};

} }

#endif
