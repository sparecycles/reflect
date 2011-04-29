#ifndef REFLECT_CASTABLE_H_
#define REFLECT_CASTABLE_H_

#include <reflect/config/config.h>
#include <reflect/Reflection.h>

namespace reflect { namespace function { class Function; } }
namespace reflect { namespace string { class Fragment; class SharedString; } }

namespace reflect {

class Class;

// Class: Dynamic
//
// This class defines the core interface for all
// runtime type identification (RTTI) in reflect.
//
// The two facets of building an RTTI system are 
//
//   - Building a class hierarchy, and
//   - Accessing the class hierarchy.
//
// In reflect, the class hierarchy is partially constructed using static initialization,
// and finalized using an explicit call (<Class.LoadClasses>) to avoid the pitfalls
// of static initialization order.
//
// The static initialization is facilitated by the method <Dynamic::TheClass>, and a macro in <Reflection.hpp>.
//
// Accessing the hierarchy is done through the virtual function <GetClass>,
// which always returns the result of TheClass() but is accessed through the Dynamic's virtual table.
//
// In order to keep the particular type of Class consistent, Dynamics also typedef the ClassType
// so that subclasses can inherit it.

class ReflectExport(reflect) Dynamic
{
public:
	// Typedef: ClassType
	//   The type of Class which reflects the castable.
	//
	// Note:
	//  This typedef can be hidden by subclasses to represent a change in the class type.
    typedef Class ClassType;

	// Function: GetClass
	//
	// Returns:
	//   The dynamic <Class> of the castable.
    virtual ClassType *GetClass() const;

	// Function: TheClass
	// This function is always hidden by subclasses and used during
	// static construction to construct the class hierarchy.
	//
	// Returns:
	//   The static class of the castable.
    static ClassType *TheClass();

	// Destructor: ~Dynamic
	//   Declare virtual to allow correct deletion of castable.
	virtual ~Dynamic();

	// Function: Function(name)
	//   Returns a <Function> which represents the named function.
	const function::Function *Function(string::Fragment function_name) const;

protected:
	// Constructor: Dynamic
	//   Declared protected because there's no good reason to make a plain Dynamic.
    Dynamic();
};

}

#endif
