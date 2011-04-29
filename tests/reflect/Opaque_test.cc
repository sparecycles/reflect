#include <reflect/test/Test.h>
#include <reflect/Persistent.h>
#include <reflect/PersistentClass.hpp>

using namespace reflect;

namespace tester {
	class MIBase : public reflect::Persistent {
		DECLARE_REFLECTION(reflect::Persistent)
	public:
		int value;
	};

	struct MIInjectA { double injected; virtual void f() {} };

	class MIClass : public MIInjectA, public MIBase {
		DECLARE_REFLECTION(MIBase)
	public:
	};

	class MIClass2 : public MIBase, public MIInjectA {
		DECLARE_REFLECTION(MIBase)
	public:
	};
}

DEFINE_REFLECTION(tester::MIBase, "reflect::test::MIBase")
{
	+ Concrete;
}

DEFINE_REFLECTION(tester::MIClass, "reflect::test::MIClass")
{
	+ Concrete;
}

DEFINE_REFLECTION(tester::MIClass2, "reflect::test::MIClass2")
{
	+ Concrete;
}

TEST(MultipleInheritence)
{
	ASSOCIATE(tester::MIBase);
	ASSOCIATE(tester::MIClass);
	ASSOCIATE(tester::MIClass2);

	double memory[(sizeof(tester::MIClass)+sizeof(double)-1)/sizeof(double)];

	Persistent *miclassobject = tester::MIClass::TheClass()->Construct(memory);
	CHECK(reinterpret_cast<void *>(miclassobject) != reinterpret_cast<void *>(memory));
	CHECK(static_cast<tester::MIInjectA *>(static_cast<tester::MIClass *>(miclassobject)) == reinterpret_cast<void *>(memory));
	miclassobject->~Persistent();

	miclassobject = tester::MIBase::TheClass()->Construct(memory);
	CHECK(reinterpret_cast<void *>(miclassobject) == reinterpret_cast<void *>(memory));
	miclassobject->~Persistent();

	miclassobject = tester::MIClass2::TheClass()->Construct(memory);
	CHECK(reinterpret_cast<void *>(miclassobject) == reinterpret_cast<void *>(memory));
	CHECK(static_cast<tester::MIInjectA *>(static_cast<tester::MIClass2 *>(miclassobject)) != reinterpret_cast<void *>(memory));
	miclassobject->~Persistent();
}
