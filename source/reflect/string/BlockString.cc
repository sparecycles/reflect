#include <reflect/string/BlockString.h>
#include <reflect/string/StringBlock.h>

namespace reflect { namespace string {
	
BlockString::BlockString()
	: mStringBlock(NULL)
	, mStringIndex(-1)
{}

BlockString::BlockString(const StringBlock *block, int index)
	: mStringBlock(block)
	, mStringIndex(index)
{
}

const char *BlockString::data() const
{
	return c_str();
}

const char *BlockString::c_str() const
{
	if(NULL == mStringBlock) return NULL;
	return mStringBlock->GetStringData(mStringIndex);
}

int BlockString::Index() const
{
	return mStringIndex;
}

bool BlockString::operator ==(const BlockString &other) const
{
	if(other.mStringBlock == mStringBlock && other.mStringIndex == mStringIndex)
		return true;

	return compare(other) == 0;
}

bool BlockString::operator <=(const BlockString &other) const
{
	return compare(other) <= 0;
}

bool BlockString::operator >=(const BlockString &other) const
{
	return compare(other) >= 0;
}

bool BlockString::operator < (const BlockString &other) const
{
	return compare(other) <  0;
}

bool BlockString::operator > (const BlockString &other) const
{
	return compare(other) >  0;
}

bool BlockString::operator !=(const BlockString &other) const
{
	if(other.mStringBlock == mStringBlock && other.mStringIndex != mStringIndex)
		return true;

	return compare(other) != 0;
}

int BlockString::compare(const BlockString &other) const
{
	return std::strcmp(c_str(), other.c_str());
}

BlockString::operator bool() const
{
	return NULL != c_str();
}

BlockString::operator Fragment() const
{
	return data() ? Fragment(data()) : NULL;
}

BlockString::operator ConstString() const
{
	return c_str() ? ConstString(c_str()) : NULL;
}

} }
