#ifndef REFLECT_STRING_POOLSTRING_H_
#define REFLECT_STRING_POOLSTRING_H_

#include <reflect/string/ConstString.h>
#include <reflect/string/Fragment.h>
#include <reflect/config/config.h>

namespace reflect { namespace string {

// Class: SharedString
// SharedStrings are like ConstStrings except their string pointers are
// guaranteed to be one-one with their data.  Comparisons between SharedStrings is
// uses the pointer, rather than the data, and therefore may give
// different orderings on different runs/platforms.
//
// To conserve space, and because SharedStrings are used in maps,
// there is no need to store the length of the string in the
// structure.
//
// Consider SharedString the same thing as a const char * for use in maps
// with pooling keeping them unique.
//
// SharedStrings may only be constructed from StringPools, which
// guarantee the uniqueness of the strings.
//
// There is a global string pool stack formed from <StringPoolContext>
// objects (see Context).  Most uses of a SharedString will require that the
// string be registered in the global pool stack, any strings that are not
// on the global stack should probably not be accessable as SharedStrings to
// prevent confusion.

class StringPool;

class ReflectExport(reflect) SharedString
{
public:

	// Enumeration: ConstructMode
	//   Insert - Insert a new string if not found.
	//   Find   - Find only, constructs an empty string if not found.

	static SharedString Literal(const char *);
	static SharedString Copy(string::Fragment fragment);
	static SharedString Find(string::Fragment fragment);

	// Constructor: SharedString
	// Constructs an empty shared string.
	SharedString();

	// Function: data
	//
	// Returns:
	//    A chararcter pointer to the data.
	const char *data() const;

	// Function: c_str
	//
	// Returns:
	//    A chararcter pointer to NUL terminated data.
	const char *c_str() const;

	// Function: Fragment
	// Casts this string to a Fragment
	operator Fragment() const;

	// Function: ConstString
	// Casts this string to a ConstString
	operator ConstString() const;

	bool operator ==(const SharedString &) const;
	bool operator <=(const SharedString &) const;
	bool operator >=(const SharedString &) const;
	bool operator < (const SharedString &) const;
	bool operator > (const SharedString &) const;
	bool operator !=(const SharedString &) const;

	// Function: bool
	// false if the string is not set.
	operator bool() const;

	// Function: empty
	// true if the string is not set, or is the empty string.
	bool empty() const;

	static void Serialize(const SharedString *in, SharedString *out, reflect::Reflector &reflector);

private:
	friend class StringPool;

	// Constructor: SharedString(const char *string, const StringPool *owner)
	//
	// Constructs a shared string from a character pointer, used by <StringPools>.
	SharedString(const char *string, const StringPool *owner);

	int compare(const SharedString &) const;

	// The character pointer to the string data
	const char *mpString;
};

// Class: FoundSharedString
// A shared string that is always found, to allow easy construction/passing
// without worrying about adding new strings.  For Example, Use it for accessing maps
// when the map keys are made of <SharedStrings>.
//
// - Has implicit casts to/from const char *.
class FoundSharedString : public SharedString
{
public:
	FoundSharedString()
	{}
	
	FoundSharedString(const SharedString &other)
		: SharedString(other)
	{}
	
	explicit FoundSharedString(const char *text)
		: SharedString(SharedString::Find(text))
	{}
	
	FoundSharedString(const reflect::string::Fragment &text)
		: SharedString(SharedString::Find(text))
	{}

	operator const char *() const { return data(); }
};

inline
SharedString::SharedString()
	: mpString(0)
{}

inline
const char *SharedString::data() const
{
	return mpString;
}

inline
const char *SharedString::c_str() const
{
	return mpString;
}

#ifdef REFLECT_FAST_SHARED_STRING
inline
bool SharedString::operator ==(const SharedString &other) const
{
	return mpString == other.mpString;
}

inline
bool SharedString::operator < (const SharedString &other) const
{
	return mpString < other.mpString;
}
#endif

} }

DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::SharedString)
DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::FoundSharedString)

#endif
