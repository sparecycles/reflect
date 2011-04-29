#include <reflect/Variant.h>
#include <reflect/test/Test.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/Reflection.h>
#include <reflect/config/ReflectExport.h>
#include <reflect/Type.hpp>

#include <reflect/string/String.h>
#include <reflect/string/Fragment.h>

using namespace reflect;

struct DeletionChecker
{
	DeletionChecker() : deleted(0) {}

	~DeletionChecker() { if(deleted) *deleted = true; }
	
	bool *deleted;
};

DEFINE_LOCAL_STATIC_REFLECTION(DeletionChecker, reflect::Type, "")
{
	+ Concrete;
}

TEST(VariantConstruction)
{
	Variant variant;

	CHECK(0 == variant.GetType());
}

TEST(VariantTypeAquisition)
{
	Variant variant;
	CHECK(variant.SetValue(3));
	CHECK(TypeOf<int>() == variant.GetType());

	CHECK(variant.SetValue(3L));
	CHECK(TypeOf<int>() == variant.GetType());
}

TEST(VariantDeletion)
{
	Variant variant;

	variant.BindType<DeletionChecker>();

	CHECK(variant.Construct());
	CHECK(variant.CanRefAs<DeletionChecker>());

	bool deleted = false;

	if(variant.CanRefAs<DeletionChecker>())
		variant.AsRef<DeletionChecker>().deleted = &deleted;
	
	CHECK(!deleted);

	{
		Variant copy = variant;
		CHECK(!deleted);
	} // copy destructs here

	CHECK(!deleted);

	// copy the variant, one more ref on the allocation
	{
		Variant copy = variant;

		// clear the original
		variant = Variant();
		
		// copy should hold the reference
		CHECK(!deleted);
	} // copy goes out of scope, destructs here

	// no copy now, deleted.
	CHECK(deleted);
}

TEST(VariantCasting)
{
	int x = 3;
	Variant value = Variant::FromRef(x);

	CHECK(value.SetValue(8.0));
	
	CHECK_EQUAL(8, x);

	CHECK(false == value.SetValue("Hello World"));
	
	CHECK_EQUAL(8, x);

	string::String str = "Hello";
	value = Variant::FromRef(str);

	CHECK(value.SetValue(string::Fragment("Bye")));

	CHECK_EQUAL("Bye", str);
}
