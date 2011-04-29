#include <reflect/string/StringBlock.h>

namespace reflect { namespace string {

StringBlock::StringBlock()
{
}

void StringBlock::Load(const Fragment &data)
{
	mStringData.reserve(data.size());

	for(Fragment::size_type index = 0; index < data.size(); )
	{
		ConstString item(data.data() + index);

		index += item.length() + 1;

		AddString(item);
	}
}

BlockString StringBlock::FindString(const Fragment &in) const
{
	bool found;
	
	String::size_type sort_index = FindSortIndex(in, found);

	if(found)
	{
		BlockString result = FromIndex(mStringOffsets[sort_index]);
		return result;
	}
	else
	{
		return BlockString();
	}
}

BlockString StringBlock::AddString(const Fragment &in)
{
	bool found;
	
	String::size_type sort_index = FindSortIndex(in, found);

	if(found)
	{		
		return FromIndex(mStringOffsets[sort_index]);
	}
	else
	{
		int offset = int(mStringData.size());
		mStringData += in;
		mStringData += '\0';
		mStringOffsets.insert(mStringOffsets.begin() + int(sort_index), offset);
		return FromIndex(offset);
	}
}

const char *StringBlock::GetStringData(int index) const
{
	return mStringData.data() + index;
}

BlockString StringBlock::FromIndex(int index) const
{
	return BlockString(this, index);
}

String::size_type StringBlock::FindSortIndex(const Fragment &string, bool &success) const
{
	std::vector<int>::size_type low = 0, high = mStringOffsets.size();

	while(low < high)
	{
		std::vector<int>::size_type mid = (low + high) / 2;
		int cmp = string.compare(GetStringData(mStringOffsets[mid]));
		if(cmp < 0)
		{
			high = mid;
		}
		else if(cmp > 0)
		{
			low = mid + 1;
		}
		else
		{
			success = true;
			return String::size_type(mid);
		}
	}

	success = false;
	return String::size_type(low);
}

const char *StringBlock::data()
{
	return mStringData.data();
}

StringBlock::size_type StringBlock::size()
{
	return mStringData.size();
}

} }
