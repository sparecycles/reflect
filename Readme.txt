File: Readme

Reflect is a cohesive framework for doing
runtime-type-identification (RTTI) and serialization in
C++.  Reflect provides tools to identify, save, load,
inspect and edit reflected objects.

Section: Overview

What Reflect is and why you want to use it.

Topic: Features

Reflect is a complete C++ RTTI framework.
Reflect allows you to 

 - inspect types;
 - serialize, 
 - convert, and
 - marshal data; and
 - unit test components. 

This is done with representative <Type>
objects which represent, or reflect, your C++ types.
A reflected type is a C++ type that has a Type object associated with it.

Topic: Replaces RTTI

Reflect does not depend on C++'s built-in RTTI: typeid and dynamic_cast.
I keep these turned off, they're not completely portable. For instance, 
typeid.name() is often mangled differently on different platforms/compilers.

Topic: The Two Faces of Reflection

Reflect, mirroring C++, has two faces: *static* information
known at compiletime, and *dynamic* information, known at runtime.
Static types of all flavors are reflected by <Types>, 
<Dynamic> types are reflected by <Class>es 
(which extends <Type>).  The reflection object can be accessed 
by <TypeOf>.  A <Dynamic> object also shows
its type through the (virtual) <Dynamic.GetClass> method.

Topic: Unit Testing

Reflect includes its own Unit-<Test> framework.
It borrows a little syntax from UnitTest++
(http://unittest-cpp.sourceforge.net/)
With the main additional feature that it can associate
tests with Types, and print results grouped by Type, so it provides
a rough guide of code coverage (and many features omitted).

Topics: Philosophy

Extendable - <Type> (and <Class>) is extendable, 
this is done in reflect but you can too!

SURPRISES - Less please. If you didn't tell reflect something,
it's not going to figure it out by parsing header files or any other
outside-of-the-compiler magic.
This means that some members can be reflected,
other can be maintained implicitly, or accessed through functions only.
Everything, while assisted as much as feasable
with templates, macros, and syntactic sugar cubes, is done manually.
I'm not claiming that this is always a good thing, but it does
allow for some interesting flexibility.

Lightweight - Dynamically reflected objects need no more than their
virtual table to identify them to the reflection system.  Static objects have
nothing to identify them at runtime, but can be passed around dynamically
in a <Variant>.

Platform Integration - Reflect is designed so that no memory allocations
need to be performed before main() starts, making it that much 
easier to initialize a custom memory system before reflect starts up.

Supports Change - The serialization pattern reflect
uses is capable of dealing with an evolving program.  A member or property
added or removed won't invalidate old data, extra data can be ignored without
trouble, and missing data was handled by the objects' constructor (right??).

Plugin System - Reflect has macros designed to help
automate building DLLs.  Dynamically loaded classes are looked up and used just
like resident classes, so Reflect can be used to hide the differences between
dynamic and static linking.  By default reflect is configured as an exe, and
can load other DLLs and execute them, but it can also be switched to a DLL and
linked into another application.

Section: Tutorial

Or: Let's see some examples already.

Lets to start with a <Persistent>, which is a <Dynamic> that
can be saved/loaded/edited through its properties.  There are
three places we need to look at: declaring reflection in the
header file, defining it in the implementation file,
and using it somewhere else.

Topic: Declaring Reflection

Suppose you want to represent Circles by center and radius,
you plan to make circle subclass Shape, so it should be at least
Dynamic.  You're also writing a tool to edit these circles, so you
want to be able to save and load and edit Circles.

(begin code)
// --- Circle.h

#include <reflect/Persistent.h>
#include <reflect/StructType.h> 
#include <cmath>

namespace example {

class Circle                     //    must match
	: public reflect::Persistent // <-------------+ 
{                                           //    |
	DECLARE_REFLECTION(reflect::Persistent) // <--+
public:
	struct Point
    {
		// default constuctor arguments so this can be default-constructed.
		Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
		double x, y;
    };

	// default constuctor arguments so this can be default-constructed.
	Circle(Point center = Point(), double size = 1.0)
		: mCenter(center)
		, mSize(size)
	{}

	// a regular method call
	void Move(double xstep, double ystep)
    { 
        mCenter.x += xstep;
        mCenter.y += ystep;
    }

	// find the distance to another Circle.
	// (overlapping circles have a negative distance)
	double Distance(const Circle &other) const
	{
		double dx = mCenter.x - other.mCenter.x;
		double dy = mCenter.y - other.mCenter.y;

		return std::sqrt(dx*dx + dy*dy) - mSize - other.mSize;
	}

private:
	Point mCenter;
	double mSize;
};

}

DECLARE_STATIC_REFLECTION(local, example::Circle::Point, reflect::StructType)

}
(end code)

Topic: Defining Reflection

At this point we have defined the circle type, and declared it and
the inner Point type reflected.  Not much is in the needed in the .h 
file for declaring reflection.
The reflection information for Circle goes in the .cc file.

(begin code)
// --- Circle.cpp
#include "Circle.h"
#include <reflect/PersistentClass.hpp> // needed for Circle reflection
#include <reflect/StructType.hpp> // needed for Point reflection

DEFINE_REFLECTION(example::Circle, "ex::Circle")
{
	+ Concrete;

	Properties
		("center", &example::Circle::mCenter)
            ["tooltip"]("Center of the circle") // <-- annotation
		("size", &example::Circle::mSize)
		;

	Functions
		("Distance", &example::Circle::Distance)
		("Move", &example::Circle::Move)
		;
}

// different reflection macro for external reflection.
DEFINE_STATIC_REFLECTION(Circle::Point, "Circle::Point")
{
	+ Concrete;

	// structs act like tuples.
	Members
		(&Circle::Point::x)
		(&Circle::Point::y)
		;
}
(end code)


Topic: Using Reflection

Some uses of the reflected Circle class

(begin code)
#include "Circle.h"
#include <reflect/autocast.h>
#include <reflect/function/Function.h> // for using Persistent.Function
#include <reflect/PropertyPath.h> // for using Persistent.Property

void test(reflect::Persistent *a_circle_maybe)
{
	// autocast: like dynamic_cast but less obtrusive
    // and it follows the DRY rule (Don't Repeat Yourself)

	if(example::Circle *circle = a_circle_maybe % reflect::autocast)
	{
		// yes it's a circle
		// use it directly...
		(void)circle;
		printf("it's a circle\n");
	}

	// The following assumes something duck-typed like a circle,
	// i.e., has center property, has "Move" and "Distance" functions.

	// but even if it's not... we can still move it using its property
	{
        // reading data with an actual type.
		example::Circle::Point old_center;
		bool read_result = a_circle_maybe->Property("center").ReadData(old_center);

		example::Circle::Point new_center(3,5);
		bool write_result = a_circle_maybe->Property("center").WriteData(new_center);

        // reading the serialized form of a circle::Point
		reflect::String center_text = a_circle_maybe->Property("center").Read();

		// writing a new value using formatted text
        write_result = a_circle_maybe->Property("center").Writef("%g %g", 45.6, 24.8);

		(void)read_result;
		(void)write_result;
	}

	// or, let it decide how to move
	if(const reflect::function::Function *move_func = a_circle_maybe->Function("Move"))
	{
		function::Parameters move_params(move_func);
		move_params.SetParamValue(0, 2.4); // by reflected value
		move_params.SetParam(1, "27.3"); // or by StandardSerializer data
		if(false == move_func->Call(a_circle_maybe, move_params))
		{
			// error?
		}
	}
	
	// A result from a function can be found by the following
	if(const reflect::function::Function *distance_func = a_circle_maybe->Function("Distance"))
	{
		function::Parameters distance_params(distance_func);
		distance_params.CopyParamValue(0, *a_circle_maybe);

        if(distance_func->NumParameters() > 0)
            printf("Distance wants a %s\n", distance_params[0].GetType()->Name());

		double distance;
		reflect::Variant result = reflect::Variant::Ref(distance);
		if(distance_func->Call(a_circle_maybe, distance_params, result))
		{
			printf("self circle distance is %g\n", distance);
		}
	}

	Variant variant = Variant::Ref(*a_circle_maybe);
	reflect::string::String serialized = variant.ToString();

	// save serialized to file ...
	// close program

    // writes something like: "( $center=%d %d; $size=%d; )" 
	
	// open program
	// load serialized from file ...
	
	Circle clone;
	Variant clone_variant = Variant::Ref(clone);
	clone_variant.FromString(serialized);
}
(end code)

Section: Implementation

The nitty-gritty details.

Topic: Signature and Dynamic

At the center of all of reflect are <Signature> and <Dynamic>.
<Signature> is the struct template that provides compiletime 
access to <Type> objects and the virtual function which provides
runtime access.  A Signature provides a typedef for the
class type and a method <Signature::TheClass> for accessing it.  A
<Dynamic> also provides a typedef for the class type and static method
<Dynamic::TheClass> to access it, as well as a virtual method
<Dynamic.GetClass>.

The static typedef allows subclasses of Dynamics to inherit their parents'
<Class> type unless it's overridden.  When overriding <Class> types, a subclass
must use a subclass of the parents' class type (this is actually enforced at
compile time because <Dynamic.GetClass> takes advantage of covariance in its
return type: i.e., a <Persistent> returns a <PersistentClass> from 
GetClass even though it's a virtual declared to return a <Class>).

The main use of <Signature> is the <TypeOf> template function, which
obviates the typing of Signature<T>::TheClass() with the more readable
TypeOf<T>().  The main use of <Dynamic.GetClass> is the <autocast>
object, which is a syntactic sugar object superior to dynamic_cast.

Topic: Variants

<Variant> provides a nice way of wrapping up a static type 
with some extra data so it can be passed through runtime without losing
information.  <Variant>'s handle a lot of cases, they can own
their data, or reference existing data, (possibly const data), 
they can <Variant.Alias> data, which is interesting when passing 
parameters of not-quite-the-right-type by reference into reflected functions.
Variant's can also be typed without having values.  Variants are reference
counted (actually a circular linked list is used), so constructed data
can be shared among variants efficiently.

In order to implement variant we have the concept
of an opaque pointer: a cannoical void * pointer to an object 
(we need pointers canonicallized to deal with multiple inheritance).
At runtime, an opaque pointer and a <Type> is just as good as
a statically-typed value.  Opaque values can
be manipulated with the <Type.ConvertValue> method, which converts
one value into another, this is used extensively in <Variant> 
and probably does not need to be accessed directly.

Each class provides methods <Type.OpaqueCast> 
and <Type.TranslucentCast> to convert typed 
pointers to and from opaque ones.  However, rather than using the awkward

> T *t_ptr;
> void *opaque = Signature<T>::ClassType::OpaqueCast(t_ptr);
> t_ptr = Signature<T>::ClassType::template TranslucentCast<Target>(opaque)

please use <opaque_cast> and <translucent_cast>

> T *t_ptr;
> void *opaque = opaque_cast(t_ptr);
> t_ptr = translucent_cast<T *>(t_ptr);

Variants use this internally, as does the Property system. 
In general, if you see a static_cast in your code, always consider
using an opaque instead.

Topic: Defining reflection

Reflection definitions are facilitated by the <Type.DescriptionHelper> 
(or extensions of it), which contains syntactic sugar objects
to make reflecting features easier.  The description helper type
is friended in the <DECLARE_REFLECTION> macro, so pointers to
private members can be referenced in the reflection definition.

Topic: Serialization

How to use reflection to save and load things.

Above, you see a fairly basic use of serialization.
Most of the time, you will want to use the <StandardSerializer> 
or the <ShallowSerializer> modification of it.  There are times
when this is insufficient, like

    - when floating point numbers can't take being decimalized and reparsed, or
    - when text is too big, and you want binary.

Serialization operates with a few primitive data token types

    - bool
    - long
    - double
    - string
    - binary data

and tags which provide a hierarchical structure like XML,
but (in the StandardSerializer format) with only one-token lookahead
to determine tag type.  (It would be possible to implement streaming 
XML serializers/deserializers using the same interface.)  

Serializers are driven by writing data to them, as well as providing structure
with <Serializer.Begin> and <Serializer.End>.  A <Deserializer> executed
on the output of a Serializer with the parallel calls should read data very
similar to the data written.

The hierarchical structure of streams allows derializers to skip unknown
sections of data.  Skipping happens when the <Deserializer.End> method is
called but an end tag is not in the stream, if the serializer supports
skipping, it will try to read and discard data and tags until the matching end
tag is reached.  Not all serializers need to support skipping, but it is 
highly recommended that data is persisted in a form that does. 
(The <StandardSerializer> does, of course, support skipping data).

Topic: Serialization and the Stack

Serialization makes heavy use of the function call stack, recursing
on first use of any <Persistent> pointer (on a big graph, this can be very deep).

It might be possible to delay serializing/deserializing any properties of
pointers: do a shallow serialization, remembering all pointer properties, 
and then repeatedly flush and serialize remaining new pointers into the 
stream as a finalization step. Thoughts?

Topic: A Note on Properties

<Properties> are <Dynamic> objects, reflected as different types for different
levels of interfaces.  The particular implementations are not reflected, C++
polymorphism takes over from there.  The property system illustrates when
reflection is good, and when to abandon it: there is no rule that a subclass of
a reflected object must be reflected, but take care when serializing such
objects, because they will be sliced.

Section: Whining - No

<DECLARE_REFLECTION> and <DEFINE_REFLECTION> are rather large macros.  While it
is possible to do a lot more of the work using template injection, it caused
problems making reflected DLLs/SharedObjects, so that approach was abandoned.
Template injection is not debugger friendly, nor is it constructor friendly,
so it has other drawbacks which outweigh the stylistic issues with macro
injection methods.  Sometimes the debugger will step through these macros, 
but the work done in these is minimal and safe.

Exceptions are not used in Reflect, due mainly to my unfamiliarity with them
and that the original target of its (proprietary) spiritual predecessor miniork
(http://www.tweakoz.com/) was embedded development.
Exceptions would make certain actions well defined (what happens when you
request the wrong Type as a reference from a variant?) 

Don't register two properties with the same name in a single inheritance chain
or two classes with the same name anywhere, the serialization mechanisms will
fail.  If error checking these conditions is necessary, then <Type::LoadTypes>
would have to be extended to possibly fail and cleanup these cases.

Reflect requires a modern C++ compiler, it was developed to support both 
Visual Studio 2008 Express and gcc 4.3.1.

Section: License

Reflect is released to the public in the hopes that it will do some good
in the world.  Ideally it (and it's horrible machinations) will 
actually convince people to stop using C++, or at least use it less
and cross-script more.

Section: Author
Adam Freidin, adam.freidin@gmail.com.

Special Thanks To:
	Michael Mayers - http://www.tweakoz.com/, for that which came before.
	Hayley Goerisch - for putting up with me and my midnight coding.
	Santa Cruz Games - for letting this code out.
