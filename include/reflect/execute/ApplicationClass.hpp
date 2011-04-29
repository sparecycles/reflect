#ifndef REFLECT_EXECUTE_APPLICATIONCLASS_HPP_
#define REFLECT_EXECUTE_APPLICATIONCLASS_HPP_

#include <reflect/execute/Application.h>
#include <reflect/execute/ApplicationClass.h>
#include <reflect/Class.hpp>

namespace reflect { namespace execute {

// Class: ApplicationClass::DescriptionHelper
//   Helper class for <Application> reflection definitions.
//
// Features:
//    Executable - Add this feature to reflect a method for running this application type. 
//
// See Also:
//    - <Class::DescriptionHelper>
//    - <Class>
//    - <DEFINE_REFLECTION>
template<typename ApplicationType>
struct ApplicationClass::DescriptionHelper : public Class::DescriptionHelper<ApplicationType>
{
	struct 
	{
		void operator +() const
		{
			struct Function
			{
				static int Execute(int argc, char *argv[])
				{
					ApplicationType application;
					return application.Execute(argc, argv);
				}
			};

			TypeOf<ApplicationType>()->SetExecutionFunction(&Function::Execute);
		}
	} Executable;
};

} }

#define DEFINE_APPLICATION(name_) \
  namespace { \
	class REFLECT_UNIQUENAME(Application) : public reflect::execute::Application \
	{ \
		DECLARE_REFLECTION(reflect::execute::Application) \
		int Run(int argc, char *argv[]); \
	}; \
	\
	DEFINE_REFLECTION(REFLECT_UNIQUENAME(Application), name_) { + Executable; } \
	\
  } \
  int REFLECT_UNIQUENAME(Application)::Run(int argc, char *argv[])

#endif
