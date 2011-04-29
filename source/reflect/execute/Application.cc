#include <reflect/execute/Application.h>
#include <reflect/execute/ApplicationClass.hpp>

DEFINE_REFLECTION(reflect::execute::Application, "reflect::execute::Application")
{
}

int reflect::execute::Application::Execute(int argc, char *argv[])
{
	return Run(argc, argv);
}
