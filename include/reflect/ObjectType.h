#ifndef REFLECT_OBJECTTYPE_H_
#define REFLECT_OBJECTTYPE_H_

#include <reflect/Type.h>
#include <reflect/Variant.h>

namespace reflect { namespace function { class Function; } }

namespace reflect {

class Reflector;
class Variant;

namespace string { class ConstString; }

// Class: ObjectType
class ReflectExport(reflect) ObjectType : public Type
{
	DECLARE_REFLECTION(Type)
public:
	// Typedef: Annotations
	typedef std::map<string::SharedString, Variant> Annotations;

	// Typedef: AnnotationMap
	// A map of annotations for the class by name.
	// The map only contains annotations registered on this class
	// directly, and not inherited properties.
	//
	// Annotations for properties are stored in the map by name.
	// Annotations for the class are stored in the map with the empty
	// string as the key.
	typedef std::map<string::SharedString, Annotations *> AnnotationMap;

	template<typename T>
	class DescriptionHelper;

	class FunctionIterator;

	typedef std::map<string::SharedString, const function::Function *> FunctionMap;

    ObjectType(void (*init_cb)() = 0);

	// Function: RegisterFunction
	// Adds the function to the function map.
	void RegisterFunction(const char *name, const function::Function *);

	// Function: Functions
	// Const accessor for the class <FunctionMap>.
    const FunctionMap *GetFunctionMap() const;
    
	// Function: Functions
	// Accessor for the class <FunctionMap>.
    FunctionMap &Functions();
	
	// Function: FindFunction
	// Finds a function in this class' <FunctionMap> or a parent class.
    const function::Function *FindFunction(string::Fragment name) const;
    
	// Function: FindFunction
	// Finds a function in this class' <FunctionMap> or a parent class.
	// The <SharedString> name should be in the global string stack.
    const function::Function*FindFunction(string::SharedString name) const;

	template<typename T>
	void AnnotateValue(const string::SharedString &key, const string::SharedString &subkey, const T &value)
	{
		Annotate(key, subkey, Variant::FromValue(value));
	}

	template<typename T>
	void AnnotateStaticValue(const string::SharedString &key, const string::SharedString &subkey, const T &value)
	{
		Annotate(key, subkey, Variant::FromStaticValue(value));
	}

	void Annotate(string::SharedString key, string::SharedString subkey, Variant value);
	Annotations *GetAnnotations(string::FoundSharedString key) const;
	Variant GetAnnotation(string::Fragment key, string::Fragment subkey) const;
	bool HasAnnotation(string::Fragment key, string::Fragment subkey) const;

protected:
    //virtual
    void Initialize();

private:
	// Member: mFunctions
	// The map of functions registered on this type.
	FunctionMap *mFunctions;
    AnnotationMap *mAnnotationMap;
};

// Class: ObjectType::FunctionIterator
class ReflectExport(reflect) ObjectType::FunctionIterator
{
public:
	FunctionIterator(const ObjectType *);
	const ObjectType *type() const { return mCurrentClass; }
	const ObjectType::FunctionMap::value_type &operator *() const;
	const ObjectType::FunctionMap::value_type *operator ->() const;
	operator bool() const;
	bool next();

private:
	bool FindNext(const ObjectType *);

	const ObjectType *mCurrentClass;
	ObjectType::FunctionMap::const_iterator mCurrentItem;
};

}

#endif
