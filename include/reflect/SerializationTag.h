#ifndef REFLECT_SERIALIZATIONTAG_H_
#define REFLECT_SERIALIZATIONTAG_H_

#include <reflect/string/ArrayString.h>

#include <reflect/config/config.h>

namespace reflect {

// Class: SerializationTag
//
// This class represents the begin/end tags in a
// <Serializer>/<Deserializer> stream.
//
// Every SerializationTag passed to a serializer's Begin function must
// (same reference) be passed to the serializer's End function as well.
//
// See Also:
//    - <Serializer.Begin>
//    - <Serializer.End>
//    - <Deserializer.Begin>
//    - <Deserializer.End>
class ReflectExport(reflect) SerializationTag
{
public:
	// Enumeration: TagType
	//   ObjectTag - Starts an Object, the text specifies the Class of the object.
	//            (Ex. PersistentCategory::SerializeDynamic)
	//   PropertyTag - Starts a Property, the text specifies the Property name.
	//            (Ex. PersistentClass::SerializeProperties)
	//   AttributeTag - Starts an Attribute, the text specifies the Attribute name.
	//            (Ex. ArrayProperty::Serialize)
	//   ItemTag - Starts an Item, the text is ignored.
	//            (Ex. MapProperty::Serialize)
    enum TagType 
    { 
        ObjectTag,
        PropertyTag,
        AttributeTag,
        ItemTag,
        UnknownTag = -1
    };
    
    // Constructor: SerializationTag
    // Constructs a serialization tag with the given text and TagType.
    SerializationTag(const char *text, TagType type);

    // Constructor: SerializationTag
    // Constructs a serialization tag with the given TagType.
    SerializationTag(TagType type = UnknownTag);
    
    // Function: Text
    // a mutable version of the text
	string::MutableString Text();
	
	// Function: Text() const
    // a const version of the text
	string::ConstString Text() const;
	
	// Function: Type
    // a reference to the Type
    TagType &Type();
    
	// Function: Type() const
    // the type
    TagType Type() const;

	// Function: operator ==
	// Checks if the other tag has the same type and text.
	bool operator ==(const SerializationTag &tag) const;
private:
    TagType mType;
	string::ArrayString<128> mText;
};

}

#endif
