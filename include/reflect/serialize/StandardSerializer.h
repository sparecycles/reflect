#ifndef REFLECT_SERIALIZE_STANDARDSERIALIZER_H_
#define REFLECT_SERIALIZE_STANDARDSERIALIZER_H_

#include <reflect/Serializer.h>
#include <reflect/string/String.h>
#include <reflect/config/config.h>
#include <map>

namespace reflect {
class OutputStream;
}

namespace reflect { namespace serialize {

// Class: StandardSerializer
//     A basic ascii text serializer.
//
// See Also:
//     - <Serializer>
//     - <StandardDeserializer>
class ReflectExport(reflect) StandardSerializer : public Serializer
{
public:
    StandardSerializer(OutputStream &stream);
    
protected:
	// Function: Begin
	//   Writes a serialization start tag. 
	//
	// Tags are written in the following formats:
	//   OBJECT    - "(<<type>> ... )"
	//   PROPERTY  - "$<<name>>= ... ;"
	//   ATTRIBUTE - "[<<name>>= ... ]"
	//   ITEM      - "{ ... }"
    bool Begin(const SerializationTag &); /*virtual*/
    
    // Function: End
    //   Writes a serialization end tag, see <Begin>.
    bool End(const SerializationTag &); /*virtual*/

	// Function: Serialize(bool)
	//   Writes "true"/"false".
    bool Serialize(bool); /*virtual*/
    
    // Function: Serialize(long)
	//   Writes "%ld", e.g., 3 -> "3"
    bool Serialize(long); /*virtual*/

    // Function: Serialize(unsigned long)
	//   Writes "0x%lX", e.g., 32u -> "0x20"
    bool Serialize(unsigned long); /*virtual*/
    
    // Function: Serialize(double)
    //   Writes "%#g".  e.g., 3 -> "3.0" 
    bool Serialize(double); /*virtual*/
    
    // Function: Serialize(const Dynamic *obj)
    //   Writes "#<<obj->GetClass()->GetCategory()->Name()>>"
    // and then defers to 
    // obj->GetClass()->GetCategory()->SerializeDynamic(obj, Reflector(*this)).
    //
    // For instance, a Class * would serializes as 
    // > #Class "<<name of class>>"
    // and a persistent object would serialize as
    // > #Persistent 
    // >   (<<type of object>> 
    // >      $x = 34;
    // >      $s = "hello";
    // >   )
    bool Serialize(const Dynamic *object); /*virtual*/
    
    // Function: Reference(const Dynamic *object)
    //   Writes "@<<count>>" where *count* is the number of 
    // references made so far.
    bool Reference(const Dynamic *object); /*virtual*/
    
    // Function: SerializeText
    //    Writes *nbytes* of *text* as a string with
    //    quotes around it and some standard escapes.
    bool SerializeText(const char *text, unsigned nbytes); /*virtual*/

    // Function: SerializeData
    //    Serializes *nbytes* of *data* as hex pairs A3 45 23 8F ...
    bool SerializeData(const void *data, unsigned nbytes); /*virtual*/
    
    // Function: SerializeEnum
    //    Writes the enum value as a bare string (no quotes).
    // If the enum value is not mapped in clazz it serializes as an unsigned number.
    bool SerializeEnum(int value, const EnumType *clazz); /*virtual*/
    
    // Function: SerializeProperty
    //    Writes the property by deferring to its <Property::Serialize> method.
    // TODO: Figure out how/if this can prevent slicing when using CompositeSerializers.
   	bool SerializeProperty(const void *object, const Property *prop); /*virtual*/

protected:
    bool Write(const char *string, ...);
    void Indent();
    void Undent();
    void Break();
	void Space();
	void NoSpace();
	
private:
    int mIndent;
    bool mBreak;
	bool mSpace;
	int mNextIndex;
	std::map<const Dynamic *, int> mReferenced;
	OutputStream &mStream;
	string::String mBuffer;
};

} }

#endif
