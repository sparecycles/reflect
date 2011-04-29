#include <reflect/execute/ApplicationClass.hpp>
#include <reflect/Class.hpp>

DEFINE_REFLECTION(
	reflect::execute::ApplicationClass,
	"reflect::execute::ApplicationClass")
{
}

namespace reflect { namespace execute {

ApplicationClass::ApplicationClass(void (*init_cb)())
	: Class(init_cb)
{
}

void ApplicationClass::SetExecutionFunction(int (*function)(int, char *[]))
{
	mExecutionFunction = function;
}

extern "C" ReflectExport(reflect) void atexec() {}

int ApplicationClass::Execute(int argc, char *argv[])
{
	atexec();
	
	return (*mExecutionFunction)(argc, argv);
}

} }
