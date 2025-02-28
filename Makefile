# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
# Define the C++ compiler to use
CXX = g++

# Define any compile-time flags
CXXFLAGS := -std=c++17 -Wall -Wextra -g

# Define library paths in addition to /usr/lib
# If you want to include libraries not in /usr/lib, specify
# their path using -Lpath, something like:
LFLAGS = -lmingw32 -lSDL3

# Define output directory
OUTPUT := output

# Define source directory
SRC := src

# Define include directory
INCLUDE := include

# Define lib directory
LIB := lib

# Define glad.c file path
GLAD_C_FILE := src/glad.c

ifeq ($(OS),Windows_NT)
    MAIN := main.exe
    SOURCEDIRS := $(SRC)
    INCLUDEDIRS := $(INCLUDE)
    LIBDIRS := $(LIB)
    FIXPATH = $(subst /,\,$1)
    RM := cmd /c del /q /f
    MD := mkdir
else
    MAIN := main
    SOURCEDIRS := $(shell find $(SRC) -type d)
    INCLUDEDIRS := $(shell find $(INCLUDE) -type d)
    LIBDIRS := $(shell find $(LIB) -type d)
    FIXPATH = $1
    RM = rm -f
    MD := mkdir -p
endif

# Define any directories containing header files other than /usr/include
INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# Define the C libs
LIBS := $(patsubst %,-L%, $(LIBDIRS:%/=%))

# Define the C source files
SOURCES := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))

# Define the C object files
OBJECTS := $(SOURCES:.cpp=.o)

# Include glad.c as an object file
OBJECTS += $(GLAD_C_FILE:.c=.o)

# Define the dependency output files
DEPS := $(OBJECTS:.o=.d)

# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'

OUTPUTMAIN := $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LFLAGS) $(LIBS)

# Include all .d files
-include $(DEPS)

# This is a suffix replacement rule for building .o's and .d's from .c's
# It uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# -MMD generates dependency output files with the same name as the .o file
# (see the GNU make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@

# Special rule for compiling .c files (like glad.c)
.c.o:
	gcc $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS:.c=.o))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!
