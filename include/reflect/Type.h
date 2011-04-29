#ifndef REFLECT_TYPE_H_
#define REFLECT_TYPE_H_

#include <reflect/Dynamic.h>
#include <reflect/Reflection.h>
#include <reflect/Signature.h>
#include <map>

namespace reflect {

class Reflector;

namespace string { class SharedString; class FoundSharedString; }

// Class: Type
class ReflectExport(reflect) Type : public Dynamic
{
	DECLARE_REFLECTION(Dynamic)
public:
	template<typename Type>
    class DescriptionHelper;

	typedef std::map<string::SharedString, Type *> TypeMapType;
	typedef void (*Conversion)(void *, const void *);
	typedef std::map<const Type *, Conversion> ConversionMap;

	// Constructor: Type
	// Constructs a class.
	//
	// Parameters:
	//    init_cb - the function to run to initialize this class.
	//
	// See Also:
	//   - <Type::LoadTypes>
    Type(void (*init_cb)() = 0);

	// Destructor: ~Type
	// Destructs this class and unlinks it from the class hierarchy.
    ~Type();

	// Function: Parent
	// Gets the parent (base class) of this Type.
    Type *Parent() const;

	// Function: SetParent
	// Sets the parent (base class) of this Type.
	void SetParent(Type *parent);

	// Function: Derives
	//
	// Checks if this class represents a subclass of another.
	//
	// Parameters:
	//    other - another class.
	//
	// Returns:
	//    true if this class is a subclass of other
    bool DerivesType(const Type *other) const;

	// Function: Derives (template)
	// Checks if this class represents a subclass of another.
	// 
	// Template version of <Derives>.
	template<typename T>
	bool Derives() const { return DerivesType(TypeOf<T>()); }

	// Function: Child
	// The first subclass of this class or NULL if there are no subclasses.
	//
	// The <Sibling> method can be used to iterate over the other subclasses.
    Type *Child() const;

	// Function: Sibling
	// The next sibling of this class, or itself if there are no siblings.
	// Siblings are linked in a ring, so be careful when iterating over them.
	// The folling idiom can be used to iterator over the subclasses of *clazz*:
	//
	// > if(Type *child = clazz->Child()) do
	// > {
	// >    // PROCESS child
	// > }
	// > while((child = child->Sibling()) != clazz->Child());
    Type *Sibling() const;

    // Function: LoadTypes
    //    Initializes all types which have been static initialized.
    //
    // Returns:
    //    The first class in the current class load chain.
    // 
    // See Also:
    //    - <Type::UnloadTypes>
    //    - <Type::FirstTypeToLoad>
	//    - <Type::NextTypeToLoad>
    static Type *LoadTypes();

    // Function: UnloadTypes
    // Reverses the effect of <Type::LoadTypes>.
    //
    // Note: this hasn't been tested much,
    // it is probable that doing this is unsafe.
    //
    // TODO:
    // - Remove conversions from all classes to unloaded types.
    static void UnloadTypes(Type *);

	// Function: AnyRootType
	// Retrieves a root (any root) of the type hierarchy.
	// roots are connected by <NextSibling>
	static Type *AnyRootType();

	// Function: FindType
	// Retrieves a type by name.
	// 
	// Paramters:
	//    typename - the name of a class.
	//
	// Returns:
	//    The type or NULL if no class is registered with that name.
	static Type *FindType(string::FoundSharedString);
    
	// Function: FindType(char*)
	// Retrieves a type by name.
	//
	// Paramters:
	//    typename - the name of a type.
	//
	// Result:
	//    The type or NULL if no type is registered with that name.
	static Type *FindType(const char *);

	// Function: SetName
	// Sets the name of the type and calls RegisterName to it in a global map.
    void SetName(const char *name);
	
	// Function: Name
	// The registered name of this type.
    const char *Name() const;

	// Function: NextTypeToLoad
	// The next type in a NULL terminated chain of
	// types to load in <Type::LoadTypes>.
	// The type chain is preserved after <Type::LoadTypes>.
	Type *NextTypeToLoad() const;

	// Function: FirstTypeToLoad
	// The first class in a NULL terminated chain of
	// classes to load in the next <Type::LoadTypes>.
	// After <Type::LoadTypes> is called this will be NULL.
	static Type *FirstTypeToLoad();

	// Function: Serialize
	// Serialize defines how objects represented by this type are serialized.
	// The void * arguments must be converted from typed pointers the by the static
	// template function <Type::OpaqueCast>, subtypes may hide the default implementation.
	//
	// See Also:
	// - <opaque_cast> global helpers
	virtual void Serialize(const void *in, void *out, Reflector &reflector) const;
	
	// Function: OpaqueCast
	// Casts a type represented by this Type to a void *, used by
	// <Serialize>. (there are const and non-const versions of this function)
	//
	// See Also:
	// - <TranslucentCast>
	// - <opaque_cast> global helpers
	// - <translucent_cast> global helpers
	// - <Variant> variant based on this mechanism
	template<typename Type>
	static void *OpaqueCast(Type *ptr) { return static_cast<void *>(ptr); }

	template<typename Type>
	static const void *OpaqueCast(const Type *ptr) { return static_cast<const void *>(ptr); }

	// Function: TranslucentCast
	// Casts a void * from <OpaqueCast> back to a Type.
	// (there are const and non-const versions of this function)
	//
	// See Also:
	// - <OpaqueCast>
	// - <opaque_cast> global helpers
	// - <translucent_cast> global helpers
	// - <Variant> variant based on this mechanism
	template<typename T>
	static T *TranslucentCast(void *ptr) { return static_cast<T *>(ptr); }

	template<typename T>
	static const T *TranslucentCast(const void *ptr) { return static_cast<const T *>(ptr); }

    // Function: SetBasicOperations
    // Sets the factory function for this class.
    void SetBasicOperations(unsigned size, unsigned alignment, void *(*construct)(void *), void *(*destruct)(void *));

    // Function: Size
   	//
    // Returns:
    //    The size of the object represented by this class.
    unsigned Size() const;

	// Function: Alignment
	//
	// Returns:
    //    The minimum required alignment of the object represented by this class.
    unsigned Alignment() const;

    // Function: Construct
	// Constructs an instance of this type.  The pointer provided should be at least <Size> bytes and
    // aligned to <Alignment>.
    //
    // Parameters:
    //    data - an aligned pointer to at least <Alignment> bytes of memory.
	//
    // Returns:
    //    An <OpaqueCast> of the object (if it's not Abstract)
    void *Construct(void *) const;
    
    // Function: Destruct
	// Destructs an instance of this type.
	// The pointer should be returned from <Construct>, or an <opaque_cast> 
    // of a pointer to data of this type.
    //
    // Parameters:
    //    data - the result of a previous <Construct>, or the <OpaqueCast>
    //           of an object represented by this class.
    //
    // Returns:
    //    The void pointer 
    //    that would be passed to <Construct> originally.
    //    This would be an allocation pointer, not an opaque pointer.
    void *Destruct(void *) const;
    
    // Function: Abstract
	// Checks if the class is Abstract.
	//
	// See Also:
	// - <Type::DescriptionHelper> Concrete feature to make a class not Abstract.
	//
    // Returns:
    //    true - if the class has no size.
	bool Abstract() const;

	// Function: RegisterConversion
	// Registers a cast from type *from* to this type. 
    //
	// See Also:
	// - <Type::DescriptionHelper>
	void RegisterConversion(const Type *from, void (*)(void *, const void *));
	
	// Function: CanConvertFrom
	// Checks if a cast from the specified type exists.
	virtual bool CanConvertFrom(const Type *from) const;

	// Function: CanConvertFrom (template)
	// Checks if a cast from the specified type exists.
	template<typename T>
	bool CanConvertFrom() const { return CanConvertFrom(TypeOf<T>()); }

	// Function: ConvertValue
	// Uses conversions registered with <RegisterConversion>.
	//
	// Returns:
	//   true - if a conversion from *from* is found.
	//
	// See Also:
	// - <Type::DescriptionHelper> converson features.
	virtual bool ConvertValue(void *opaque, const void *source, const Type *from) const;

	static const TypeMapType &GlobalTypeMap();

protected:
    virtual void Initialize();

private:
    void CompileHierarchy();
	void ReleaseCompiledHierarchy(Type **hierarchy);
    void Init();

    void LinkHierarchy();
    void UnlinkHierarchy();
    void InitHierarchy();

	virtual void RegisterName();

    Type *mParent;
    Type *mFirstChild;
    Type *mSibling;

    void (*mTypeInitializerCB)();
    const char *mName;

    int mDepth;
    Type **mHierarchy;

    Type *mNextTypeToLoad;

	unsigned mSize;
	unsigned mAlignment;
    void *(*mConstructor)(void *);
    void *(*mDestructor)(void *);
 
    ConversionMap mConversions;
};

}

#endif
