#include <reflect/test/Test.h>
#include <reflect_js/JavaScript.h>
#include <reflect/Persistent.h>
#include <reflect/StructType.hpp>
#include <reflect/PropertyPath.h>
#include <reflect/PersistentClass.hpp>
#include <reflect/string/String.h>
#include <reflect/PrimitiveTypes.h>

#include <cstdio>

struct TestVector 
{
	TestVector(float x_ = 0, float y_ = 0, float z_ = 0)
		: x(x_)
		, y(y_)
		, z(z_)
	{}
	
	~TestVector()
	{
		x = y = z = 0;
	}
	
	void show() const 
	{
		printf("%g, %g, %g\n", x, y, z);
	}

	float x, y, z;
};

DECLARE_STATIC_REFLECTION(local, TestVector, reflect::StructType)

class TestType : public reflect::Persistent
{
	DECLARE_REFLECTION(reflect::Persistent)
public:
	TestType()
		: mNumber(0)
		, mName()
		, mDeleted(0)
	{
	}

	void SetWhenDeleted(bool &deleted)
	{
		mDeleted = &deleted;
	}
	
	reflect::string::ConstString Name() { return mName; }
	int Number() { return mNumber; }
	
	int AppendName(const char *ex)
	{
		if(mName.size())
			mName += ',';
		mName += ex;
		return mName.size();
	}
	
	void AddNumber(int amount)
	{
		mNumber -= amount;
	}

	TestVector vec(float x, float y)
	{
		return TestVector(x, y, x*y);
	}
	
	void PrintVec(const TestVector &v)
	{
		std::printf("ta: %d,%d\n", test_array[0], test_array[1]);
		std::printf("Hi, %g, %g, %g\n", v.x, v.y, v.z);
	}
	
	~TestType()
	{
		if(mDeleted) *mDeleted = true;
	}
	
private:
	int mNumber;
	reflect::string::String mName;
	int test_array[2];
	bool *mDeleted;
};

DEFINE_STATIC_REFLECTION(TestVector, "reflect::js::TestVector")
{
	+ Concrete;

	Members
		(&TestVector::x)
		(&TestVector::y)
		(&TestVector::z)
		;
		
	Functions
		("show", &TestVector::show)
		;
}

DEFINE_REFLECTION(TestType, "reflect::js::TestType")
{
	+ Concrete;
	
	Properties
		("name", &TestType::mName)
		("number", &TestType::mNumber)
		("ta", &TestType::test_array, Array)
		;
		
	Functions
		("AppendName", &TestType::AppendName)
		("AddNumber", &TestType::AddNumber)
		("vec", &TestType::vec)
		("PrintVec", &TestType::PrintVec)
		;
}

struct JSFixture
{
	reflect::js::JavaScriptContext *ctx;
	
	JSFixture()
	{
		ctx = new reflect::js::JavaScriptContext;
	}
	
	~JSFixture()
	{
		delete ctx;
	}
};

FIXTURE(ReadNumber, JSFixture)
{
#if 0
	bool test_object_deleted = false;
	ASSOCIATE(TestType);
	ASSOCIATE(TestType *);

	Dynamic *object = 0;
	CHECK(ctx->Eval("test_object = new Native.reflect.js.TestType", object));
	TestType *test_object = object % reflect::autocast;

	test_object->SetWhenDeleted(test_object_deleted);
	
	CHECK(test_object != 0);
	CHECK_EQUAL(0, test_object->Number());
	CHECK_EQUAL("", test_object->Name());
	
	// integer assignment
	CHECK(ctx->Eval("test_object.number = 100"));
	CHECK_EQUAL(100, test_object->Number());
	
	// string assignment
	CHECK(ctx->Eval("test_object.name = 'name'"));
	CHECK_EQUAL("name", test_object->Name());

	int number = 0;
	test_object->Property("number").Writef("335");
	CHECK(ctx->Eval("test_object.number", number));
	CHECK_EQUAL(335, number);
	
	unsigned len;
	CHECK(ctx->Eval("test_object.AppendName(' is silly')", len));
	CHECK_EQUAL("name, is silly", test_object->Name());
	CHECK_EQUAL(test_object->Name().size(), len);
	
	reflect::string::String type_result;
	CHECK(ctx->Eval("typeof test_object", type_result));
	CHECK(false == test_object_deleted);
	CHECK(ctx->Eval("delete test_object;"));
	ctx->GC();
	CHECK(test_object_deleted);
#endif
	CHECK(ctx->Eval("Console()"));
}
