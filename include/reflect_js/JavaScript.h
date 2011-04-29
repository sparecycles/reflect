// File: JavaScript.h
//
// Bindings for reflect to 
// Mozilla's Spidermonkey (v1.7.0).
//
// This provides a basic ECMA script runtime with
// the following runtime support
//
//   print() - A basic print method.
//   GC()  - Run the garbage collector.
//   Console() - Runs a very basic Javascript console.
//   Native - An object which provides access to the reflected types by namespace.
//   

#ifndef REFLECT_JS_JAVASCRIPT_H_
#define REFLECT_JS_JAVASCRIPT_H_

#include <reflect_js/config/config.h>
#include <reflect/Reflection.h>
#include <reflect/Dynamic.h>
#include <reflect/Variant.h>
#include <reflect/string/Fragment.h>
#include <reflect/string/ConstString.h>
#include <reflect/utility/Shared.hpp>
#include <vector>

extern "C" {
	struct JSRuntime;
	struct JSContext;
	struct JSObject;
}

namespace reflect { namespace function {
class Function;
template<typename FunctionType>
const Function *CreateFunction(const char *, FunctionType);
} }

namespace reflect { namespace js {

// Class: JavaScriptRuntime
// Wraps a JSRuntime, a JSRuntime object represents
// a single JavaScript object space which can hold
// multiple contexts.  JavaScript objects in different
// <JavaScriptContexts> but within the same runtime
// may be shared.
//
// A <SharedRuntime> is automatically used by default, 
// client code only needs to instantate a runtime object
// if they want to have completely seperate runtimes.
class ReflectExport(reflect_js) JavaScriptRuntime
	: public utility::Shared<JavaScriptRuntime>
{
public:
	JavaScriptRuntime();
	JavaScriptRuntime(const JavaScriptRuntime &);
	~JavaScriptRuntime();
	
	// Function: SharedRuntime
	static JavaScriptRuntime SharedRuntime();

	// Function: GetRuntime
	JSRuntime *GetRuntime() const { return mRuntime; }

private:
	JavaScriptRuntime(JSRuntime *);
	static JavaScriptRuntime sSharedRuntime;

	JSRuntime *mRuntime;
};

// Class: JavaScriptContext
//
// Represents a javascript context
class ReflectExport(reflect_js) JavaScriptContext
{
public:
	// Constructor: JavaScriptContext
	JavaScriptContext(const JavaScriptRuntime & = JavaScriptRuntime::SharedRuntime());
	
	~JavaScriptContext();
	
	// Function: GC
	// Runs the garbage collector.
	void GC() const;
	
	// Function: RegisterFunction
	// Registers a function in this context.
	//
	// Parameters:
	//   name - the javascript name of this function
	//   function - the function to use
	//   take_ownership - if true, delete the *function* when destructing this context.
	//
	// See Also:
	//   - <RegisterFunction (template)> preferred way of the registering functions.
	bool RegisterFunction(string::ConstString name, const function::Function *function, bool take_ownership = false);
	
	// Function: RegisterFunction (template)
	// 
	// Like <RegisterFunction>, but takes a regular C function pointer, 
	// creates the function::Function object wrapper and marks it for deletion.
	template<typename FunctionType>
	bool RegisterFunction(string::ConstString name, FunctionType function)
	{
		return RegisterFunction(name, function::CreateFunction<FunctionType>(name.c_str(), function), true);
	}

	// Function: Eval (toss result)
	bool Eval(string::Fragment script) const;

	// Function: Eval (typed result)
	template<typename T>
	bool Eval(string::Fragment script, T &result) const
	{
		Variant var_result = Variant::FromRef(result);
		return Eval(script, var_result);
	}

	// Function: Eval
	// Assigns the result of Eval into the variant *result*.
	// 
	// Returns:
	//   true - if the evaluation and assignment executed successfully
	//   false - if the evaluation threw an exception or assignment failed.
	bool Eval(string::Fragment script, Variant &result) const;

	// Function: IncRef
	// Increments the reference count on a pointer that the java runtime knows about.
	// When java finalizes the assoicated object, the pointer will not be deleted
	// if it has a positive reference count.  The default reference count on an object
	// is one, functions that allocate new objects for javascript should <DecRef>
	// the reference count.  Note that an <IncRef>-ed object will never be deleted by
	// the object system if it never makes it to the java runtime.
	template<typename T>
	void IncRef(T *object) const { IncOpaqueRef(opaque_cast(object)); }
	
	// Function: DecRef
	template<typename T>
	void DecRef(T *object) const { DecOpaqueRef(opaque_cast(object)); }

	void IncOpaqueRef(void *opaque) const;
	void DecOpaqueRef(void *opaque) const;

	// Function: ThrowError
	// Throws an error from a function
	// to the JavaScript runtime.
	void ThrowError(const char *format, ...) const;

	// Function: GetJSContext
	// Returns the native context pointer.
	JSContext *GetJSContext() const;

	// for operating from within C++ methods invoked from javascript
	
	// Function: CallContext
	// Retrieves the current context from within a javascript call.
	static const JavaScriptContext *CallContext();
	
	// Function: CallArgumentCount
	// Retrieves the number of arguments actually passed to the native function.
	static int CallArgumentCount();	
	
	// Function: CallArgument
	// Retrieves the argument *index* passed to the native function
	// through a <Variant>.
	static bool CallArgument(int index, Variant &variant);
	
	// Function: CallArgumentText
	// Retrieves the argument *index* as text (converted by javascript).
	// Should be the same as Eval("String(arguments[index])")
	static const char *CallArgumentText(int index);

	// Function: CallArgumentValue
	// Retrieves the argument *index* as whatever (converted by <Variant>).
	template<typename T>
	static bool CallArgumentValue(int index, T &value)
	{
		return CallArgument(index, Variant::Ref(value));
	}

	// Function: CallReturn
	// Sets the return value of a void native function by variant.
	static bool CallReturn(Variant &value);
	
	// Function: CallReturnValue
	// Sets the return value of a void native function by value.
	template<typename T>
	static bool CallReturnValue(const T &value)
	{
		return CallReturn(Variant::ConstRef(value));
	}

private:
	JavaScriptRuntime mRuntime;
	JSContext *mContext;
	std::vector<const function::Function *> mRegisteredFunctions;
};

} }

#endif
