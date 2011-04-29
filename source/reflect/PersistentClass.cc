#include <reflect/PersistentClass.hpp>
#include <reflect/Serializer.h>
#include <reflect/Deserializer.h>
#include <reflect/Reflector.h>
#include <reflect/SerializationTag.h>
#include <reflect/autocast.h>
#include <reflect/Persistent.h>
#include <reflect/PropertyPath.h>

#include <cstdio>

DEFINE_REFLECTION(reflect::PersistentClass, "reflect::PersistentClass")
{
}

namespace reflect {

PersistentClass::PersistentClass(void (*init_cb)())
    : Class(init_cb) 
    , mProperties(0)
{}

Persistent *PersistentClass::Construct(void *data) const
{
	Persistent *result = translucent_cast<Persistent *>(Class::Construct(data));
	
	return result;
}

void PersistentClass::Destruct(Persistent *data) const
{
	Class::Destruct(opaque_cast(data));
}

Persistent *PersistentClass::Create() const
{
	Persistent *result = Construct(new char[Size()]);

    return result;
}

void PersistentClass::RegisterProperty(const char *name, const Property *property)
{
	Properties().insert(PropertyMap::value_type(string::SharedString::Literal(name), property));
}

const PersistentClass::PropertyMap *PersistentClass::GetPropertyMap() const
{
	return mProperties;
}

PersistentClass::PropertyMap &PersistentClass::Properties()
{
	if(0 == mProperties)
		mProperties = new PropertyMap;

    return *mProperties;
}

const Property *PersistentClass::FindProperty(string::Fragment name) const
{
	if(string::SharedString property_name = string::SharedString::Find(name))
	{
		return FindProperty(property_name);	
	}

	return 0;
}

const Property *PersistentClass::FindProperty(string::SharedString name) const
{
	for(const PersistentClass *search_class = this;
		0 != search_class;
		search_class = search_class->Parent() % autocast)
	{
		if(const PropertyMap *property_map = search_class->GetPropertyMap())
		{
			PropertyMap::const_iterator find_it = property_map->find(name);

			if(find_it != property_map->end())
			{
				return find_it->second;
			}
		}
	}
	
	return 0;
}

void PersistentClass::SerializePointer(const Dynamic *in, Reflector &reflector) const
{
	const Persistent *persist = in % autocast;
	reflector.GetSerializer().Reference(in);
	SerializeProperties(persist, reflector);
}

void PersistentClass::DeserializePointer(Dynamic *&out, Reflector &reflector) const
{
	Persistent *object = Create() % autocast;

	if(0 == object)
		reflector.Fail();

	reflector.GetDeserializer().Reference(object);
	DeserializeProperties(object, reflector);

	out = object;
}

void PersistentClass::SerializeProperties(const Persistent *object, Reflector &reflector) const
{
    Serializer &serializer = reflector;
	
	for(PropertyIterator it = this; it; it.next())
	{
        SerializationTag tag(it->first.c_str(), SerializationTag::PropertyTag);
        reflector.Check(serializer.Begin(tag));
        reflector.Check(serializer.SerializeProperty(object, it->second));
        reflector.Check(serializer.End(tag));
	}

#if 0 // this alternate pattern serializes base classes before child classes... remove or switch back to?
    if(PersistentClass *parent = AutoCast(Parent()))
    {
        parent->SerializeProperties(object, reflector);
    }

    for(PropertyMap::const_iterator it = Properties().begin(); it != Properties().end(); ++it)
    {
        const PropertyMap::key_type &name = it->first;
        const Property *prop = it->second;

        SerializationTag tag(name.c_str(), SerializationTag::PROPERTY);
        reflector.Check(serializer.Begin(tag));
        reflector.Check(serializer.SerializeProperty(object, prop));
        reflector.Check(serializer.End(tag));
    }
#endif
}

void PersistentClass::DeserializeProperties(Persistent *object, Reflector &reflector) const
{
    SerializationTag tag;
    Deserializer &deserializer = reflector;

    while(deserializer.Begin(tag, SerializationTag::PropertyTag))
    {
        const Property *prop = FindProperty(tag.Text().c_str());

        if(prop)
		{
	        reflector.Check(deserializer.DeserializeProperty(object, prop));
		}
		else
        {
            // warning, unknown property
			printf("UNKNOWN Property to class %s: '%s'\n", Name(), tag.Text().c_str());
        }

        reflector.Check(deserializer.End(tag));
    }
}

void PersistentClass::Serialize(const void *in, void *out, Reflector &reflector) const
{
	SerializationTag tag("", SerializationTag::ItemTag);

	if(reflector.Serializing())
	{
		if(reflector.GetSerializer().Begin(tag))
		{
			const reflect::Persistent *object = static_cast<const reflect::Persistent *>(in);
			SerializeProperties(object, reflector);
			reflector.Check(reflector.GetSerializer().End(tag));
		}
		else reflector.Fail();
	}
	else
	{
		if(reflector.GetDeserializer().Begin(tag, SerializationTag::ItemTag))
		{
			reflect::Persistent *object = static_cast<reflect::Persistent *>(out);
			DeserializeProperties(object, reflector);
			reflector.Check(reflector.GetDeserializer().End(tag));
		}
		else reflector.Fail();
	}
}


/*virtual*/ void PersistentClass::Initialize()
{
	Class::Initialize();
}

bool PersistentClass::ResolvePropertyPath(PropertyPath &result, const Persistent *object, string::Fragment path)
{
	return PersistentClass::ResolvePropertyPath(result, object, NULL, path);
}

bool PersistentClass::ResolvePropertyPath(PropertyPath &result, Persistent *object, string::Fragment path)
{
	return PersistentClass::ResolvePropertyPath(result, object, object, path);
}

bool PersistentClass::ResolvePropertyPath(PropertyPath &result, const Persistent *const_object, Persistent *object, string::Fragment path)
{
	result = PropertyPath(const_object, object);
	if(result.Advance(path))
	{
		return true;
	}
	else
	{
		result = PropertyPath();
		return false;
	}
}

bool PersistentClass::PropertyIterator::FindNext(const PersistentClass *clazz)
{
	for(mCurrentClass = clazz;
		0 != mCurrentClass;
		mCurrentClass = mCurrentClass->Parent() % autocast)
	{
		if(const PersistentClass::PropertyMap *props = mCurrentClass->GetPropertyMap())
		{
			if((mCurrentItem = props->begin()) != props->end())
				return true;
		}
	}
	
	return false;
}

PersistentClass::PropertyIterator::PropertyIterator(const PersistentClass *clazz)
{	
	// if a non empty property map was found, mCurrentClass points
	// to the class with that map, and mCurrentItem is it's begin().
	FindNext(clazz);
}

const PersistentClass::PropertyMap::value_type &PersistentClass::PropertyIterator::operator *() const
{
	return *mCurrentItem;
}

const PersistentClass::PropertyMap::value_type *PersistentClass::PropertyIterator::operator ->() const
{
	return mCurrentItem.operator ->();
}

PersistentClass::PropertyIterator::operator bool() const
{
	return mCurrentClass != 0;
}

bool PersistentClass::PropertyIterator::next()
{
	// if there's no class, the iterator is not valid
	if(0 == mCurrentClass)
		return false;

	// step the iterator
	mCurrentItem++;
	
	// if we're not at the end yet, good
	if(mCurrentItem != mCurrentClass->GetPropertyMap()->end())
		return true;

	// otherwise find the next property up the class tree.
	return FindNext(mCurrentClass->Parent() % autocast);
}


}
