#include <reflect/function/Function.hpp>
#include <reflect/PrimitiveTypes.h>
#include <reflect/Type.hpp>
#include <reflect/string/String.h>

#include <reflect/test/Test.h>

struct refonly
{
	refonly(int &i) : value(i) {}

	int &value;
};

int NativeAdd(int &i, int j)
{
	return i + j;
}

void NativeAccumulate(int &i, int j)
{
	i += j;
}

DEFINE_LOCAL_STATIC_REFLECTION(refonly, reflect::Type, "reflect::test::refonly")
{
}

TEST(FunctionStuff)
{
	ASSOCIATE(refonly);

	using namespace reflect;
	int i0 = 3;
	float i = float(i0);
	double j0 = 7.5, j = j0;
	char r;
	Variant args[2] = {Variant::FromRef(i), Variant::FromConstRef(j)};
	Variant result = Variant::FromRef(r);

	const function::Function *function0 = function::CreateFunction("+", &NativeAdd);
	function::Parameters function_args(function0);
	function_args.ref(args[0]).const_ref(args[1]);
	
	CHECK(function0->Call(function_args, result));
	CHECK(r == int(i) + int(j));
	CHECK(r == int(i0) + int(j0));

	const function::Function *function1 = function::CreateFunction("+=", &NativeAccumulate);
	CHECK(i == int(i0));
	CHECK(function1->Call(function_args, result));
	CHECK(i == int(i0) + int(j0));
	
	Variant fragment;
	fragment.Construct<string::String>();
	fragment.SetValue("abc");
	CHECK_EQUAL("\"abc\"", fragment.ToString());
}
