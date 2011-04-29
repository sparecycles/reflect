#ifndef REFLECT_SERIALIZE_SHALLOWDESERIALIZER_H_
#define REFLECT_SERIALIZE_SHALLOWDESERIALIZER_H_

#include <reflect/serialize/CompositeDeserializer.h>

namespace reflect { namespace serialize {

// Class: ShallowDeserializer
// 
// A composite serializer that deserializes pointers as raw data
// rather than traversing them.
//
// See Also:
//    - <ShallowSerializer>
//    - <CompositeDeserializer>
class ReflectExport(reflect) ShallowDeserializer : public CompositeDeserializer
{
public:
	ShallowDeserializer(Deserializer &base)
		: CompositeDeserializer(base)
	{
	}
	
protected:
	/*virtual*/ bool Deserialize(Dynamic *&dynamic)
	{
		bool result = false;
		
		SerializationTag tag;
			
		if(Begin(tag, SerializationTag::ItemTag))
		{
			void *ptrval;
	
			if(CompositeDeserializer::DeserializeData(&ptrval, sizeof(ptrval)))
			{
				result = true;
				dynamic = reinterpret_cast<Dynamic *>(ptrval);
			}
			
			result = End(tag) && result;
		}
		
		return result;
	}
};

} }

#endif
