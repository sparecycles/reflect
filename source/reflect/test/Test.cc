#include <reflect/test/Test.h>
#include <reflect/Class.hpp>
#include <reflect/utility/Context.hpp>
#include <reflect/string/String.h>
#include <reflect/execute/Application.h>
#include <reflect/execute/ApplicationClass.hpp>
#include <list>
#include <set>
#include <cstdio>

namespace reflect { namespace test {

class TestContext;

class ReflectExport(reflect) TestRecord : public utility::Context<TestRecord>
{
public:
	TestRecord(bool v = false)
		: test_count(0)
		, verbose(v)
	{}

	struct TypeCounts { 
		TypeCounts() : failures(0), successes(0) {}
		int failures;
		int successes;
	};
	typedef std::map<const Type *, TypeCounts> TypeCountMap;
	TypeCountMap by_type;
	std::list<const TestClass *> failures;
	int test_count;
	bool verbose;

	static void StartTestContext();
	static void CommitTestContext();

	void PrintResults();
};

class ReflectExport(reflect) TestContext : public utility::Context<TestContext>
{
public:
	typedef std::list<const Type *> TypeList;

	TestContext(const TestClass *test) 
		: mTest(test) 
		, mChecks(0)
		, mChecksPassed(0)
	{
		TestRecord::StartTestContext();
	}

	void ReportResult(bool result)
	{
		mChecks++;
		if(result) mChecksPassed++;
	}

	void Complain(const char *format, ...)
    {
		va_list args;

		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		fprintf(stderr, "\n");
    }

	const char *TestName() const { return mTest->Name(); }
	const TestClass *Test() const { return mTest; }

	bool Success() const { return mChecksPassed == mChecks; }

	void AssociateType(const Type *type)
	{
		mTypes.push_back(type);
    }

	const TypeList &AssociatedTypes() const { return mTypes; }

	~TestContext()
	{
		TestRecord::CommitTestContext();
    }


private:
	TypeList mTypes;
	const TestClass *const mTest;
	int mChecks;
	int mChecksPassed;
};


void TestRecord::StartTestContext()
{
	if(TestContext *context = TestContext::GetContext())
	{
		if(TestRecord *record = TestRecord::GetContext()) 
		{
			if(record->verbose)
				fprintf(stderr, "-- Running Test: %s in %s\n", context->Test()->Name(), context->Test()->File());
		}
    }
}

void TestRecord::CommitTestContext()
{
	if(TestContext *context = TestContext::GetContext())
	{
		if(TestRecord *record = TestRecord::GetContext()) 
		{
			record->test_count++;

			if(!context->Success())
			{
				record->failures.push_back(context->Test());
			}

			for(TestContext::TypeList::const_iterator it = context->AssociatedTypes().begin(); it != context->AssociatedTypes().end(); ++it)
			{
				if(context->Success()) record->by_type[*it].successes++;
				else record->by_type[*it].failures++;
			}
		}
		else
		{
			if(!context->Success())
				fprintf(stderr, "Test Failed: %s failed\n", context->Test()->Name());
		}
	}
}

void TestRecord::PrintResults()
{
	if(verbose)
	{
		std::set<const Type *> untested_classes;
		
		const Type::TypeMapType &classmap = Type::GlobalTypeMap();

		for(Type::TypeMapType::const_iterator it = classmap.begin(); it != classmap.end(); ++it)
		{
			untested_classes.insert(it->second);
		}

		printf("Test Results By Type:\n");

		for(TypeCountMap::iterator it = by_type.begin(); it != by_type.end(); ++it)
		{
			untested_classes.erase(it->first);
	
#ifndef _WIN32
			if(it->second.failures != 0)
				printf("\033[31m");
#endif
			printf("\t%s : %d of %d tests passed\n", it->first->Name(), 
				it->second.successes, it->second.failures + it->second.successes);
#ifndef _WIN32
			if(it->second.failures != 0)
				printf("\033[0m");
#endif
		}

		if(untested_classes.size())
		{
			int i = 0;
			printf("Untested Types:");

			for(std::set<const Type *>::const_iterator it = untested_classes.begin();
				it != untested_classes.end(); ++it)
			{
				printf(0 == (i++ % 5) ? "\n\t" : ", ");
				printf("%s", (*it)->Name());
			}

			printf("\n");
		}
	}

	printf("%d of %d tests passed\n", test_count - int(failures.size()), test_count);

	if(failures.size() > 0)
	{
		printf("Failures in:");
		int i = 0;

		for(std::list<const TestClass *>::const_iterator it = failures.begin();
		    it != failures.end(); ++it)
		{
			printf(0 == (i++ % 5) ? "\n\t" : ", ");
			printf("%s", (*it)->Name());
		}
		
		printf("\n");
	}
}

void Test::AssociateType(const Type *type)
{
	if(TestContext *context = TestContext::GetContext())
		context->AssociateType(type);
}

void Test::Check(bool result, const char *what, const char *file, int line)
{
	if(TestContext *context = TestContext::GetContext())
	{
		context->ReportResult(result);

		if(result == false)
		{
#ifdef _MSC_VER // change the error format for msvc
			context->Complain("%s(%d) : error: %s: check '%s' failed.",
				 file, line, context->TestName(), what);
#else
			context->Complain("%s:%d: error: %s: check '%s' failed.",
				 file, line, context->TestName(), what);
#endif
		}
	}
}

void Test::CheckEqual(bool result, const char *lhs, const char *rhs, const Variant &value1, const Variant &value2, const char *file, int line)
{
	if(TestContext *context = TestContext::GetContext())
	{
		context->ReportResult(result);

		if(result == false)
#ifdef _WIN32
			context->Complain(
				"%s(%d) : error: %s: check equal failed.\n"
				"\texpected %s=%s, got %s=%s", 
				file, line, context->TestName(), 
				lhs, value1.ToString().c_str(), rhs, value2.ToString().c_str()
			);
#else
			context->Complain(
				"%s:%d: error: %s: check equal failed.\n"
				"\texpected %s=%s, got %s=%s", 
				file, line, context->TestName(), 
				lhs, value1.ToString().c_str(), rhs, value2.ToString().c_str()
			);
#endif
	}
}

TestClass::TestClass(void (*init)()) 
	: Class(init) 
{}

bool TestClass::RunTest() const
{ 
	TestContext context(this);
	(*mTestFunction)(); 
	return context.Success();
}

void TestClass::SetTestFunction(void (*test_fun)())
{ 
	mTestFunction = test_fun;
}

bool TestClass::RunAllTests(bool verbose)
{
	TestRecord record(verbose);

    if(TestClass *test = Test::TheClass()->Child() % autocast) do
    {
        test->RunTest();
    } while(test = test->Sibling() % autocast, test != Test::TheClass()->Child());

	record.PrintResults();

	return record.failures.size() == 0;
}

void TestClass::RegisterName()
{
}

void TestClass::SetTestInfo(const char *filename, int lineno)
{
	mFilename = filename;
	mLineNo = lineno;
}

const char *TestClass::File() const
{
	return mFilename;
}

} }

template class reflect::utility::Context<reflect::test::TestContext>;
template class reflect::utility::Context<reflect::test::TestRecord>;

DEFINE_REFLECTION(reflect::test::TestClass, "reflect::test::TestClass")
{
}

DEFINE_REFLECTION(reflect::test::Test, "reflect::test::Test")
{
}

DEFINE_APPLICATION("reflect_test::RunTests")
{
	bool verbose = false;

	for(int i = 1; i < argc; i++)
	{
		if(reflect::string::ConstString(argv[i]) == "-v" ||
		   reflect::string::ConstString(argv[i]) == "--verbose") verbose = true;
    }

    return reflect::test::TestClass::RunAllTests(verbose) ? 0 : 1;
}
