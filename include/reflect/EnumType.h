#ifndef REFLECT_ENUMTYPE_H_
#define REFLECT_ENUMTYPE_H_

#include <reflect/Type.h>
#include <reflect/string/SharedString.h>
#include <map>

namespace reflect {

class Property;

// Class: EnumType
class ReflectExport(reflect) EnumType : public Type
{
    DECLARE_REFLECTION(Type)
public:
	// typedef: ToValueMap
	typedef std::map<string::SharedString, int> ToValueMap;

	// typedef: ToNameMap
	typedef std::map<int, string::SharedString> ToNameMap;
	
	// Constructor: EnumType
    EnumType(void (*init_cb)() = 0);

	// Function: ReadEnum
	int ReadEnum(const void *);

	// Function: WriteEnum
	void WriteEnum(void *, int);

	template<typename Type>
	class DescriptionHelper;
	
	// Function: AddValue
	void AddValue(int value, const char *name);
	
	// Function: ValueMap
	const ToValueMap &ValueMap() const { return mValues; } 

	// Function: NameMap
	const ToNameMap &NameMap() const { return mNames; } 
	
	// Function: SetAccessors
	void SetAccessors(int (*)(const void *), void (*)(void *, int));

	void Serialize(const void *, void *, Reflector &) const; // virtual
private:
	ToValueMap mValues;
	ToNameMap mNames;
	int (*mReadValue)(const void *data);
	void (*mWriteValue)(void *data, int i);
};

}

#endif
