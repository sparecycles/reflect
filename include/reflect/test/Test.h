#ifndef REFLECT_TEST_TEST_H_
#define REFLECT_TEST_TEST_H_

#include <reflect/Variant.h>
#include <reflect/Class.h>
#include <reflect/Reflection.hpp>

namespace reflect { namespace test {

class ReflectExport(reflect) TestClass : public Class
{
	DECLARE_REFLECTION(Class)
public:
	TestClass(void (*init)());
	bool RunTest() const;
	void SetTestFunction(void (*test_fun)());
	void SetTestInfo(const char *filename, int lineno);
	const char *File() const;
	static bool RunAllTests(bool verbose = false);
private:
	/*virtual*/ void RegisterName();
	void (*mTestFunction)();
	const char *mFilename;
	int mLineNo;
};

// Class: Test
//
// Each subclass of Test implements a unit test.
// 
// See Also:
//   - <TEST> and <FIXTURE>: Macros for making tests.
class ReflectExport(reflect) Test : public Dynamic
{
	DECLARE_REFLECTION_EX(Dynamic, TestClass)
protected:
	static void Check(bool result, const char *what, const char *file, int line);
	static void CheckEqual(bool result, const char *lhs, const char *rhs,
		const Variant &value1, const Variant &value2, const char *file, int line);
	static void AssociateType(const Type *);
};

template<typename Test>
struct TestFunction
{
	static void test()
	{
		Test test;
		test.Run();
	}
};

#define REFLECT_TEST_MAKE_STRING(x) REFLECT_TEST_MAKE_STRING_(x)
#define REFLECT_TEST_MAKE_STRING_(x) #x

#define MAKE_TEST(name__, fixture__) \
	class Test##name__ \
           : public ::reflect::test::Test, private fixture__ \
    {  DECLARE_REFLECTION(::reflect::test::Test) \
	   public: /*virtual*/ void Run(); }; \
	\
	static void REFLECT_UNIQUENAME(ClassInitializer__)() { \
		reflect::test::TestClass *type = reflect::TypeOf<Test##name__>(); \
		type->SetParent(reflect::Signature<Test##name__::BaseType>::TheType()); \
        type->SetName(REFLECT_TEST_MAKE_STRING(Test##name__)); \
        type->SetTestFunction(&::reflect::test::TestFunction<Test##name__>::test); \
        type->SetTestInfo(__FILE__, __LINE__); \
	} \
	\
	static Test##name__::ClassType REFLECT_UNIQUENAME(sClass)(&REFLECT_UNIQUENAME(ClassInitializer__)); \
	Test##name__::ClassType *Test##name__::TheClass() { return &REFLECT_UNIQUENAME(sClass); } \
	Test##name__::ClassType *Test##name__::GetClass() const { return Test##name__::TheClass(); } \
	void Test##name__::Run()

struct EmptyFixture {};

template<typename LHS, typename RHS>
inline bool ReflectTestCheckEqualCompare(const LHS &lhs, const RHS &rhs)
{
	return lhs == rhs;
} 

template<typename LHS, unsigned size>
inline bool ReflectTestCheckEqualCompare(const LHS &lhs, const char (&rhs)[size])
{
	return ReflectTestCheckEqualCompare(lhs, reflect::string::ConstString(rhs));
} 

template<unsigned size, typename RHS>
inline bool ReflectTestCheckEqualCompare(const char (&lhs)[size], const RHS &rhs)
{
	return ReflectTestCheckEqualCompare(reflect::string::ConstString(lhs), rhs);
}

// Section: Macros
// Helper macros for generating and using TEST types.

// Macro: ASSOCIATE
// associates a type with a test, used to tell the test framework which
// types have been tested.
#define ASSOCIATE(TYPE) AssociateType(::reflect::TypeOf<TYPE>())

// Macro: CHECK
// checks if an expression is true.  if it isn't the test is marked as failed and
// a an error message is printed.
#define CHECK(expr) Check((expr), #expr, __FILE__, __LINE__)

// Macro: CHECK_EQUAL
// A specialized check for comparing values, may give slightly more 
// interesting output compared to <CHECK>.
#define CHECK_EQUAL(expected, what) CheckEqual(::reflect::test::ReflectTestCheckEqualCompare(expected, what), #expected, #what,  \
          ::reflect::Variant::FromConstRef(expected), reflect::Variant::FromConstRef(what), __FILE__, __LINE__)

// Macro: TEST
// Defines a new TEST,
// 
// Usage:
// (begin code)
// TEST(Stuff)
// {
//     CHECK(5 < strlen("Hello World"));
//     CHECK_EQUAL(32, atoi("32"));
// }
// (end code)
#define TEST(name__) MAKE_TEST(name__, ::reflect::test::EmptyFixture)
#define FIXTURE(name__, fixture__) MAKE_TEST(name__, fixture__)

} }

#endif
