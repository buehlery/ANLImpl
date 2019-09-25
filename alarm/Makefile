# License of this file can be seen in the LICENSE file.
#  Based on https://gitlab.com/snippets/1734324/raw
#  License of original can be found at https://gitlab.com/snippets/1734324

CXXARGS=-std=c++11 -Wl,-rpath -Wl,\$$ORIGIN
CXXARGS_DEBUG=-g -Werror -Wall -pedantic -DDEBUG
CXXARGS_RELEASE=-Wall -pedantic -O3 -DNDEBUG

LINTER=cpplint --filter=-legal/copyright

DEBUG=$(filter debug test,$(MAKECMDGOALS))
OBJ_EXT=$(if $(DEBUG),.debug.o,.o)
FINAL_ARGS=$(CXXARGS) $(if $(DEBUG),$(CXXARGS_DEBUG),$(CXXARGS_RELEASE))
INCLUDE_PATH=-I../include -Isrc

SOURCES=$(shell find src -name "*.cpp")
HEADERS=$(shell find src -name "*.h")
OBJECTS_RELEASE=$(addsuffix .o,$(basename $(SOURCES)))
OBJECTS_DEBUG=$(addsuffix .debug.o,$(basename $(SOURCES)))
ALL_OBJECTS=$(if $(DEBUG),$(OBJECTS_DEBUG),$(OBJECTS_RELEASE))
OBJECTS=$(filter-out %Main$(OBJ_EXT) %Test$(OBJ_EXT),$(ALL_OBJECTS))
LINTER_TARGETS=$(SOURCES) $(HEADERS)

BINARY_OBJECTS=$(filter %Main$(OBJ_EXT) %Test$(OBJ_EXT),$(ALL_OBJECTS))
RAW_BINARIES=$(notdir $(basename $(filter %Main.cpp,$(SOURCES))))
DBG_BINARIES=$(addsuffix _debug,$(RAW_BINARIES))
MAIN_BINARIES=$(if $(DEBUG),$(DBG_BINARIES),$(RAW_BINARIES))

.PRECIOUS: %.o %.debug.o
.SUFFIXES:
.PHONY: all compile debug release checkstyle clean
.SECONDEXPANSION:

all: release

compile: $(MAIN_BINARIES)

debug: release

release: compile checkstyle

checkstyle: $(SOURCES) $(HEADERS)
	$(LINTER) --repository=src $(SOURCES) $(HEADERS)

clean:
	rm -vf $(OBJECTS_RELEASE)
	rm -vf $(OBJECTS_DEBUG)
	rm -vf $(RAW_BINARIES)
	rm -vf $(DBG_BINARIES)

define bin-prereq
	$(filter %$(strip $(subst $(1),,$(2)))$(OBJ_EXT),$(BINARY_OBJECTS))
endef

%Main_debug: $$(call bin-prereq,_debug,$$@) $(OBJECTS) $(HEADERS)
	g++ $(FINAL_ARGS) -L.. -o $@ $< $(OBJECTS) -lanlsim

%Main: $$(call bin-prereq,~~INVALID,$$@) $(OBJECTS) $(HEADERS)
	g++ $(FINAL_ARGS) -L.. -o $@ $< $(OBJECTS) -lanlsim

%.o %.debug.o: %.cpp $(HEADERS)
	g++ $(FINAL_ARGS) $(INCLUDE_PATH) -c $< -o $@
