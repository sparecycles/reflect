#ifndef REFLECT_FUNCTION_FUNCTIONSIGNATURE_HPP_
#define REFLECT_FUNCTION_FUNCTIONSIGNATURE_HPP_

#include <reflect/Variant.h>

namespace reflect { namespace function {

// File: FunctionSignature.hpp
// Generated template bindings for reflecting functions.
//
// This is the first stage in creating a run-time interface for functions,
// it takes the inconsistent compile-time interface of function calls, and 
// makes it into a more consistent (if awkward) templated compile-time interface.
//
// The second stage is in reflect/function/Function.hpp which
// takes the template interface and wraps it in virtual functions.
//
// The third stage is in reflect/function/Function.{h,cpp} which defines the
// run-time interface and provides some wrappers.

// Class: VariantAliasRef
// The VariantAliasRef automates
// flushing modified by-reference parameters
// back into their source values.
//
//    * When it destructs, it updates the values of any <Variant::Alias>-ed variants, and
//    * You can view its value in a debugger. (optional)
template<typename T>
class VariantAliasRef
{
public:
	VariantAliasRef(Variant &var)
		: mVar(var)
#ifndef NO_DEBUG_FUNCTION_ALIAS
		, mValue(var.AsRef<T>())
#endif
	{}
	
	operator T &() const
	{ 
#ifndef NO_DEBUG_FUNCTION_ALIAS
		return mValue;
#else
		return var.AsRef<T>();
#endif
	}

	~VariantAliasRef()
	{
		mVar.UpdateAliases();
	}
private:
	Variant &mVar;
#ifndef NO_DEBUG_FUNCTION_ALIAS
	T &mValue;
#endif
};

template<typename Type_>
struct ParameterHelper
{
  typedef Type_ Type;
  typedef const Type &ArgType;
  static inline const Type &Arg(Variant &variant) { return variant.AsConstRef<Type>(); }
  enum { MutableParameter = false };
};

template<typename Type_>
struct ParameterHelper<const Type_>
{
  typedef Type_ Type; 
  typedef const Type &ArgType;
  static inline const Type &Arg(Variant &variant) { return variant.AsConstRef<Type>(); }
  enum { MutableParameter = false };
};

template<typename Type_>
struct ParameterHelper<Type_&>
{
  typedef Type_ Type;
  typedef VariantAliasRef<Type> ArgType;
  static inline Variant &Arg(Variant &variant) { return variant; }
  enum { MutableParameter = true };
};

template<typename Type_>
struct ParameterHelper<const Type_ &>
{
  typedef Type_ Type;
  typedef const Type &ArgType;
  static inline const Type &Arg(Variant &variant) { return variant.AsConstRef<Type>(); }
  enum { MutableParameter = false };
};

template<>
struct ParameterHelper<void>
{
  typedef void Type;
  static inline const void Arg(Variant &) { return; }
};

template<typename FunctionType> class FunctionSignature;
template<typename ResultType>
class FunctionSignature<ResultType (*)()>
{
	typedef ResultType (*FunctionType)();
public:
	enum
	{ 
	    ParameterCount = 0,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};


	static const Type *GetParameterType(int, bool *)
	{
		return 0;
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *)
	{		
		return (*function)();
	}
};

template<typename ResultType, typename A0>
class FunctionSignature<ResultType (*)(A0)>
{
	typedef FunctionSignature<void (*)()> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0);
public:
	enum
	{ 
	    ParameterCount = 1,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;

	typedef P0 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);		
		return (*function)(arg0);
	}
};

template<typename ResultType, typename A0, typename A1>
class FunctionSignature<ResultType (*)(A0, A1)>
{
	typedef FunctionSignature<void (*)(A0)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1);
public:
	enum
	{ 
	    ParameterCount = 2,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;

	typedef P1 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);		
		return (*function)(arg0, arg1);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2>
class FunctionSignature<ResultType (*)(A0, A1, A2)>
{
	typedef FunctionSignature<void (*)(A0, A1)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2);
public:
	enum
	{ 
	    ParameterCount = 3,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;

	typedef P2 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);		
		return (*function)(arg0, arg1, arg2);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3);
public:
	enum
	{ 
	    ParameterCount = 4,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;

	typedef P3 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);		
		return (*function)(arg0, arg1, arg2, arg3);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4);
public:
	enum
	{ 
	    ParameterCount = 5,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;

	typedef P4 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5);
public:
	enum
	{ 
	    ParameterCount = 6,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;

	typedef P5 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6);
public:
	enum
	{ 
	    ParameterCount = 7,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;

	typedef P6 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7);
public:
	enum
	{ 
	    ParameterCount = 8,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;

	typedef P7 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8);
public:
	enum
	{ 
	    ParameterCount = 9,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;

	typedef P8 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9);
public:
	enum
	{ 
	    ParameterCount = 10,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;

	typedef P9 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
public:
	enum
	{ 
	    ParameterCount = 11,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;

	typedef P10 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
public:
	enum
	{ 
	    ParameterCount = 12,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;

	typedef P11 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
public:
	enum
	{ 
	    ParameterCount = 13,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;

	typedef P12 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
public:
	enum
	{ 
	    ParameterCount = 14,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;

	typedef P13 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
	}
};

template<typename ResultType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class FunctionSignature<ResultType (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> BaseFunctionSignature;
	typedef ResultType (*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
public:
	enum
	{ 
	    ParameterCount = 15,
	    MemberFunction = false,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;
	typedef ParameterHelper<A14> P14;

	typedef P14 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef void TheClassType;
	static const Type *GetClassType() { return 0; }
	static ResultType Call(const FunctionType &function, const Variant &, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);
		typename P14::ArgType arg14 = P14::Arg(args[14]);		
		return (*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
	}
};

template<typename ResultType, typename ClassType>
class FunctionSignature<ResultType (ClassType::*)()>
{
	typedef ResultType (ClassType::*FunctionType)();
public:
	enum
	{ 
	    ParameterCount = 0,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};


	static const Type *GetParameterType(int, bool *)
	{
		return 0;
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *)
	{		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)();
	}
};

template<typename ResultType, typename ClassType, typename A0>
class FunctionSignature<ResultType (ClassType::*)(A0)>
{
	typedef FunctionSignature<void (*)()> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0);
public:
	enum
	{ 
	    ParameterCount = 1,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;

	typedef P0 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1>
class FunctionSignature<ResultType (ClassType::*)(A0, A1)>
{
	typedef FunctionSignature<void (*)(A0)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1);
public:
	enum
	{ 
	    ParameterCount = 2,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;

	typedef P1 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2)>
{
	typedef FunctionSignature<void (*)(A0, A1)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2);
public:
	enum
	{ 
	    ParameterCount = 3,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;

	typedef P2 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3);
public:
	enum
	{ 
	    ParameterCount = 4,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;

	typedef P3 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4);
public:
	enum
	{ 
	    ParameterCount = 5,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;

	typedef P4 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5);
public:
	enum
	{ 
	    ParameterCount = 6,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;

	typedef P5 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6);
public:
	enum
	{ 
	    ParameterCount = 7,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;

	typedef P6 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7);
public:
	enum
	{ 
	    ParameterCount = 8,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;

	typedef P7 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8);
public:
	enum
	{ 
	    ParameterCount = 9,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;

	typedef P8 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9);
public:
	enum
	{ 
	    ParameterCount = 10,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;

	typedef P9 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
public:
	enum
	{ 
	    ParameterCount = 11,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;

	typedef P10 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
public:
	enum
	{ 
	    ParameterCount = 12,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;

	typedef P11 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
public:
	enum
	{ 
	    ParameterCount = 13,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;

	typedef P12 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
public:
	enum
	{ 
	    ParameterCount = 14,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;

	typedef P13 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
public:
	enum
	{ 
	    ParameterCount = 15,
	    MemberFunction = true,
	    ConstMemberFunction = false
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;
	typedef ParameterHelper<A14> P14;

	typedef P14 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);
		typename P14::ArgType arg14 = P14::Arg(args[14]);		
		ClassType &self = object.AsRef<ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
	}
};

template<typename ResultType, typename ClassType>
class FunctionSignature<ResultType (ClassType::*)() const>
{
	typedef ResultType (ClassType::*FunctionType)() const;
public:
	enum
	{ 
	    ParameterCount = 0,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};


	static const Type *GetParameterType(int, bool *)
	{
		return 0;
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *)
	{		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)();
	}
};

template<typename ResultType, typename ClassType, typename A0>
class FunctionSignature<ResultType (ClassType::*)(A0) const>
{
	typedef FunctionSignature<void (*)()> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0) const;
public:
	enum
	{ 
	    ParameterCount = 1,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;

	typedef P0 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1>
class FunctionSignature<ResultType (ClassType::*)(A0, A1) const>
{
	typedef FunctionSignature<void (*)(A0)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1) const;
public:
	enum
	{ 
	    ParameterCount = 2,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;

	typedef P1 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2) const>
{
	typedef FunctionSignature<void (*)(A0, A1)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2) const;
public:
	enum
	{ 
	    ParameterCount = 3,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;

	typedef P2 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3) const;
public:
	enum
	{ 
	    ParameterCount = 4,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;

	typedef P3 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4) const;
public:
	enum
	{ 
	    ParameterCount = 5,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;

	typedef P4 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5) const;
public:
	enum
	{ 
	    ParameterCount = 6,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;

	typedef P5 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6) const;
public:
	enum
	{ 
	    ParameterCount = 7,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;

	typedef P6 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7) const;
public:
	enum
	{ 
	    ParameterCount = 8,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;

	typedef P7 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8) const;
public:
	enum
	{ 
	    ParameterCount = 9,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;

	typedef P8 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
public:
	enum
	{ 
	    ParameterCount = 10,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;

	typedef P9 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const;
public:
	enum
	{ 
	    ParameterCount = 11,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;

	typedef P10 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const;
public:
	enum
	{ 
	    ParameterCount = 12,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;

	typedef P11 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const;
public:
	enum
	{ 
	    ParameterCount = 13,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;

	typedef P12 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const;
public:
	enum
	{ 
	    ParameterCount = 14,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;

	typedef P13 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13);
	}
};

template<typename ResultType, typename ClassType, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class FunctionSignature<ResultType (ClassType::*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const>
{
	typedef FunctionSignature<void (*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> BaseFunctionSignature;
	typedef ResultType (ClassType::*FunctionType)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const;
public:
	enum
	{ 
	    ParameterCount = 15,
	    MemberFunction = true,
	    ConstMemberFunction = true
	};

	typedef ParameterHelper<A0> P0;
	typedef ParameterHelper<A1> P1;
	typedef ParameterHelper<A2> P2;
	typedef ParameterHelper<A3> P3;
	typedef ParameterHelper<A4> P4;
	typedef ParameterHelper<A5> P5;
	typedef ParameterHelper<A6> P6;
	typedef ParameterHelper<A7> P7;
	typedef ParameterHelper<A8> P8;
	typedef ParameterHelper<A9> P9;
	typedef ParameterHelper<A10> P10;
	typedef ParameterHelper<A11> P11;
	typedef ParameterHelper<A12> P12;
	typedef ParameterHelper<A13> P13;
	typedef ParameterHelper<A14> P14;

	typedef P14 LastParamType;

	static const Type *GetParameterType(int index, bool *is_mutable = 0)
	{
		return index == ParameterCount-1
		    ? ((is_mutable ? (void)(*is_mutable = LastParamType::MutableParameter) : (void)0), TypeOf<typename LastParamType::Type>())
		    : BaseFunctionSignature::GetParameterType(index, is_mutable);
	}

	typedef ResultType TheResultType;
	static const Type *GetResultType() { return TypeOf<typename ParameterHelper<ResultType>::Type>(); }
	typedef ClassType TheClassType;
	static const Type *GetClassType() { return TypeOf<typename ParameterHelper<ClassType>::Type>(); }
	static ResultType Call(const FunctionType &function, const Variant &object, Variant *args)
	{
		typename P0::ArgType arg0 = P0::Arg(args[0]);
		typename P1::ArgType arg1 = P1::Arg(args[1]);
		typename P2::ArgType arg2 = P2::Arg(args[2]);
		typename P3::ArgType arg3 = P3::Arg(args[3]);
		typename P4::ArgType arg4 = P4::Arg(args[4]);
		typename P5::ArgType arg5 = P5::Arg(args[5]);
		typename P6::ArgType arg6 = P6::Arg(args[6]);
		typename P7::ArgType arg7 = P7::Arg(args[7]);
		typename P8::ArgType arg8 = P8::Arg(args[8]);
		typename P9::ArgType arg9 = P9::Arg(args[9]);
		typename P10::ArgType arg10 = P10::Arg(args[10]);
		typename P11::ArgType arg11 = P11::Arg(args[11]);
		typename P12::ArgType arg12 = P12::Arg(args[12]);
		typename P13::ArgType arg13 = P13::Arg(args[13]);
		typename P14::ArgType arg14 = P14::Arg(args[14]);		
		const ClassType &self = object.AsConstRef<const ClassType>();
		return (self.*function)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14);
	}
};

} }

#endif // REFLECT_FUNCTION_FUNCTIONSIGNATURE_HPP_
