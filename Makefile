# License of this file can be seen in the LICENSE file.
#  Based on https://gitlab.com/snippets/1734324/raw
#  License of original can be found at https://gitlab.com/snippets/1734324

CXXARGS=-std=c++11
CXXARGS_DEBUG=-g -Werror -Wall -pedantic -DDEBUG
CXXARGS_RELEASE=-Wall -pedantic -O3 -DNDEBUG

LINTER=cpplint --filter=-legal/copyright

DEBUG=$(filter debug test,$(MAKECMDGOALS))
OBJ_EXT=$(if $(DEBUG),.debug.o,.o)
FINAL_ARGS=$(CXXARGS) $(if $(DEBUG),$(CXXARGS_DEBUG),$(CXXARGS_RELEASE))
INCLUDE_PATH=-Iinclude -Isrc

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find include -name "*.h")
OBJECTS_RELEASE=$(addsuffix .o,$(basename $(SOURCES)))
OBJECTS_DEBUG=$(addsuffix .debug.o,$(basename $(SOURCES)))
ALL_OBJECTS=$(if $(DEBUG),$(OBJECTS_DEBUG),$(OBJECTS_RELEASE))
OBJECTS=$(filter-out %Main$(OBJ_EXT) %Test$(OBJ_EXT),$(ALL_OBJECTS))

MAIN_OBJECTS=$(filter %Main$(OBJ_EXT),$(ALL_OBJECTS))
BINARY_OBJECTS=$(filter %Test$(OBJ_EXT),$(ALL_OBJECTS))
TEST_BINARIES=$(notdir $(basename $(filter %Test.cpp,$(SOURCES))))

.PRECIOUS: %.o %.debug.o %Test
.SUFFIXES:
.PHONY: all compile debug release test checkstyle clean
.SECONDEXPANSION:

all: release

compile: libanlsim.so

debug: release

release: compile test checkstyle

test: $(addsuffix __run_test,$(TEST_BINARIES))

%__run_test: %
	./$<

checkstyle: $(SOURCES) $(HEADERS)
	$(LINTER) --repository=src $(SOURCES)
	$(LINTER) --repository=include $(HEADERS)

clean:
	rm -vf $(OBJECTS_RELEASE)
	rm -vf $(OBJECTS_DEBUG)
	rm -vf $(TEST_BINARIES)
	rm -vf libanlsim.so
	rm -vf example/libanlsim.so
	rm -vf bbtest/libanlsim.so
	rm -vf alarm/libanlsim.so

define bin-prereq
	$(filter %$(strip $(subst $(1),,$(2)))$(OBJ_EXT),$(BINARY_OBJECTS))
endef

libanlsim.so: $(OBJECTS) $(HEADERS) $(MAIN_OBJECTS)
	g++ $(FINAL_ARGS) -fPIC -shared -o $@ $(OBJECTS) $(MAIN_OBJECTS)
	cp -v $@ example/$@
	cp -v $@ bbtest/$@
	cp -v $@ alarm/$@

%Test: $$(call bin-prereq,~~INVALID,$$@) $(OBJECTS) $(HEADERS)
	g++ $(FINAL_ARGS) -o $@ $< $(OBJECTS) -lgtest -lgtest_main -lpthread

%.o %.debug.o: %.cpp $(HEADERS)
	g++ $(FINAL_ARGS) -fPIC $(INCLUDE_PATH) -c $< -o $@
