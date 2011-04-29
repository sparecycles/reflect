#ifndef REFLECT_STRING_MUTABLESTRING_H_
#define REFLECT_STRING_MUTABLESTRING_H_

#include <cstdarg> // For va_list
#include <reflect/string/Fragment.h>
#include <reflect/string/ConstString.h>
#include <reflect/ObjectType.h>

namespace reflect { namespace string {

// Class: MutableString
//
// A string *reference* to a fixed-length array for its data.
//
// All Strings operate in a manner compatible with std::string
// wherever feasable in the functions implemented.  MutableString
// have a maximum size, but the data they contain is on loan from
// whatever constructed them.
//
// All mutating algorithms on strings supported by the String
// suite should be implemented on MutableStrings.  Strings
// use the MutableString implementations, with a possible
// resize to prevent clipping.
//
// MutableStrings require both a buffer, and a reference to the length
// of the string.  They are not meant to be constructed directly by
// client code.  Instead, use them by cast-constructing them from
// ArrayStrings or Strings.
// 
// Even more than <ConstStrings>, MutableStrings are 
// not designed to be storable.
//
// See Also:
//    - <Fragment>

// @see ConstString
// @see MutableString
// @see String
class MutableString
{
public:
	typedef Fragment::size_type size_type;
	static const size_type npos = size_type(Fragment::npos);

	typedef char *iterator;
	typedef const char *const_iterator;

	// Constructor: MutableString
	//   Constructs a mutable string.
	//
	// Parameters:
	//   buffer     - The data for the string
	//   buffersize - The size of the buffer, (including the NUL terminator)
	//   length     - A reference to the length of the string.
	MutableString(char *buffer, size_type buffersize, size_type &length);

	// Function: data
	//
	// Returns:
	//   a chararcter pointer to the data.
	const char *data() const;
	
	// Function: c_str
	//
	// Returns:
	//   a chararcter pointer to NUL terminated data.
	const char *c_str() const;
	
	// Function: length
	// the current length of the string.
	size_type length() const;

	// Function: size
	// the current length of the string.
	size_type size() const;

	// Function: capacity
	// the current capacity of the string.
	size_type capacity() const;

	// Function: format
	//   prints into the string, like printf.
	//
	// Returns:
	//    the number of characters formatted ignoring truncation.
    size_type format(const char *format, ...);

	// Function: vformat
	//   prints into the string, like vprintf.
	//
	// Returns:
	//   the number of characters formatted ignoring truncation.
	size_type vformat(const char *format, std::va_list);

	// Function: begin
	//   the const_iterator at the begining of the string.
	const_iterator begin() const;
	
	// Function: end
	//   the const_iterator at the end of the string.
	const_iterator end() const;
	
	// Function: begin
	//  the iterator at the end of the string.
	iterator begin();

	// Function: end
	//   the iterator at the end of the string.
	iterator end();

	// @see Fragment::find(char,size_type) const
	size_type find(char ch, size_type pos = 0) const;
	// @see Fragment::find(const char *,size_type,size_type) const
	size_type find(const char *s, size_type pos = 0, size_type len = npos) const;
	// @see Fragment::find(const Fragment &,size_type) const
	size_type find(const Fragment &s, size_type pos = 0) const;
	// @see Fragment::find_first_of(char,size_type) const
	size_type find_first_of(char ch, size_type pos = 0) const;
	// @see Fragment::find_first_of(const char *,size_type) const
	size_type find_first_of(const Fragment &s, size_type pos = 0) const;

	// @see Fragment::rfind(char,size_type) const
	size_type rfind(char ch, size_type pos = npos) const;
	// @see Fragment::rfind(const char *,size_type,size_type) const
	size_type rfind(const char *s, size_type pos = npos, size_type len = npos) const;
	// @see Fragment::rfind(const Fragment &,size_type,size_type) const
	size_type rfind(const Fragment &s, size_type pos = npos) const;
	// @see Fragment::find_last_of(char,size_type) const
	size_type find_last_of(char ch, size_type pos = npos) const;
	// @see Fragment::find_last_of(const char *,size_type) const
	size_type find_last_of(const Fragment &s, size_type pos = npos) const;
	
	// @see Fragment::substr(size_type,size_type) const
	Fragment substr(size_type index, size_type amount = npos) const;

	// @see Fragment::empty() const
	bool empty() const;

	operator Fragment() const;
	operator ConstString() const;

	bool operator ==(const Fragment &other) const;
	bool operator <=(const Fragment &other) const;
	bool operator >=(const Fragment &other) const;
	bool operator < (const Fragment &other) const;
	bool operator > (const Fragment &other) const;
	bool operator !=(const Fragment &other) const;

	const MutableString &operator =(const Fragment &other);
	const MutableString &operator =(const MutableString &other);
	const MutableString &operator +=(const Fragment &other);
	const MutableString &operator +=(char);

	// Replaces text from offset to offset+size with value.
	// <code>
	// ArrayString&lt;16&gt;("0123456789").replace(3, 3, "replace") == "012replace6789";
	// </code>
	// @param offset the index of the first character to replace
	// @param size the number of characters to replace
	// @param value the string to replace the characters with
	void replace(size_type offset, size_type size, Fragment value = Fragment());
private:
	char *mpBuffer;
	size_type &mLength;
	size_type mMaxSize;
};

} }

DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::MutableString)

#endif
