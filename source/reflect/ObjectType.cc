#include <reflect/Class.hpp>

namespace reflect { 

ObjectType::ObjectType(void (*init_cb)())
	: Type(init_cb)
	, mFunctions(0)
    , mAnnotationMap(0)
{
}

void ObjectType::RegisterFunction(const char *name, const function::Function *func)
{
	Functions().insert(FunctionMap::value_type(string::SharedString::Literal(name), func));
}

const ObjectType::FunctionMap *ObjectType::GetFunctionMap() const
{
	return mFunctions;
}

ObjectType::FunctionMap &ObjectType::Functions()
{
	if(0 == mFunctions)
		mFunctions = new FunctionMap;

    return *mFunctions;
}

const function::Function *ObjectType::FindFunction(string::Fragment name) const
{
	if(string::SharedString function_name = string::SharedString::Find(name))
		return FindFunction(function_name);	
	
	return 0;
}

const function::Function *ObjectType::FindFunction(string::SharedString name) const
{
	for(const ObjectType *search_class = this;
		0 != search_class;
		search_class = search_class->Parent() % autocast)
	{
		if(const FunctionMap *function_map = search_class->GetFunctionMap())
		{
			FunctionMap::const_iterator find_it = function_map->find(name);

			if(find_it != function_map->end())
			{
				return find_it->second;
			}
		}
	}
	
	return 0;
}

void ObjectType::Initialize()
{
	Type::Initialize();
}

bool ObjectType::FunctionIterator::FindNext(const ObjectType *clazz)
{
	for(mCurrentClass = clazz;
		0 != mCurrentClass;
		mCurrentClass = mCurrentClass->Parent() % autocast)
	{
		if(const ObjectType::FunctionMap *props = mCurrentClass->GetFunctionMap())
		{
			if((mCurrentItem = props->begin()) != props->end())
				return true;
		}
	}
	
	return false;
}

ObjectType::FunctionIterator::FunctionIterator(const ObjectType *clazz)
{
	// if a non empty property map was found, mCurrentClass points
	// to the class with that map, and mCurrentItem is it's begin().
	FindNext(clazz);
}

const ObjectType::FunctionMap::value_type &ObjectType::FunctionIterator::operator *() const
{
	return *mCurrentItem;
}

const ObjectType::FunctionMap::value_type *ObjectType::FunctionIterator::operator ->() const
{
	return mCurrentItem.operator ->();
}

ObjectType::FunctionIterator::operator bool() const
{
	return mCurrentClass != 0;
}

bool ObjectType::FunctionIterator::next()
{
	// if there's no class, the iterator is not valid
	if(0 == mCurrentClass)
		return false;

	// step the iterator
	mCurrentItem++;
	
	// if we're not at the end yet, good
	if(mCurrentItem != mCurrentClass->GetFunctionMap()->end())
		return true;

	// otherwise find the next property up the class tree.
	return FindNext(mCurrentClass->Parent() % autocast);
}

void ObjectType::Annotate(string::SharedString key, string::SharedString subkey, Variant value)
{
	if(mAnnotationMap == 0)
	{
		mAnnotationMap = new AnnotationMap();
	}
	
	Annotations *&current_annotations = (*mAnnotationMap)[key];
	
	if(current_annotations == 0)
	{
		current_annotations = new Annotations();
	}
	
	(*current_annotations)[subkey] = value;
}

ObjectType::Annotations *ObjectType::GetAnnotations(string::FoundSharedString key) const
{
	if(mAnnotationMap)
	{
		AnnotationMap::const_iterator it = mAnnotationMap->find(key);
		if(it != mAnnotationMap->end())
		{
			return it->second;
		}
	}
	
	return 0;
}

Variant ObjectType::GetAnnotation(string::Fragment key, string::Fragment subkey) const
{
	if(Annotations *annotations = GetAnnotations(key))
	{
		Annotations::const_iterator it = annotations->find(string::SharedString::Find(subkey));
	
		if(it != annotations->end())
			return it->second;
	}
	
	return Variant::Void();
}

bool ObjectType::HasAnnotation(string::Fragment key, string::Fragment subkey) const
{
	if(Annotations *annotations = GetAnnotations(key))
	{
		return annotations->find(string::SharedString::Find(subkey)) != annotations->end();
	}
	
	return false;
}


}

DEFINE_REFLECTION(reflect::ObjectType, "reflect::ObjectType")
{
}
