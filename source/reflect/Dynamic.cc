#include <reflect/Dynamic.h>
#include <reflect/Class.hpp>

namespace reflect {

static void DescribeDynamic()
{
	TypeOf<Dynamic>()->SetName("reflect::Dynamic");
	TypeOf<Dynamic *>()->SetName("*");
	
	Class::DescriptionHelper<Dynamic> helper;
	helper.Functions
		("GetClass", &Dynamic::GetClass)
		;
}

static Class sRootClass(&DescribeDynamic);

Class *Dynamic::TheClass()
{
    return &sRootClass;
}

Class *Dynamic::GetClass() const
{
    return TheClass();
}

Dynamic::Dynamic()
{
}

Dynamic::~Dynamic()
{
}

}
