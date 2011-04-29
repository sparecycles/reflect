#ifndef REFLECT_STRING_STRINGPOOL_H_
#define REFLECT_STRING_STRINGPOOL_H_

#include <reflect/string/SharedString.h>
#include <reflect/utility/Context.h>
#include <reflect/config/config.h>
#include <vector>

namespace reflect { namespace string {

// Class: StringPool
// 
// A string pool manages <SharedStrings>.
class ReflectExport(reflect) StringPool
{
public:
	// Constructor: StringPool
	explicit StringPool(const StringPool *parent = NULL);

	// Function: Copy
	SharedString Copy(const Fragment &);

	// Function: Literal
	SharedString Literal(const ConstString &);

	// Function: Find
	SharedString Find(const Fragment &) const;

protected:
	const StringPool *mParent;

private:
	const char *FindRecursive(const Fragment &) const;
	const char *FindFirst(const Fragment &, std::vector<const char *>::iterator &);
	std::vector<const char *>::size_type BinarySearch(const Fragment &, bool &) const;
	
	std::vector<const char *> mStringPool;
};

// Class: StringPoolContext
class ReflectExport(reflect) StringPoolContext 
	: public StringPool
	, public utility::Context<StringPoolContext>
{
public:
	StringPoolContext();
};

} }

#endif
