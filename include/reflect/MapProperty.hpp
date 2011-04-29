#ifndef REFLECT_MAPPROPERTY_HPP
#define REFLECT_MAPPROPERTY_HPP

#include <reflect/MapProperty.h>

namespace reflect {

class ReflectExport(reflect) ReferenceCountedData
{
public:
	ReferenceCountedData(bool no_delete);
	void incref();
	void decref();
	virtual ~ReferenceCountedData();
private:
	int mReferenceCount;
};

template<typename T>
class MapPropertyMapKeyVariantData : public ReferenceCountedData
{
public:
	MapPropertyMapKeyVariantData(const T &value, bool no_delete)
		: ReferenceCountedData(no_delete)
		, mValue(value)
	{}

	T &value() { return mValue; }
private:
	T mValue;
};

template<typename T>
inline MapKeyVariant::MapKeyVariant(const T &value)
{
	if(sizeof(MapPropertyMapKeyVariantData<T>) <= sizeof(mSmallAllocation))
	{
		mData = static_cast<ReferenceCountedData *>(new(mSmallAllocation) MapPropertyMapKeyVariantData<T>(value, true));
	}
	else
	{
		mData = static_cast<ReferenceCountedData *>(new MapPropertyMapKeyVariantData<T>(value, false));
	}
}

template<typename T>
inline T &MapKeyVariant::Key()
{
	if(0 == mData)
	{
		*this = T();
	}

	return static_cast<MapPropertyMapKeyVariantData<T>*>(static_cast<ReferenceCountedData *>(mData))->value();
}

template<typename T>
inline const T &MapKeyVariant::Key() const
{
	return static_cast<MapPropertyMapKeyVariantData<T>*>(static_cast<ReferenceCountedData *>(mData))->value();
}


}

#endif
