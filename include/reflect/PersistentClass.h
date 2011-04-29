#ifndef REFLECT_PERSISTENTCLASS_H_
#define REFLECT_PERSISTENTCLASS_H_

#include <reflect/Class.h>
#include <reflect/Reflection.h>
#include <reflect/string/Fragment.h>

#include <map>

namespace reflect {

class Property;
class Persistent;
class PersistentCategory;
class Serializer;
class Deserializer;
class PropertyPath;

namespace string {
class SharedString;
class String;
class Fragment;
class MutableString;
}

// Class: PersistentClass
//
// This class enables serialization and stores property maps.
class ReflectExport(reflect) PersistentClass : public Class
{
    DECLARE_REFLECTION(Class)
public:
	// Typedef: PropertyMap
	// A map of properties for the class by name.
	// The map only contains properties registered on this class
	// directly, and not inherited properties.
	typedef std::map<string::SharedString, const Property *> PropertyMap;

    PersistentClass(void (*init_cb)() = 0);

    // Function: Construct
    //
    // Parameters:
    //    data - an aligned pointer to at least <Alignment()> bytes of memory.
	//
    // Returns:
    //    A new instance of the class (if it's not Abstract)
    Persistent *Construct(void *) const;
    
    // Function: Destruct
    //
    // Parameters:
	//    A pointer to a Persistent constructed with this Class.
    void Destruct(Persistent *) const;

    // Function: Create
    // Returns:
    //    A new instance of the class (if it's not Abstract)
    Persistent *Create() const;

	// Function: AddProperty
	// Adds the property to the property map.
	// The property name should not be the same as any
	// property named in a subclass.
    void RegisterProperty(const char *name, const Property *);

	// Function: Properties
	// Const accessor for the class <PropertyMap>.
    const PropertyMap *GetPropertyMap() const;
    
	// Function: Properties
	// Accessor for the class <PropertyMap>.
    PropertyMap &Properties();

	// Function: FindProperty
	// Finds a property in this class' <PropertyMap> or a parent class.
    const Property *FindProperty(string::Fragment name) const;
    
	// Function: FindProperty
	// Finds a property in this class' <PropertyMap> or a parent class.
	// The <SharedString> name should be in the global string stack.
    const Property *FindProperty(string::SharedString name) const;

	// Function: ResolvePropertyPath
	// 
	// See Also:
	//  - <PropertyPath>
    static bool ResolvePropertyPath(PropertyPath &result, const Persistent *, string::Fragment path);

	// Function: ResolvePropertyPath
	// 
	// See Also:
	//  - <PropertyPath>
 	static bool ResolvePropertyPath(PropertyPath &result, Persistent *, string::Fragment path);

	// Function: ResolvePropertyPath
	// 
	// See Also:
	//  - <PropertyPath>
 	static bool ResolvePropertyPath(PropertyPath &result, const Persistent *, Persistent *, string::Fragment path);
	
	// Function: WriteProperty
	// Writes a property using a <StandardDeserializer> to parse the data.
	bool WriteProperty(Persistent *, string::Fragment path, string::Fragment value) const;

	// Function: WriteStringProperty
	bool WriteStringProperty(Persistent *, string::Fragment path, string::Fragment value) const;

	// Function: ReadProperty
	// Reads a property using a <StandardSerializer> to format the data.
	bool ReadProperty(const Persistent *, string::Fragment path, string::String &value) const;

	// Function: ReadStringProperty
	bool ReadStringProperty(const Persistent *, string::Fragment path, string::String &value) const;

	// Function: WriteRefProperty
	// Writes a reference property (a property representing a <Dynamic> *).
	//
	// See Also:
	//   - <RefProperty> 
	bool WriteRefProperty(Persistent *, string::Fragment path, Dynamic *value) const;

	// Function: ReadRefProperty
	// Reads a reference property (a property representing a <Dynamic> *).
	// 
	// See Also:
	//   - <RefProperty>
	bool ReadRefProperty(const Persistent *, string::Fragment path, Dynamic *&value) const;

	// Function: GetPropertyType
	const Type *GetPropertyType(const Persistent *, string::Fragment path) const;

	// Function: SerializeProperties
    void SerializeProperties(const Persistent *, Reflector &) const;

	// Function: DeserializeProperties
    void DeserializeProperties(Persistent *, Reflector &) const;

	// Function: Serialize
	void Serialize(const void *in, void *out, Reflector &reflector) const; // virtual

	//virtual
	void SerializePointer(const Dynamic *in, Reflector &reflector) const;
	//virtual
	void DeserializePointer(Dynamic *&out, Reflector &reflector) const;

	template<typename Type>
	class DescriptionHelper;

	class PropertyIterator;
private:
	/*virtual*/ void Initialize();

    PropertyMap *mProperties;
};

// Class: PersistentClass::PropertyIterator
class ReflectExport(reflect) PersistentClass::PropertyIterator
{
public:
	PropertyIterator(const PersistentClass *clazz);
	const PersistentClass *type() const { return mCurrentClass; }
	const PersistentClass::PropertyMap::value_type &operator *() const;
	const PersistentClass::PropertyMap::value_type *operator ->() const;
	operator bool() const;
	bool next();
	
private:
	bool FindNext(const PersistentClass *);

	const PersistentClass *mCurrentClass;
	PersistentClass::PropertyMap::const_iterator mCurrentItem;
};


}

#endif
