#ifndef REFLECT_PROPERTY_VARIANTPROPERTY_H_
#define REFLECT_PROPERTY_VARIANTPROPERTY_H_

#include <reflect/Property.h>
#include <reflect/config/config.h>

namespace reflect { namespace property {

// Class: VariantProperty
//
// An implemetation of <Property> using a callback and a Reflector
// to access the data.
template<typename ObjectType>
class VariantProperty : public Property
{
public:
	// Constructor: VariantProperty
    VariantProperty(void (ObjectType::*serializer)(Reflector &) const, void (ObjectType::*deserializer)(Reflector &));
    
    //virtual
    void Serialize(const void *in, void *out, Reflector &reflector) const;

private:
	// Member: mSerializer
    void (ObjectType::*mSerializer)(Reflector &) const;
	// Member: mDeserializer
    void (ObjectType::*mDeserializer)(Reflector &);
};

} }

#endif
