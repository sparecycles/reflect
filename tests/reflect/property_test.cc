#include <reflect/Persistent.h>
#include <reflect/string/String.h>
#include <reflect/PersistentClass.hpp>
#include <reflect/StructType.hpp>
#include <reflect/EnumType.hpp>
#include <reflect/PropertyPath.h>
#include <reflect/test/Test.h>

#include <vector>
#include <map>

using namespace reflect;

class property_tester : public Persistent
{
	DECLARE_REFLECTION(Persistent)
public:
	int value;
	const int &direct_read() const { return value; }
	void direct_write(const int &x) { value = x; }
	void indirect_read(int &x) const { x = value; }
	void indirect_write(const int &x) { value = x; }
	int read() const { return value; }
	void write(int x) { value = x; }
	void variant_read(Reflector &reflector) const { reflector | value; }
	void variant_write(Reflector &reflector) { reflector | value; }
};

DEFINE_REFLECTION(property_tester, "reflect_test::property_tester")
{
	Properties
		("value", &property_tester::value)
		("direct", &property_tester::direct_read, &property_tester::direct_write)
		("indirect", &property_tester::indirect_read, &property_tester::indirect_write)
		("accessor", &property_tester::read, &property_tester::write)
		("variant", &property_tester::variant_read, &property_tester::variant_write, Variant)
		;
}

TEST(int_prop)
{
	property_tester test;
	
	test.value = 10;
	
	CHECK(test.Property("value").Read() == "10");
	CHECK(test.Property("direct").Read() == "10");
	CHECK(test.Property("indirect").Read() == "10");
	CHECK(test.Property("accessor").Read() == "10");
	
	CHECK(test.Property("value").Write("11"));
	CHECK(test.Property("direct").Read() == "11");

	CHECK(test.Property("direct").Write("12"));
	CHECK(test.Property("indirect").Read() == "12");

	CHECK(test.Property("indirect").Write("13"));
	CHECK(test.Property("accessor").Read() == "13");

	CHECK(test.Property("accessor").Write("14"));
	CHECK(test.Property("variant").Read() == "14");

	CHECK(test.Property("variant").Write("15"));
	CHECK(test.Property("value").Read() == "15");
}
