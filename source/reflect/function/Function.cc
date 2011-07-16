#include <reflect/function/Function.h>
#include <reflect/ObjectType.hpp>

DEFINE_STATIC_REFLECTION(reflect::function::Function, "reflect::function::Function")
{
  using namespace reflect::function;
  
  Functions
    ("Name", &Function::Name)
    ("ParameterType", &Function::ParameterType)
    ("ResultType", &Function::ResultType)
    ("NumParameters", &Function::NumParameters)
    ("IsMethod", &Function::IsMethod)
    ("IsConstMethod", &Function::IsConstMethod)
    ("CallObjectType", &Function::CallObjectType)
    ;
}

namespace reflect { namespace function {

const char *Function::Name() const
{
  return mName;
}

bool Function::IsMethod() const
{
  return mObjectType != 0;
}

Function::Function(const char *name, const Type *object_type, ParameterTypeFun arg_type_fun, const Type *result_type)
  : mName(name)
  , mObjectType(object_type)
  , mParameterTypeFun(arg_type_fun)
  , mResultType(result_type)
{
  mParameterCount = 0;

  while((*mParameterTypeFun)(mParameterCount, 0))
    mParameterCount++;
}

bool Function::Call(const Parameters &params, Variant &result) const
{
  if(IsMethod() || !params.ValidForFunction(this)) {
    return false;
  }
  
  return CallInternal(Variant::Void(), params.Params(), result);
}

bool Function::Call(const Variant &self, const Parameters &params, Variant &result) const
{
  if(IsConstMethod() && !self.CanConstRefAs(mObjectType)) {
    fprintf(stderr, "%s: invalid this (expected const %s)", Name(), mObjectType->Name());
    return false;
  }

  if(IsMethod() && !IsConstMethod() && !self.CanRefAs(mObjectType)) {
    fprintf(stderr, "%s: invalid this (expected %s)", Name(), mObjectType->Name());
    return false;
  }

  if(!params.ValidForFunction(this)) {
    fprintf(stderr, "%s: invalid params", Name(), mObjectType->Name());
    return false;
  }

  return CallInternal(self, params.Params(), result);
}

Parameters::Parameters(const Function *fun)
{
  mCount = fun->NumParameters();
  mParams = mCount ? new Variant[mCount] : 0;

  for(int param_idx = 0; param_idx < mCount; param_idx++)
  {
    mParams[param_idx]
      .BindType(fun->ParameterType(param_idx));
  }
}

bool Parameters::ValidForFunction(const Function *function) const
{
  if(mCount < function->NumParameters())
    return false;
  
  for(int i = 0; i < function->NumParameters(); i++)
  {
    bool mut = false;
    const Type *expected_type = function->ClassifyParameter(i, mut);

    if(mut ? !Params()[i].CanRefAs(expected_type) : !Params()[i].CanConstRefAs(expected_type))
    {
      return false;
    }
  }
  
  return true;
}

Parameters::~Parameters()
{
  if(mParams)
    delete [] mParams;
}

Parameters::FunctionParameter::FunctionParameter(Parameters &rs, int idx)
  : mArgumentIndex(idx)
  , mParameters(rs)
{
}

Parameters::FunctionParameter Parameters::FunctionParameter::read(const string::Fragment &text) const
{
  mParameters.Params()[mArgumentIndex].FromString(text);
  return FunctionParameter(mParameters, mArgumentIndex+1);
}

Parameters::FunctionParameter Parameters::FunctionParameter::ref(Variant &value) const
{
  mParameters.Params()[mArgumentIndex].Alias(value);
  return FunctionParameter(mParameters, mArgumentIndex+1);
}

Parameters::FunctionParameter Parameters::FunctionParameter::const_ref(const Variant &value) const
{
  mParameters.Params()[mArgumentIndex].ConstRef(value);
  return FunctionParameter(mParameters, mArgumentIndex+1);
}


} }
