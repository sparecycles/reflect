#include <reflect/utility/Context.h>

namespace reflect { namespace utility {

template<typename Type>
Type *Context<Type>::GetContext()
{
	return sContext;
}

template<typename Type>
Context<Type>::Context()
	: sLink(sContext)
{
	sContext = static_cast<Type *>(this);
}

template<typename Type>
Type *Context<Type>::PreviousContext() const
{
	return sLink;
}

template<typename Type>
Context<Type>::~Context()
{
	sContext = sLink;
}

template<typename Type>
Type *Context<Type>::sContext = 0;

} }
