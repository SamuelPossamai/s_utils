
BINARY_PATH = bin
TARGET = $(BINARY_PATH)/$(notdir $(basename $(TEST)))

SOURCE_PATH = src
HEADER_PATH = include

TEST_PATH = tests

INCLUDE_PATH = include
LIBRARY_PATH =

LIBRARY_FILES =


CPP_DEPENDENCIES = true
WERROR_SET = true

ifeq ($(TARGET),)
TARGET = a.out
endif

ifndef CXX_VERSION
CXX_VERSION = 11
endif

ifndef OPTIMIZATION
OPTIMIZATION = 2
endif

CXX := g++
CXXFLAGS := -Wall -Wshadow -Wextra
LDFLAGS := 
DBG := gdb
DBGFLAGS := -g -O0
MCK := valgrind
EDITOR := kate

TEST_FILE = $(TEST_PATH)/$(TEST)_test.cpp

DEPS := $(shell find $(HEADER_PATH) -type f -name '*.h')
ifeq ($(CPP_DEPENDENCIES), true)
DEPS += $(shell find $(HEADER_PATH) -type f -name '*.cpp')
endif
SOURCE := $(shell find $(SOURCE_PATH) -type f -name '*.cpp')
ifdef TEST
SOURCE += $(TEST_FILE)
endif
CODE := $(patsubst %.cpp, %.o, $(SOURCE))

ifeq ($(shell [ -f Makedefs ] && echo y),y)
include Makedefs
endif

CXXFLAGS += -std=c++$(CXX_VERSION) -O$(OPTIMIZATION) $(addprefix -I, $(INCLUDE_PATH))
LDFLAGS += $(addprefix -L, $(LIBRARY_PATH)) $(addprefix -l, $(LIBRARY_FILES))

ifeq ($(WERROR_SET), true)
CXX_FLAGS += -Werror
endif

.PHONY: all compile run open debug debug_flags memcheck clean compile_check create_dir_if_needed

all: compile

compile: compile_check create_dir_if_needed $(TARGET)

run: compile
	./$(TARGET) $(ARGS)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(CODE)
	$(CXX) -o $@ $^ $(LDFLAGS)

open:
	@$(EDITOR) $(SOURCE) $(DEPS) &

debug: debug_flags compile
	$(DBG) ./$(TARGET) $(ARGS)

debug_flags:
	$(eval CXXFLAGS += $(DBGFLAGS))

compile_check:
ifndef TEST
	$(error TEST file not specified)
endif

memcheck: compile
	$(MCK) ./$(TARGET) $(ARGS)

clean:
	find . -type f -executable -exec rm {} +
	find . -type f -name '*.o' -exec rm {} +

create_dir_if_needed:
	@mkdir -p $(SOURCE_PATH) $(HEADER_PATH) $(TEST_PATH) $(BINARY_PATH)

