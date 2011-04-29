#ifndef REFLECT_PROPERTYPATH_H_
#define REFLECT_PROPERTYPATH_H_

#include <reflect/string/String.h>
#include <reflect/MapProperty.h>
#include <reflect/utility/InOutReflector.h>
#include <reflect/Property.h>
#include <reflect/PersistentClass.h>
#include <reflect/PrimitiveTypes.h>

namespace reflect {

class Persistent;

// Class: PropertyPath
//
// A PropertyPath is a generic proxy for a data in an object.
// It can represent data, a <Dynamic> pointer, an array, a map,
// an array of pointers, etc...
//
// PropertyPaths are normally constructed using <PersistentClass.ResolvePropertyPath>.
//
// Reading and writing data using property paths can be done using <Reflectors>
// or by strings (which are serialized or deserialized with <StandardSerializers>
// and <StandardDeserializers>). 
//
// PropertyPaths
class ReflectExport(reflect) PropertyPath
{
public:
	// Enumeration: NormalizedType
	//
	// The type of data that this path represents. 
	//
	//   Data - represents a basic property (see: <DataProperty>).
	//   Array - represents a property of many items (see: <ArrayProperty>).
	//   Map - represents a property representing a map (see: <MapProperty>).
	//   Opaque - represents a custom property which can serialize
	//            but provides no apriori hints about its contents. (see: <Property>).
	//   Object - represents an object, not a property. // TODO
	//   Invalid - bad path.
	enum NormalizedType { 
		Data,
		Array,
		Map,
		Opaque,
		Object,
		Invalid = -1 };

	enum InternalType { 
		PropertyInternalType,
		ArrayItemPropertyInternalType, 
		MapItemPropertyInternalType, 
		ObjectInternalType,
		InvalidInternalType = -1 };

	// Constructor: PropertyPath
	// default constructor.
	PropertyPath();
	
	// Constructor: PropertyPath
	PropertyPath(
		const Persistent *,
		Persistent *);

	// Constructor: PropertyPath
	PropertyPath(
		Persistent *);

	// Constructor: PropertyPath
	PropertyPath(
		const Persistent *);

	bool Advance(PropertyPath &to, string::Fragment path) const;

	PropertyPath Advanced(string::Fragment path) const;
	
	bool Advance(string::Fragment path);

	// Function: Write(Reflector&) const
	void Write(Reflector &reflector) const;

	// Function: Read(Reflector&) const
	void Read(Reflector &reflector) const;

	// Function: Write(string::Fragment) const
	bool Write(string::Fragment) const;

	// Function: Writef
	bool Writef(const char *format, ...) const;
	
	// Function: Read(mutable_string) const
	bool Read(string::MutableString) const;
	
	// Function: Read(string)
	bool Read(string::String&) const;
		
	// Function: Read
	string::String Read() const;

	// Function: IsRef
	// check if this path result is a Ref, ArrayRef, or MapRef property
	bool IsRef() const;

	// Function: IsOpaque
	// check if this path result is an Opaque property (just a <Property::Serialize>
	// function with no associated structure or Type.
	bool IsOpaque() const;
	
	// Function: IsData
	// check if this path result is a Data property (not ref, array, map...).
	bool IsData() const;

	// Function: IsDatum
	// check if this path result is not an array or map, (may be Data or Ref).
	bool IsDatum() const;

	// Function: IsMapData
	// check if this path result is a map data property.
	// Map data may not exist (until it is added to the map).
	bool IsMapData() const;

	// Function: IsArrayData
	// check if this path result is an array data property.
	// Array data is may not exist (until the underlying vector is resized).
	bool IsArrayData() const;

	// Function: DataExists
	// check if this path result is an existing map
	// data property, requires <IsMapData()> or <IsArrayData()> to be true.
	bool DataExists() const;
	
	// Function: IsArray
	// check if this path result is an array property.
	bool IsArray() const;
	
	// Function: IsMap
	// check if this path result is a map property.
	bool IsMap() const;

	// Function: ArraySize
	// get the array size, requires <IsArray()> to be true.
	unsigned ArraySize() const;

	// Function: ArrayResize
	// sets the array size, requires <IsArray()> to be true,
	// and the property should be <ArrayProperty.Resizable>.
	bool ArrayResize(unsigned newsize) const;

	// Function: ArrayItem
	// gets an array item as a property path,
	// requires <IsArray()> to be true.
	PropertyPath ArrayItem(unsigned index) const;

	// Function: MapItem
	// gets a map item as a property path,
	// requires <IsMap()> to be true.
	PropertyPath MapItem(string::Fragment key) const;

	// Function: GetType
	// retrieves the type of this path as an enum.
	NormalizedType GetType() const;
	
	// Function: GetDataType
	// the data class of the property.
	// requires <IsDatum()> to be true.
	const Type *GetDataType() const;

	// Function: GetRefClass
	// the type of reference of the property.
	const Class *GetRefClass() const;

	// Function: GetArrayItemType
	// the type of items in an array, requires <IsArray()> to be true.
	const Type *GetArrayItemType() const;
	
	// Function: GetArrayItemRefClass
	// the type of item pointers in an array of Dynamic *,
	// requires <IsArray()> and <IsRef()> to be true.
	const Class *GetArrayItemRefClass() const;

	// Function: GetMapKeyType
	// The type of item representing map keys.
	// Requires <IsMap()> to be true.
	const Type *GetMapKeyType() const;	

	// Function: GetMapValueType
	// The type of item representing map values.
	// Requires <IsMap()> to be true.
	const Type *GetMapValueType() const;

	// Function: GetMapRefValueClass
	// The type of item representing map values.
	// Requires <IsMap()> and <IsRef()> to be true.
	const Class *GetMapRefValueClass() const;
	
	// Function: Annotations
	// Returns the annotations for the current property.
	const PersistentClass::Annotations *Annotations() const;

	// Function: Annotation
	// Returns the annotation by name for the current property.
	Variant Annotation(string::Fragment name) const;

	// Function: AnnotationAs
	// Returns the annotation by name for the current property.
	template<typename T>
	T AnnotationAs(string::Fragment name) const
	{
		return Annotation(name).AsValue<T>();
	}

	// Function: ReadValue
	// Easy way to read data templated on data type.	
	template<typename T>
	bool ReadValue(T &data) const
	{
		Variant variant = Variant::FromRef(data);
		if(ReadData(variant))
		{
			return true;
		}
		else
		{
			utility::InOutReflector<reflect::serialize::StandardSerializer, reflect::serialize::StandardDeserializer> io;
			Reflector reflector(io.GetSerializer());
			Read(reflector);
			io >> data;
			return reflector.Ok();
		}
	}

	bool ReadData(Variant &value) const;

	template<typename T>
	T ReadValue() const
	{
		T value;
		ReadValue(value);
		return value;
	}
	
	// Function: WriteData
	// Easy way to write data templated on data type.
	template<typename DataType>
	bool WriteValue(const DataType &data) const
	{
		return WriteData(Variant::FromConstRef(data));
	}

	bool WriteData(const Variant &value) const;
	
private:
	InternalType GetInternalType() const;
	bool AdvanceInternal(string::Fragment path);

	NormalizedType mType;
	InternalType mInternalType;
	const Persistent *mConstObject;
	Persistent *mObject;
	const reflect::Property *mProperty;
	Variant mKey;
	unsigned mArrayIndex;
	PersistentClass::Annotations *mAnnotations;
};

}

#endif
