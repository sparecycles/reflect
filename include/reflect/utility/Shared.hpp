#ifndef REFLECT_UTILITY_SHARED_HPP_
#define REFLECT_UTILITY_SHARED_HPP_

namespace reflect { namespace utility {

template<typename T>
class Shared
{
protected:
	Shared()
	{
		mNextShared = this;
	}
	
	~Shared()
	{
		SharedRelease();
	}
	
	void SharedLink(const Shared &next)
	{
		SharedRelease();
		
		mNextShared = next.mNextShared;
		next.mNextShared = this;
	}
	
	bool SharedRelease()
	{
		Shared *prev = 0;
		
		for(Shared *next = this->mNextShared; next != this; next = next->mNextShared)
		{
			prev = next;
		}
		
		if(prev)
		{
			prev->mNextShared = this->mNextShared;
			this->mNextShared = this;
			return false;
		}
		
		return true;
	}
	
	T *NextShared()
	{
		return static_cast<T *>(mNextShared);
	}
private:
	mutable Shared *mNextShared;
};

} }

#endif