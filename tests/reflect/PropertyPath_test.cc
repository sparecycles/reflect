#include <reflect/config/config.h>
#include <reflect/Persistent.h>
#include <reflect/string/String.h>
#include <reflect/PersistentClass.hpp>
#include <reflect/StructType.hpp>
#include <reflect/EnumType.hpp>
#include <reflect/PropertyPath.h>
#include <reflect/test/Test.h>
#include <cstdio>

#include <vector>
#include <map>

using namespace reflect;

struct vec
{
	vec()
		: x(1), y(2), z(3) {}
	float x,y,z;
	
	float GetX() const { return x; }
	void SetX(float value) { x = value; }
};

struct qbase
{
	Persistent *qp;
	
	qbase() : qp(0) {}
};

DECLARE_STATIC_REFLECTION(local, vec, reflect::StructType)
DECLARE_STATIC_REFLECTION(local, qbase, reflect::StructType)


class StructTest : public vec, public qbase
{
	DECLARE_REFLECTION(vec)
public:
	StructTest()
		: xx(0)
		, pp(0)
		, ee(C)
	{
	}
	
	enum TestEnum
	{
		A, B, C
	};

	int xx;
	vec aa[2];
	Persistent *pp;
	TestEnum ee;
};

class PathTester 
	: public Persistent
	, public vec
{
	DECLARE_REFLECTION(Persistent)
public:
	PathTester()
		: link(0)
		, clazz(TypeOf<PathTester>())
	{
	}

	PathTester *link;
	std::vector<PathTester *> links;
	std::vector<int> data;
	std::map<int,int> map;
	std::map<int, PathTester *> refmap;
	string::String string;
	StructTest compound;
	Class *clazz;
};

DEFINE_LOCAL_STATIC_REFLECTION(StructTest::TestEnum, reflect::EnumType, "test::StructTest::TestEnum")
{
	Values
		(StructTest::A, "A")
		(StructTest::B, "B")
		(StructTest::C, "C")
		;
}

struct X
{
	int value;
	char buf[256];
	void *allocate() { value = 7; return buf; }
};

struct Y
{
	Y(int x) : x_value(x) {}
	int x_value;
};

void *operator new(size_t, X &x) { return x.allocate(); }

DEFINE_REFLECTION(PathTester, "test::PathTester")
{
	+ Concrete;
		
	Properties
		.base<vec>("base")
		("link", &PathTester::link)
			["tooltip"]("link tool tip")
		("links", &PathTester::links, Array)
		("data", &PathTester::data, Array)
		("map", &PathTester::map, Map)
		("refmap", &PathTester::refmap, Map)
		("string", &PathTester::string)
		("compound", &PathTester::compound)
		("class", &PathTester::clazz)
		;
}

DEFINE_REFLECTION(StructTest, "test::StructTester")
{
	Members
		.base<vec>()
		.base<qbase>()
		(&StructTest::ee)
		(&StructTest::xx)
		(&StructTest::aa, Array)
		(&StructTest::pp)
		;
}

DEFINE_STATIC_REFLECTION(vec, "test::vec")
{
	Members
		(&vec::GetX, &vec::SetX)
		(&vec::y)
		(&vec::z)
		;
}

DEFINE_STATIC_REFLECTION(qbase, "test::qbase")
{
	Members
		(&qbase::qp)
		;
}

TEST(EmptyPath)
{
	ASSOCIATE(PathTester);
	ASSOCIATE(PersistentClass);

	PathTester a;
	PropertyPath path;

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, ""));	
}

TEST(EmptyPathNullObject)
{
	PropertyPath path;

	CHECK(false == PersistentClass::ResolvePropertyPath(path, static_cast<PathTester*>(0), ""));
}

TEST(StandardPath)
{
	ASSOCIATE(PathTester);
	ASSOCIATE(PersistentClass);

	PathTester a;
	PropertyPath path;

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "link"));

	CHECK_EQUAL("null", path.Read().c_str());
}

TEST(LoopPath)
{
	PathTester a;
	PropertyPath path;

	a.link = &a;

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "link.link"));

	CHECK(path.Read() != "null");
	CHECK(path.IsRef());
	CHECK(path.ReadValue<Dynamic *>() == &a);
}

TEST(ChainedPath)
{
	ASSOCIATE(PathTester);
	ASSOCIATE(PersistentClass);

	PathTester a;
	PathTester b;
	PropertyPath path;

	a.link = &b;

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "link"));

	CHECK(path.Read() != "null");
	CHECK(path.IsRef());
	CHECK(path.ReadValue<Dynamic *>() == &b);

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "link.link"));

	CHECK(path.Read() == "null");
	CHECK(path.IsRef());
	CHECK(path.ReadValue<Dynamic *>() == 0);
}

TEST(ArrayPath)
{
	ASSOCIATE(int);
	ASSOCIATE(Dynamic *);
	ASSOCIATE(string::ConstString);
	ASSOCIATE(PathTester);

	PathTester a;
	PathTester b;
	a.links.push_back(&a);
	a.links.push_back(&b);

	PropertyPath path;
	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "links[0]"));
	CHECK(path.IsRef());
	CHECK(&a == path.ReadValue<Dynamic *>());
	
	b.map[1] = 3;
	CHECK(a.Property("links[0]").GetDataType() == TypeOf<PathTester *>());
	
{
	Dynamic *ptr;
	CHECK(a.Property("links[0]").ReadValue(ptr));
}

	CHECK(PersistentClass::ResolvePropertyPath(path, &a, "links[1]"));
	CHECK(path.IsRef());
	CHECK_EQUAL(&b, path.ReadValue<Dynamic *>());

	CHECK(PersistentClass::ResolvePropertyPath(path, &a, "links"));
	CHECK(!path.IsData());
	CHECK(!path.IsDatum());
	CHECK(path.IsArray());
	CHECK(!path.IsRef());
	CHECK(path.ArrayItem(0).IsRef());
	CHECK(path.ArrayItem(1).IsRef());

	a.data.push_back(2);
	a.data.push_back(3);

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "data"));
	CHECK(false == path.IsRef());
	CHECK(path.IsArray());
	CHECK(false == path.ArrayItem(0).IsRef());
	CHECK(false == path.ArrayItem(1).IsRef());

	path.ArrayItem(0).Write("10");
	path.ArrayItem(1).Write("20");

	CHECK_EQUAL(10, a.data[0]);
	CHECK_EQUAL(20, a.data[1]);

	CHECK(true == PersistentClass::ResolvePropertyPath(path, &a, "links[1].link"));
	CHECK(0 == path.ReadValue<Dynamic *>());

	CHECK(0 == b.link);
	CHECK(path.WriteValue(&a));
	CHECK(&a == path.ReadValue<Dynamic *>());
	CHECK(&a == b.link);
}

TEST(EasyReadWrite)
{
	PathTester a;
	PathTester b;
	a.links.push_back(&a);
	a.links.push_back(&b);

	Dynamic *alink_0;
	CHECK(a.Property("links[0]").ReadValue(alink_0));
	CHECK(&a == alink_0);

	Dynamic *alink_1;
	CHECK(a.Property("links[1]").ReadValue(alink_1));
	CHECK(&b == alink_1);

	CHECK(0 == b.link);
	CHECK(true == a.Property("links[1].link").WriteValue(&a));
	CHECK(&a == b.link);

	Dynamic *alink_1_link;
	CHECK(a.Property("links[1].link").ReadValue(alink_1_link));
	CHECK(&a == alink_1_link);
}

TEST(EasyMacAndCheese_ReadWrite)
{
	PathTester a;
	PathTester b;
	a.links.push_back(&a);
	a.links.push_back(&b);

	CHECK(&a == a.Property("links[0]").ReadValue<Dynamic *>());
	CHECK(&b == a.Property("links[1]").ReadValue<Dynamic *>());

	CHECK(0 == b.link);
	CHECK(true == a.Property("links[1].link").WriteValue(&a));
	CHECK(&a == b.link);

	CHECK(&a == a.Property("links[1].link").ReadValue<Dynamic *>());
}

TEST(StringReadWrite)
{
	PathTester a;
	CHECK(a.Property("links[0]").Read() == "");
	CHECK(a.Property("string").ReadValue<string::String>() == "");

	a.string = "Hello";
	CHECK(a.Property("string").ReadValue<string::String>() == "Hello");
	CHECK(a.Property("string").Read() == "\"Hello\"");

	CHECK(a.Property("string").WriteValue<string::Fragment>("Bye"));
	CHECK(a.Property("string").Read() == "\"Bye\"");

	CHECK(a.Property("string").WriteValue<string::Fragment>("A Quote: \""));
	CHECK(a.Property("string").ReadValue<string::String>() == "A Quote: \"");
	CHECK(a.Property("string").ReadValue<string::String>() == a.string);
	CHECK(a.Property("string").Read() == "\"A Quote: \\\"\"");
}

TEST(PropertyPath_Map)
{
	PathTester x;
	
	PropertyPath path = x.Property("map{1}");
	
	CHECK_EQUAL(false, path.DataExists());
	CHECK(path.Write("33"));
	CHECK_EQUAL(true, path.DataExists());
	
	int value = -1;
	CHECK(path.ReadValue(value));
	CHECK_EQUAL(33, value);
	
	CHECK(x.Property("map{472}").GetDataType() == TypeOf<int>());
}

TEST(PropertyPath_RefMap)
{
	PathTester x;
	
	PropertyPath path = x.Property("refmap{1}");
	
	CHECK_EQUAL(false, path.DataExists());
	CHECK(path.WriteValue(&x));
	CHECK_EQUAL(true, path.DataExists());
	CHECK(path.ReadValue<Dynamic *>() == &x);

	CHECK(x.Property("link.link.refmap{3}").GetDataType() == 0);
	x.link = &x;
	CHECK(x.Property("link.link.refmap{3}").GetDataType()->GetClass()->Derives<DynamicPointerType>());
}

TEST(PropertyPath_Compound)
{
	PathTester x;
	StructTest compound;
	
	x.compound.qp = &x;
	
	PropertyPath path = x.Property("compound");
	
	CHECK_EQUAL(true, path.DataExists());

	CHECK(TypeOf<StructTest>()->Parent() == TypeOf<vec>());

	CHECK(path.Write("7 1.2 1.3 null \"B\" 38 1.1 1.2 1.3 1.4 1.5 .16 null"));

	CHECK_EQUAL(38, x.compound.xx);
	//CHECK_EQUAL(1.2, x.compound.vec::y);
	//CHECK_EQUAL(1.3, x.compound.vec::z);
	
	compound.x = 1;
	compound.y = 4;
	compound.z = 11;
	compound.pp = &x;
	CHECK(path.WriteValue(compound));
	
	CHECK_EQUAL(compound.x, x.compound.x);
	CHECK_EQUAL(compound.y, x.compound.y);
	CHECK_EQUAL(compound.z, x.compound.z);
	
	CHECK_EQUAL(1, x.compound.x);
	CHECK_EQUAL(4, x.compound.y);
	CHECK_EQUAL(11, x.compound.z);
	CHECK(compound.pp == &x);
	CHECK(x.compound.pp != &x);
	delete x.compound.pp;
}

TEST(IO)
{
	PathTester x;
	x.link = &x;
	
	utility::StandardInOutReflector reflector;
	
	reflector << 1 << 3 << 5 << 7;
	
	int test;
	
	reflector >> test;
	CHECK_EQUAL(1, test);
	reflector >> test;
	CHECK_EQUAL(3, test);
	reflector >> test;
	CHECK_EQUAL(5, test);
	reflector >> test;
	CHECK_EQUAL(7, test);

	reflector << reflect::string::Fragment("Hello") << x;
	//fprintf(stderr, "%s\n", reflector.Data().c_str());
	PathTester copy;
	reflect::string::String hello;
	reflector >> hello;
	reflector << 33;
	reflector >> copy >> test;

	CHECK_EQUAL("Hello", hello);
	CHECK_EQUAL(33, test);
	CHECK(copy.link && copy.link->link == copy.link);

	CHECK(copy.link != x.link);

	// while it is possible to make copy.link (a pointer) reference copy (an object),
	// it is not possible to guarantee, so object references only work on pointers, not
	// objects.
	CHECK(copy.link != &copy);

	// A serialized pointer will backreference itself, however:
	{
		utility::StandardInOutReflector reflector;
		PathTester *copy_pointer = 0;
		reflector << &x;
		reflector >> copy_pointer;
		CHECK(copy_pointer != 0);
		CHECK(copy_pointer && copy_pointer->link != &x);
		CHECK(copy_pointer && copy_pointer->link == copy_pointer);
		delete copy_pointer;
	}
}

TEST(ShallowIO)
{
	PathTester x;
	x.link = &x;

	utility::ShallowInOutReflector reflector;
	
	reflector << 1 << 3 << 5 << 7;
	
	int test;
	
	reflector >> test;
	CHECK_EQUAL(1, test);
	reflector >> test;
	CHECK_EQUAL(3, test);
	reflector >> test;
	CHECK_EQUAL(5, test);
	reflector >> test;
	CHECK_EQUAL(7, test);
	
	reflector << reflect::string::Fragment("Hello") << x;
	PathTester copy;
	reflect::string::String hello;
	reflector >> hello;
	reflector << 33;
	reflector >> copy >> test;

	CHECK(hello == "Hello");
	CHECK_EQUAL(33, test);
	
	CHECK(copy.link == &x);
}


TEST(Failures)
{
	PathTester x;
	
	PropertyPath path;
	
	CHECK(0 == path.ReadValue<Dynamic *>());
	CHECK_EQUAL(false, path.WriteValue<Dynamic *>(0));
	CHECK_EQUAL(false, path.Write(""));
	
	path = x.Property("map{0}");
	
	CHECK_EQUAL(false, path.WriteValue<Dynamic *>(0));
	
	path = x.Property("refmap{0}");

	CHECK_EQUAL(false, path.Write("0"));
	// but...
	CHECK_EQUAL(true, path.Write("null"));
	
	x.data.resize(1);
	path = x.Property("data[0]");
	
	CHECK_EQUAL(false, path.Write("  Hello"));
	CHECK_EQUAL(true, path.Write("21"));
	
	CHECK_EQUAL("link tool tip", x.Property("link").Annotation("tooltip").AsValue<string::String>().c_str());
}
