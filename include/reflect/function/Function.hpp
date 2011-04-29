#ifndef REFLECT_FUNCTION_FUNCTION_HPP_
#define REFLECT_FUNCTION_FUNCTION_HPP_

#include <reflect/function/Function.h>
#include <reflect/function/FunctionSignature.hpp>

namespace reflect { namespace function {

// note the use of ResultType as a default template parameter,
// this allows this class to auto-specialize itself for void result-type
// (done below).
template<typename FunctionType,
	typename ResultType = typename FunctionSignature<FunctionType>::TheResultType>
struct FunctionImpl
    : public Function
{
    FunctionImpl(const char *name, FunctionType &function)
        : Function(
			name,
            FunctionSignature<FunctionType>::GetClassType(),
            FunctionSignature<FunctionType>::GetParameterType,
            FunctionSignature<FunctionType>::GetResultType())
		, mFunction(function)
    {}
	
	bool IsConstMethod() const
	{
		return IsMethod() && FunctionSignature<FunctionType>::ConstMemberFunction;
	}

	// virtual
    bool CallInternal(const Variant &self, Variant *args, Variant &result) const
    {
		if(FunctionSignature<FunctionType>::MemberFunction)
		{
			if(FunctionSignature<FunctionType>::ConstMemberFunction)
			{
				if(!self.CanConstRefAs(FunctionSignature<FunctionType>::GetClassType()))
					return false;
				
				// const_casted value is used const just inside the Call method,
				// the only reason we const cast is to have a common static signature for
				// the Call method.  We could try to do some fancy template_if
				// redirection to get rid of the const_cast, but why bother?
				return result.SetValue(FunctionSignature<FunctionType>
					::Call(mFunction, self, args));
			}
			else
			{
				if(!self.CanRefAs(FunctionSignature<FunctionType>::GetClassType()))
					return false;
					
				return result.SetValue(FunctionSignature<FunctionType>
					::Call(mFunction, self, args));
			}
		}
		else
		{
			return result.SetValue(FunctionSignature<FunctionType>::Call(mFunction, self, args));
		}
    }

private:
    FunctionType mFunction;
};

// Specialization for void result-type, automatically selected by
// the original template definition above.
template<typename FunctionType>
struct FunctionImpl<FunctionType, void>
    : public Function
{
    FunctionImpl(const char *name, FunctionType &function)
        : Function(name,
             FunctionSignature<FunctionType>::GetClassType(),
             FunctionSignature<FunctionType>::GetParameterType,
             FunctionSignature<FunctionType>::GetResultType()
            )
		, mFunction(function)
    {}
	
	bool IsConstMethod() const
	{
		return IsMethod() && FunctionSignature<FunctionType>::ConstMemberFunction;
	}
	
	// virtual
    bool CallInternal(const Variant &self, Variant *args, Variant &result) const
    {
		if(FunctionSignature<FunctionType>::MemberFunction)
		{
			if(FunctionSignature<FunctionType>::ConstMemberFunction)
			{
				if(!self.CanConstRefAs(FunctionSignature<FunctionType>::GetClassType()))
					return false;
					
				// const_casted value is used const just inside the Call method,
				// the only reason we const cast is to have a common static signature for
				// the Call method.  We could try to do some fancy template_if
				// redirection to get rid of the const_cast, but why bother?
				FunctionSignature<FunctionType>
					::Call(mFunction, self, args);
			}
			else
			{
				if(!self.CanRefAs(FunctionSignature<FunctionType>::GetClassType()))
					return false;
				
				FunctionSignature<FunctionType>
					::Call(mFunction, self, args);
			}
		}
		else
		{
			FunctionSignature<FunctionType>::Call(mFunction, self, args);
		}
		
		result.BindType<void>();
		return true;
    }
    
private:
    FunctionType mFunction;
};

// Function: CreateFunction
// Template method which generates a generic <Function> object
// from a native function pointer.
template<typename FunctionType>
inline const function::Function *CreateFunction(const char *name, FunctionType func)
{
	return new function::FunctionImpl<FunctionType>(name, func);
}

} }

#endif
