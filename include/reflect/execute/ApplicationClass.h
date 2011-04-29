#ifndef REFLECT_EXECUTE_APPLICATIONCLASS_H_
#define REFLECT_EXECUTE_APPLICATIONCLASS_H_

#include <reflect/Class.h>
#include <reflect/Reflection.h>
#include <reflect/config/config.h>

namespace reflect { namespace execute {

// Class: ApplicationClass
// 
// A class which reflects "runnable" classes.
//
// See Also:
//   - <Application>
class ReflectExport(reflect) ApplicationClass : public Class
{
	DECLARE_REFLECTION(Class)
public:
    ApplicationClass(void (*init_cb)() = 0);

	// Function: Execute
	int Execute(int argc = 0, char *argv[] = 0);

	// Function: SetExecutionFunction
	void SetExecutionFunction(int (*)(int, char *[]));

	template<typename Application>
	struct DescriptionHelper;	
private:
	int (*mExecutionFunction)(int, char *[]);
};

} }

#endif
