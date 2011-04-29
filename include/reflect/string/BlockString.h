#ifndef REFLECT_STRING_BLOCKSTRING_H_
#define REFLECT_STRING_BLOCKSTRING_H_

#include <reflect/string/ConstString.h>
#include <reflect/string/Fragment.h>

namespace reflect { namespace string {

class StringBlock;

// Class: BlockString
//
// BlockStrings are good for serializing strings
// when read/write order is not guarenteed.
//
// BlockStrings' data lives in String in a string block,
// and therefore can be moved.

class BlockString
{
public:
	// Constructor: BlockString
	// Constructs an empty block string.
	BlockString();

	// Function: Index
	int Index() const;

	// Function: data
	//
	// Returns:
	//   A chararcter pointer to the data.
	const char *data() const;
	
	// Function: c_str
	//
	// Returns:
	//   A chararcter pointer to NUL terminated data.
	const char *c_str() const;

	// Function: Fragment
	// Casts this string to a Fragment
	operator Fragment() const;
	
	// Function: ConstString
	// Casts this string to a ConstString
	operator ConstString() const;

	bool operator ==(const BlockString &) const;
	bool operator <=(const BlockString &) const;
	bool operator >=(const BlockString &) const;
	bool operator < (const BlockString &) const;
	bool operator > (const BlockString &) const;
	bool operator !=(const BlockString &) const;

	// Function: bool
	// false if the string is not set.
	operator bool() const;
	
	int compare(const BlockString &) const;
private:
	friend class StringBlock;

	// Constructs a pool string from a character pointer, used by StringPool.
	BlockString(const StringBlock *, int);

	const StringBlock *mStringBlock; 
	int mStringIndex;
};

} }

#endif
