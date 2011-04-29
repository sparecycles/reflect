#ifndef REFLECT_STRING_FRAGMENT_H_
#define REFLECT_STRING_FRAGMENT_H_

#include <reflect/Reflection.h>
#include <reflect/PrimitiveType.h>
#include <reflect/config/config.h>

namespace reflect { namespace string {

class ConstString;

// Class: Fragment
//
// A Fragment is the most basic string type in the reflect string suite.
// 
// It consists of a pointer and a length.  It does not own its data.
// It can and should provide any const std::string functions we need
// that do not require a NUL terminator.
class ReflectExport(reflect) Fragment
{
public:
	typedef unsigned int size_type;
	static const size_type npos = size_type(-1);
	typedef const char *const_iterator;

	// Constructor: Fragment
	// Constructs a piece string from a character pointer to NUL terminated data.
	// length is found by strlen()
	// @param s the string to make into a piece string.
	Fragment(const char *s);
	// Constructs a piece string from a character pointer, length provided.
	// @param s the character data
	// @param length the length of the string
	Fragment(const char *s, size_type length);
	// Constructs an empty piece string with NULL data and no length.
	Fragment();

	// @return a chararcter pointer to the data.
	const char *data() const;
	// @return the current length of the string.
	size_type length() const;
	// @return the current length of the string.
	size_type size() const;

	// @return a const_iterator at the begining of the string.
	const_iterator begin() const;
	// @return a const_iterator at the end of the string.
	const_iterator end() const;

	// Finds a character in the string.
	// @param ch the character to search for
	// @param pos the position to start from
	// @return index of ch in string or npos if not found.
	size_type find(char ch, size_type pos = 0) const;
	// Finds a string in the string.
	// @param s the string to search for
	// @param pos the position to start from
	// @return index of s in string or npos if not found.
	size_type find(const char *s, size_type pos = 0, size_type len = npos) const;
	// Finds a string in the string.
	// @param s the string to search for
	// @param pos the position to start from
	// @return index of s in string or npos if not found.
	size_type find(const Fragment &s, size_type pos = 0) const;
	// Finds a character in the string.
	// @param[in] ch the character to search for
	// @param[in] pos the position to start from
	// @return index of ch in string or npos if not found.
	size_type find_first_of(char ch, size_type pos = 0) const;

	// Function: find_first_of
	// Finds any of a set of characters in the string.
	//
	// Parameters:
	//   s - the set of characters to search for
	//   pos - the position to start from
	//
	// Returns:
	//   index of s in string or npos if not found.
	size_type find_first_of(const Fragment &, size_type pos = 0) const;

	// Function:
	// Finds a character in the string, in reverse.
	//
	// Parameters:
	//   ch - the character to search for
	//   pos - the position to start from
	//
	// Returns:
	//   index of ch in string or npos if not found.
	size_type rfind(char ch, size_type pos = npos) const;

	// Function: rfind
	// Finds a string in the string, in reverse.
	//
	// Parameters:
	//   s - the string to search for
	//   pos - the position to start from
	//
	// Returns:
	//   index of the begining of s in string or npos if not found.
	size_type rfind(const char *s, size_type pos = npos, size_type length = npos) const;

	// Function: rfind
	// Finds a string in the string, in reverse.
	//
	// Parameters:
	//   s - the string to search for
	//   pos - the position to start from
	//
	// Returns:
	//   index of the begining of s in string or npos if not found.
	size_type rfind(const Fragment &s, size_type pos = npos) const;

	// Function: find_last_of
	// Finds a character in the string, in reverse.
	//
	// Parameters:
	//  ch - the character to search for
	//  pos - the position to start from
	//
	// Returns:
	//   index of the last character in string matching ch or npos if not found.
	size_type find_last_of(char ch, size_type pos = npos) const;
	
	// Function: find_last_of
	// Finds any of a set of characters in the string, in reverse.
	//
	// Parameters:
	//   s - the set of characters to search for
	//   pos - the position to start from
	//
	// Returns:
	//   index of the last character in string contained in s or npos if not found.
	size_type find_last_of(const Fragment &s, size_type pos = npos) const;
	
	// Function: substr
	//   Makes a substr fragment from this one.
	// The substr's storage is the same as the fragment's storage.
	//
	// Parameters:
	//  index - the begining of the substring
	//  amount - the maximum length of the substring
	//
	// Returns:
	//    - a Fragment substring
	Fragment substr(size_type index, size_type amount = npos) const;

	// Function: empty
	bool empty() const;

	bool operator == (const Fragment &other) const;
	bool operator <= (const Fragment &other) const;
	bool operator >= (const Fragment &other) const;
	bool operator <  (const Fragment &other) const;
	bool operator >  (const Fragment &other) const;
	bool operator != (const Fragment &other) const;
 
	// Function: compare
	//
	// Returns:
	//  (+1, 0, or -1) - if the string is
	//      (greater, equal, or less) than the other string. 
	int compare(const Fragment &other) const;

	static void Serialize(const Fragment *in, Fragment *out, Reflector &reflector);
private:
	// the character pointer for this string's data
	const char *mpString;
	// the length of the string.
	size_type mLength;
};

} }

DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::Fragment)

#endif
