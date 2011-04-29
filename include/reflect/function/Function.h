#ifndef REFLECT_FUNCTION_FUNCTION_H_
#define REFLECT_FUNCTION_FUNCTION_H_

#include <reflect/Variant.h>
#include <reflect/string/Fragment.h>
#include <reflect/ObjectType.h>

namespace reflect { namespace function {

class Parameters;

// Class: Function
//
// A function object provides a virtual inteface to a function.
// Function uses <Variant> for <Parameters> and "this" pointer passing.
//
// Usage:
// (begin code)
// if(reflect::function::Function *function = dynamic->GetClass()->FindFunction("some_function"))
// {
//     reflect::function::Parameters params(function);
//     params.arg(4).arg("text");
//     Variant result;
//
//     if(function->Call(dynamic, params, result))
//         printf("some_function returned: %s\n", result.ToString().c_str());
// }
//
// (end code)
//
// Functions are implemented by <FunctionImpl>.
class ReflectExport(reflect) Function
{
public:
	// Function: Call (method, by variant)
	// Calls this function with a variant object as self.
	// 
	// Returns:
	//   true - if the call succeeded
	//   false - if the parameters did not match
	//           or the result could not be written.
	bool Call(const Variant &object, const Parameters &params, Variant &result = Variant::Void()) const;

	// Function: Call (function)
	// Calls a non-method function without a "this" pointer.
	// 
	// Returns:
	//   true - if the call succeeded
	//   false - if the parameters did not match
	//           or the result could not be written.
	bool Call(const Parameters &params, Variant &result = Variant::Void()) const;

	// Function: Call (method, by template param)
	// Calls a method with a statically typed "this" pointer.
	//
	// Returns:
	//   true - if the call succeeded
	//   false - if the parameters did not match
	//           or the result could not be written.
	template<typename T>
	bool Call(T *object, const Parameters &params, Variant &result = Variant::Void()) const
	{
		return Call(Variant::Ref(*object), params, result);
	}

	// Function: Call (const method, by template param)
	// Calls a method with a statically typed, but const, "this" pointer.
	//
	// Returns:
	//   true - if the call succeeded
	//   false - if the parameters did not match
	//           or the result could not be written.
	template<typename T>
	bool Call(const T *object, const Parameters &params, Variant &result = Variant::Void()) const
	{
		return Call(Variant::ConstRef(*object), params, result);
	}

	// Function: NumParameters
	// The number of parameters passed to this function.
	int NumParameters() const { return mParameterCount; }

	// Function: ClassifyParameter
	// Returns the type of parameter at *index*, and changes *is_mutable* to true
	// if the parameter is passed as a non-const reference.
	const Type *ClassifyParameter(int index, bool &is_mutable) const { return (*mParameterTypeFun)(index, &is_mutable); }

	// Function: ParameterType
	// Returns the type of parameter at *index*.
	const Type *ParameterType(int index) const { return (*mParameterTypeFun)(index, 0); }

	const Type *ResultType() const { return mResultType; }

	// Function: Name
	const char *Name() const;

	// Function: IsMethod
	// Checks if this function is a member function,
	// and needs a "this" pointer to be called.
	bool IsMethod() const;
	
	// Function: IsConstMethod
	// Checks if this function can be called with a const
	// "this" pointer.
	virtual bool IsConstMethod() const = 0;
	
	// Function: CallObjectType
	// Returns the type of "this" pointer needed.
	const Type *CallObjectType() const { return mObjectType; }
	
protected:
	typedef const Type *(*ParameterTypeFun)(int index, bool *is_mutable);

	// Constructor: Function (protected)
	Function(const char *name, const Type *object_type, ParameterTypeFun arg_type_fun, const Type *result_type);

	// Function: CallInternal (protected)
	virtual bool CallInternal(const Variant &self, Variant *args, Variant &result) const = 0;

private:
	const char *mName;
	const Type *const mObjectType;
	ParameterTypeFun mParameterTypeFun;
	const Type *const mResultType;
	int mParameterCount;
};

// Class: Parameters
// Represents a parameter list passed to a <Function>.
class ReflectExport(reflect) Parameters
{
public:
	// Constructor: Parameters
	// Builds a parameter list for a function.
	Parameters(const Function *fun);

	~Parameters();
	
	// Function: Params
	// Returns the parameters as an array of <Variant>.
	Variant *Params() const;
	
	Variant &operator[](int index) const;
	
	// Function: ValidForFunction
	// Checks if this parameter list is valid to call a particular
	// function.  The types must match exactly and the mutable parameters
	// must cover the mutable parameters needed by the function call.
	bool ValidForFunction(const Function *function) const;

	class ReflectExport(reflect) FunctionParameter;

	// Function: arg
	// Assign (non-mutable) values to parameter.
	// This method is chainable with itself and <ref>.
	//
	// Usage:
	// > function::Parameters params(func);
	// > params.arg(3).arg("text");
	template<typename T>
	FunctionParameter value(const T &val, int index = 0);
	
	// Function: ref
	// Assign (mutable) params (held in a variant) to parameter, using a chainable method.
	// The variant's lifetime (and its reference) must scope any
	// calls of this parameter list.
	//
	// Usage:
	// > reflect::function::Parameters params(func);
	// > int i = 3;
	// > reflect::Variant mutable = Variant::Ref(i);
	// > params.ref(mutable).arg("text");
	FunctionParameter ref(Variant &value, int index = 0);
	FunctionParameter const_ref(const Variant &value, int index = 0);

	// Function: read
	// Deserialize text using a StandardSerializer.
	FunctionParameter read(const string::Fragment &text, int index = 0);
private:
	// no copying
	const Parameters &operator =(const Parameters &);
	Parameters(const Parameters &);
	
	int mCount;
	Variant *mParams;
};

class Parameters::FunctionParameter
{
public:
	template<typename T>
	FunctionParameter value(const T &v) const;

	FunctionParameter ref(Variant &v) const;
	FunctionParameter const_ref(const Variant &v) const;
	
	FunctionParameter read(const string::Fragment &text) const;
private:
	friend class Parameters;
	
	FunctionParameter(Parameters &rs, int idx = 0);
	
	const int mArgumentIndex;
	Parameters &mParameters;
};

template<typename T>
inline
Parameters::FunctionParameter Parameters::value(const T &val, int index)
{
	return FunctionParameter(*this, index).value(val);
}

inline
Variant *Parameters::Params() const
{
	return mParams;
}

inline
Parameters::FunctionParameter Parameters::ref(Variant &value, int index)
{
	return FunctionParameter(*this, index).ref(value);
}

inline
Parameters::FunctionParameter Parameters::const_ref(const Variant &value, int index)
{
	return FunctionParameter(*this, index).const_ref(value);
}


inline
Variant &Parameters::operator[](int index) const
{
	return Params()[index];
}


inline
Parameters::FunctionParameter Parameters::read(const string::Fragment &text, int index)
{
	return FunctionParameter(*this, index).read(text);
}


template<typename T>
inline 
Parameters::FunctionParameter Parameters::FunctionParameter::value(const T &value) const
{
	mParameters.Params()[mArgumentIndex].SetValue(value);
	return FunctionParameter(mParameters, mArgumentIndex+1);
}

} }

DECLARE_STATIC_REFLECTION(reflect, reflect::function::Function, reflect::ObjectType)

#endif
