#ifndef REFLECT_UTILITY_CONTEXT_H_
#define REFLECT_UTILITY_CONTEXT_H_

namespace reflect { namespace utility {

// Class: Context<Type>
// A template base class which allows global access to its Type's current instance.
//
// Contexts are a lightweight and powerful (if thread unsafe) way of
// passing data across the callstack.
//
// Note: Contexts are only used in Reflect to provide access to the global
// <StringPool> used by <SharedStrings>.
template<typename Type>
class Context
{
public:
	// Function: GetContext
	// Returns:
	//    The current (most recently constructed) active context.
	static Type *GetContext();

protected:
	Context();

	// Function: PreviousContext
	// The context that was active when the current context was constructed.
	// Some contexts do not want to expose this method, so it is a protected method,
	// use an accessor declaration to expose it when desired.
	Type *PreviousContext() const;

public:
	~Context();

private:
	static Type *sContext;
	Type *sLink;

	// The following stubs disable copying of contexts.
	// Do not implement.
	Context(const Context &);
	const Context &operator =(const Context &);
};

} }
#endif
