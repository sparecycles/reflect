#include <reflect/Class.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/test/Test.h>

using namespace reflect;

TEST(LoneClass)
{
	Class alone;
	Type *classlink = Type::LoadTypes();

	CHECK(alone.Parent() == NULL);
	CHECK(alone.Child() == NULL);

	Class::UnloadTypes(classlink);
}

TEST(SingleChild)
{
	// define and load a local test hierarchy
	Class parent;
	Class child;
	child.SetParent(&parent);
	Type *classlink = Type::LoadTypes();

	CHECK(parent.Parent() == NULL);

	CHECK(child.Parent() == &parent);
	CHECK(child.Sibling() == &child);

	// clean up global space
	Type::UnloadTypes(classlink);
}

TEST(TwoChild)
{
	// define and load a local test hierarchy
	Class parent;
	Class child0;
	Class child1;
	child0.SetParent(&parent);
	child1.SetParent(&parent);
	Type *classlink = Type::LoadTypes();

	CHECK(parent.Parent() == NULL);
//	CHECK(parent.Sibling() == &parent);

	CHECK(child0.Parent() == &parent);
	CHECK(child1.Parent() == &parent);
	CHECK(child0.Sibling() == &child1);
	CHECK(child1.Sibling() == &child0);

	// clean up global space
	Type::UnloadTypes(classlink);
}

TEST(SubclassIdentification)
{
	// define and load a local test hierarchy
	Class parent;
	Class child0;
	Class child1;
	Class child1sub;
	child0.SetParent(&parent);
	child1.SetParent(&parent);
	child1sub.SetParent(&child1);
	Type *classlink = Type::LoadTypes();

	CHECK(child1.DerivesType(&parent));
	CHECK(child0.DerivesType(&parent));
	CHECK(child1sub.DerivesType(&parent));
	CHECK(child1sub.DerivesType(&child1));
	CHECK(child1sub.DerivesType(&child1sub));
	CHECK(false == child1sub.DerivesType(&child0));
	CHECK(false == parent.DerivesType(&child1sub));
	CHECK(false == parent.DerivesType(&child1));
	CHECK(false == parent.DerivesType(&child0));

	// clean up global space
	Type::UnloadTypes(classlink);
}

TEST(Primitive)
{
	CHECK_EQUAL(TypeOf<int>()->Name(), "int");
	CHECK_EQUAL(TypeOf<float>()->Name(), "float");
}
