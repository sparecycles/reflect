#ifndef REFLECT_EXECUTE_APPLICATION_H_
#define REFLECT_EXECUTE_APPLICATION_H_

#include <reflect/execute/ApplicationClass.h>
#include <reflect/Dynamic.h>
#include <reflect/Reflection.h>
#include <reflect/config/config.h>

namespace reflect { namespace execute {

// Class: Application
//   Base class of "runnable" classes.
//
// See Also:
//   - <ApplicationClass>
//   - <ApplicationClass::DescriptionHelper>
class ReflectExport(reflect) Application : public Dynamic
{
	DECLARE_REFLECTION_EX(Dynamic, ApplicationClass)
public:
	// Function: Execute
	//   invokes Run
	int Execute(int argc, char *argv[]);
protected:
	// Function: Run
	virtual int Run(int argc, char *argv[]) = 0;
};

} }

#endif
