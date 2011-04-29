#ifndef REFLECT_STRING_CONSTSTRING_H_
#define REFLECT_STRING_CONSTSTRING_H_

#include <reflect/string/Fragment.h>
#include <reflect/Reflection.h>
#include <reflect/config/config.h>

namespace reflect { namespace string {

// Class: ConstString
// A ConstString is a reflected string type
// which doesn't own its data.  Usually this is
// a better type to pass around than to store.
class ReflectExport(reflect) ConstString
{
public:
	typedef Fragment::size_type size_type;
	static const size_type npos = size_type(Fragment::npos);

	// Constructor: ConstString
	// constructs a const string wrapping a c string.
	ConstString(const char *s);

	// Constructor: ConstString (default)
	// constructs an empty ("") const string.
	ConstString();

	// Function: c_str
	// returns the string as a nul-terminated pointer to const char.
	const char *c_str() const;

	// Function: data
	// same as c_str, but the data method isn't guaranteed
	// to return nul-terminated data in the other reflect 
	// string classes. For example <Fragment> has <Fragment::data> but not
	// <c_str>.
	const char *data() const;

	// Function: length
	// the number of non-nul chars in this string.
	size_type length() const;

	// Function: size
	// same as length, for compatibility with std::string.
	size_type size() const;
	
	typedef const char *const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	size_type find(char ch, size_type pos = 0) const;
	size_type find(const char *s, size_type pos = 0, size_type len = npos) const;
	size_type find(const Fragment &s, size_type pos = 0) const;
	size_type find_first_of(char ch, size_type pos = 0) const;
	size_type find_first_of(const Fragment &s, size_type pos = 0) const;

	size_type rfind(char ch, size_type pos = npos) const;
	size_type rfind(const char *s, size_type pos = npos, size_type len = npos) const;
	size_type rfind(const Fragment &s, size_type pos = npos) const;
	size_type find_last_of(char ch, size_type pos = npos) const;
	size_type find_last_of(const Fragment &s, size_type pos = npos) const;
	
	Fragment substr(size_type index, size_type amount = npos) const;

	bool empty() const;

	operator Fragment() const;

	bool operator ==(const Fragment &other) const;
	bool operator <=(const Fragment &other) const;
	bool operator >=(const Fragment &other) const;
	bool operator < (const Fragment &other) const;
	bool operator > (const Fragment &other) const;
	bool operator !=(const Fragment &other) const;
private:
	const char *mpString;
	size_type mLength;
};

inline bool operator ==(const char *a, ConstString b)
{
	return ConstString(a) == b;
}

} }

DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::ConstString)

#endif
