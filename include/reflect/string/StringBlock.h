#ifndef REFLECT_STRING_STRINGBLOCK_H_
#define REFLECT_STRING_STRINGBLOCK_H_

#include <reflect/string/BlockString.h>
#include <reflect/string/String.h>

#include <vector>

namespace reflect { namespace string {

class Fragment;

// Class: StringBlock
//
// Holds a concatenated block of strings.
class StringBlock
{
public:
	typedef Fragment::size_type size_type;

	StringBlock();
	
	// Function: Load
	// Copies the data into the StringBlock.
	// Same as calling <AddString> for each terminated
	// string inside the fragment.
	void Load(const Fragment &data);

	// Function: AddString
	BlockString AddString(const Fragment &);

	// Function: FindString
	BlockString FindString(const Fragment &) const;

	// Function: FromIndex
	BlockString FromIndex(int index) const;

	// Function: data (the entire block)
	const char *data();

	// Function: size (of the entire block)
	size_type size();
private:
	const char *GetStringData(int index) const;
	String::size_type FindSortIndex(const Fragment &, bool &success) const;

	String mStringData;
	std::vector<int> mStringOffsets;

	friend class BlockString;
};

} }

#endif
