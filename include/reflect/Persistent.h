#ifndef REFLECT_PERSISTENT_H_
#define REFLECT_PERSISTENT_H_

#include <reflect/Reflection.h>
#include <reflect/PersistentClass.h>
#include <reflect/config/config.h>

namespace reflect {

// Class: Persistent
//   The base class of all objects that can be saved and loaded.
// Persistent objects should be no larger than a normal castable.
class ReflectExport(reflect) Persistent : public Dynamic
{
	DECLARE_REFLECTION_EX(Dynamic, PersistentClass)
public:
	// Function: Property
	//   Returns a <PropertyPath> which can be used to access the named property.
	PropertyPath Property(string::Fragment property_path);
};

}

#endif
