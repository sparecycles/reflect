include projects/gnu/Makefile.rules

all : reflect reflect_tests reflect_js doc

SOURCES:=$(wildcard source/reflect/*.cc source/reflect/*/*.cc)
program:=reflect
include projects/gnu/Makefile.program

# javascript module ---
SOURCES:=$(filter-out %js.c %jscpucfg.c %jskwgen.c,$(wildcard source/reflect_js/*.cc ext/js/src/*.c))
module:=reflect_js
include projects/gnu/Makefile.module 
include ext/js/src/config/Linux_All.mk
JS_CPPFLAGS += -I./ext/js/src
JS_CFLAGS = $(OS_CFLAGS) $(C_ARCH)
$(OBJECTS) $(OBJECTS:.o=.d) : CPPFLAGS += $(JS_CPPFLAGS)
$(OBJECTS) $(OBJECTS:.o=.d) : CFLAGS += $(JS_CFLAGS)

$(OBJECTS) $(OBJECTS:.c=.d) : ext/js/src/jsautocfg.h ext/js/src/jsautokw.h

ext/js/src/jscpucfg ext/js/src/jskwgen : LDFLAGS=
ext/js/src/jsautocfg.h : ext/js/src/jscpucfg
	./$< > $@
ext/js/src/jsscan.o : ext/js/src/jsautokw.h
ext/js/src/jsautokw.h : ext/js/src/jskwgen
	./$< > $@
# --- end javascript module

SOURCES:=$(wildcard tests/reflect/*.cc)
module:=reflect_tests
include projects/gnu/Makefile.module

clean : 
	rm -f ext/js/src/jsautocfg.h ext/js/src/jsautokw.h ext/js/src/jscpucfg ext/js/jskwgen
	rm -f $(ALL_TARGETS)
	rm -f $(ALL_OBJECTS)
	rm -f $(ALL_OBJECTS:.o=.d)

static : reflect_static
reflect_static : $(ALL_OBJECTS)
	$(CXX) $(CFLAGS) $^ -o reflect_static -ldl

.PHONY : tags js_test test test_v static doc

doc :
	- ./makedoc

tags : 
	ctags -I ReflectExport+ -I JS_PUBLIC_API --recurse source include ext/js/src

test : reflect reflect_tests
	./reflect --load ./reflect_tests.so --execute reflect_test::RunTests 

test_v : reflect reflect_tests
	./reflect --load ./reflect_tests.so  --execute reflect_test::RunTests --verbose

js_test : reflect reflect_js
	./reflect --load ./reflect_js.so --execute reflect_test::RunTests 

debug_js_test : reflect reflect_js
	pyclewn --args "--args reflect --load ./reflect_js.so --execute reflect_js::Test"

debug_test : reflect
	pyclewn --args "--args reflect --execute reflect_test::RunTests"

