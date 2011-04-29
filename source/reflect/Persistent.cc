#include <reflect/Persistent.h>
#include <reflect/PropertyPath.h>
#include <reflect/PersistentClass.hpp>
#include <reflect/string/String.h>
#include <cstdarg>

DEFINE_REFLECTION(reflect::Persistent, "reflect::Persistent")
{
}

namespace reflect {

PropertyPath Persistent::Property(string::Fragment property_path)
{
	PropertyPath path;
	Persistent::TheClass()->ResolvePropertyPath(path, this, property_path);
	return path;
}

const function::Function *Dynamic::Function(string::Fragment function_name) const
{
	return GetClass()->FindFunction(function_name);
}

}
