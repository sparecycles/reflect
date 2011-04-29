#include <reflect/Class.hpp>
#include <cstdio>

namespace reflect {

DynamicPointerType::DynamicPointerType()
{
}

Class::Class(void (*init_cb)())
	: ObjectType(init_cb)
{
	mSerializationClass = this;
}

Dynamic *Class::Cast(Dynamic *object) const
{
    if(object != 0 && object->GetClass()->DerivesType(this))
    {
        return object;
    }

    return 0;
}

const Dynamic *Class::Cast(const Dynamic *object) const
{
    if(object != 0 && object->GetClass()->DerivesType(this))
    {
        return object;
    }

    return 0;
}

const Class *Class::SerializesAs() const
{
	return mSerializationClass;
}

void Class::SetSerializationClass(const Class *serialization_class)
{
	// assert(this->Derives(serialization_class))
	mSerializationClass = serialization_class;
}

DynamicPointerType *Class::PointerType() const
{
	return &mDynamicPointerType;
}

void Class::SerializePointer(const Dynamic *, Reflector &reflector) const
{
	reflector.Fail();
}

void Class::DeserializePointer(Dynamic *&, Reflector &reflector) const
{
	reflector.Fail();
}

void Class::Initialize()
{
	ObjectType::Initialize();
}

void DynamicPointerType::Initialize(const Class *type, Dynamic *(*t2c)(void *), void *(*c2t)(Dynamic *))
{
	mValueClass = type;
	mTypedToDynamic = t2c;
	mDynamicToTyped = c2t;
//	if(TypeOf<Dynamic *>() != this)
//		SetParent(TypeOf<Dynamic *>());
}

bool DynamicPointerType::CanConvertFrom(const Class *other) const
{
	if(const DynamicPointerType *other_ptr_type = other % autocast)
	{
		return other_ptr_type->ValueClass()->DerivesType(ValueClass());
	}
	
	return false;
}

bool DynamicPointerType::ConvertValue(void *opaque, const void *other, const Type *other_type) const
{
	if(const DynamicPointerType *other_ptr_type = other_type % autocast)
	{
		void **target = static_cast<void **>(opaque);
		void *const*source = static_cast<void *const*>(other);
		
		Dynamic *castable_value = other_ptr_type->mTypedToDynamic(*source);
		
		if(castable_value == 0)
		{
			*target = mDynamicToTyped(castable_value);
			return true;
		}
		
		if(!castable_value->GetClass()->DerivesType(other_ptr_type->ValueClass()))
		{
			fprintf(stderr, "*** Confused pointer types!\n");
			return false;
		}
		
		if(castable_value->GetClass()->DerivesType(ValueClass()))
		{
			*target = mDynamicToTyped(castable_value);
			return true;
		}
	}
	
	return false;
}

void DynamicPointerType::Serialize(const void *in, void *out, Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		const Dynamic *value = (*mTypedToDynamic)(*static_cast<void *const*>(in));
		reflector.Check(reflector.GetSerializer().Serialize(value));
	}
	else
	{
		Dynamic *value = 0;
		reflector.Check(reflector.GetDeserializer().Deserialize(value));
		*static_cast<void **>(out) = (*mDynamicToTyped)(value);
	}
}

const Class *DynamicPointerType::ValueClass() const
{
	return mValueClass;
}

}

DEFINE_REFLECTION(reflect::DynamicPointerType, "reflect::DynamicPointerType")
{
}

DEFINE_REFLECTION(reflect::Class, "reflect::Class")
{
}
