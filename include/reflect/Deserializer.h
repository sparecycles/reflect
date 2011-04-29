#ifndef REFLECT_DESERIALIZER_H_
#define REFLECT_DESERIALIZER_H_

#include <reflect/config/config.h>
#include <reflect/SerializationTag.h>

namespace reflect {

class Dynamic;
class Property;
class Category;
class EnumType;


// Class: Deserializer
//
// Defines the interface for deserialization.
//
// Serialization and Deserialization convert between 
// hierarchical, structured and tagged data and
// a linear stream of data.
//
// See Also:
//   - <Serializer>
class ReflectExport(reflect) Deserializer
{
public:
	// Function: Begin
	// Deserializes an opening tag in the stream.
	// If it succeeds, <End> must be called to end the
	// tag with the same <SerializationTag> object.
    virtual bool Begin(SerializationTag &,
		SerializationTag::TagType = SerializationTag::UnknownTag) = 0;

	// Function: End
	// Deserializes an ending tag in the stream.
	// Properly tagged streams can handle this method prematurely,
	// skipping over any excess data until the end is reached. 
    virtual bool End(SerializationTag &) = 0;
    
	// Function: Deserialize(bool &)
	// Reads a bool from the stream.
    virtual bool Deserialize(bool &) = 0;

	// Function: Deserialize(long &)
	// Reads an integral number.
    virtual bool Deserialize(long &) = 0;

	// Function: Deserialize(long &)
	// Reads an unsigned integral number.
    virtual bool Deserialize(unsigned long &) = 0;
 
	// Function: Deserialize(double &)
	// Reads a floating point number.
    virtual bool Deserialize(double &) = 0;
 
	// Function: Deserialize(Dynamic *&)
	// Reads a Dynamic *, using a category.
    virtual bool Deserialize(Dynamic *&object) = 0;
 
	// Function: Reference
	// References a Dynamic *.
	// The pattern of referencing must match
	// the pattern durring serialization.
    //
    // The reason that <Reference> is not implemented in <Serialize> is
    // that when deserializing a pointer which contains (or references) 
    // a pointer to itself, it must have been Referenced to 
    // deserialize correctly.
    //
    // See:
    //    - <PersistentClass.DeserializePointer>
    virtual bool Reference(Dynamic *object) = 0;
 
	// Function: DeserializeTextChunk
	// Deserializes text in chunks. 
	//
	// Parameters:
	//    text - the output buffer, if text is 0 then up to max_bytes text will be read and discarded.
	//    max_bytes - *in/out*
	//       in: the maximum number of bytes to write to text,
	//       out: the number of bytes actually written.
	//    complete - set to true when the end of the text is reached.
	// 
	// Once a text chunk read is started, clients must keep calling deserialize text chunk
	// until complete is true.  
    virtual bool DeserializeTextChunk(char *text, unsigned &max_bytes, bool &complete) = 0;
 
	// Function: DeserializeData
	// Deserializes a block of data.
	// The size of the data must be the same as what was written.  There is no requirement for the
	// deserializer to validate that it is the correct size.  It is format specific
	// whether serialized binary data concatenates/splits gracefully.
    virtual bool DeserializeData(void *data, unsigned nbytes) = 0;

	// Function: DeserializeEnum
	virtual bool DeserializeEnum(int &value, const EnumType *clazz) = 0;

	// Function: DeserializeProperty
	// Writes into the object using the <Properties'> method.
	virtual bool DeserializeProperty(void *object, const Property *prop) = 0;

protected:
	// Destructor: ~Deserializer
	virtual ~Deserializer();
};

}

#endif
