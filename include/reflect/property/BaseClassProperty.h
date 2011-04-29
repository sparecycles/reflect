#ifndef REFLECT_PROPERTY_BASECLASSPROPERTY_H_
#define REFLECT_PROPERTY_BASECLASSPROPERTY_H_

#include <reflect/DataProperty.h>

namespace reflect { namespace property {

// Class: BaseClassProperty
//
// An implemetation of <Property> which serializes a base class
// of another class.  
//
// A base jump function is required because ObjectType might not subclass BaseType
// in all cases.  For instance, <PersistentClass> uses <Persistent> for all its
// properties.
template<typename BaseObjectType>
class BaseClassProperty : public DataProperty
{
public:
	// Constructor: BaseClassProperty
    BaseClassProperty(BaseObjectType *(*base_jumper)(void *));

    //virtual
    void Serialize(const void *in, void *out, Reflector &reflector) const;

    //virtual
    Type *DataType() const;

    //virtual
    bool WriteData(void *object, const Variant &opaque) const;

    //virtual
    bool ReadData(const void *object, Variant &opaque) const;

	//virtual
    bool RefData(const void *in, void *out, Variant &) const;

private:
	// Member: mBaseJumper
	BaseObjectType *(*mBaseJumper)(void *);
};

} }

#endif
