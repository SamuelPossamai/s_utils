
BINARY_PATH = bin
ifdef TEST
TARGET = $(BINARY_PATH)/$(notdir $(basename $(TEST)))
else
TARGET = $(BINARY_PATH)/sutils
endif

SOURCE_PATH = src
HEADER_PATH = include

TEST_PATH = tests

INCLUDE_PATH = include
LIBRARY_PATH =

LIBRARY_FILES =

CPP_DEPENDENCIES = true
WERROR_SET = true

ifndef CXX_VERSION
CXX_VERSION = 1z
endif

ifndef OPTIMIZATION
OPTIMIZATION = 2
endif

CXX := g++
CXXFLAGS := -Wall -Wshadow
LDFLAGS :=
DBG := gdb
DBGFLAGS := -g -O0
MCK := valgrind

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

ifeq ($(WERROR_SET),true)
CXX_FLAGS += -Werror
endif

CXXFLAGS += $(ADD_CXXFLAGS)
LDFLAGS += $(ADD_LDFLAGS)

LIB_TARGET := $(dir $(TARGET))lib$(notdir $(TARGET))

.PHONY: all compile run open debug debug_flags memcheck clean compile_check create_dir_if_needed static_lib create_dyn_lib dyn_lib add_dyn_lib_flag

all: compile

ifdef TEST
compile: create_dir_if_needed $(TARGET)
else
compile: create_dir_if_needed static_lib
endif

run: compile
	./$(TARGET) $(ARGS)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(CODE)
	$(CXX) -o $@ $^ $(LDFLAGS)

debug: debug_flags compile
	$(DBG) ./$(TARGET) $(ARGS)

debug_flags:
	$(eval CXXFLAGS += $(DBGFLAGS))

memcheck: compile
	$(MCK) ./$(TARGET) $(ARGS)

clean:
	find $(BINARY_PATH) -type f -executable -exec rm {} +
	find $(BINARY_PATH) -type f -name '*.a' -exec rm {} +
	find $(BINARY_PATH) -type f -name '*.so' -exec rm {} +
	find . -type f -name '*.o' -exec rm {} +

create_dir_if_needed:
	@mkdir -p $(SOURCE_PATH) $(HEADER_PATH) $(TEST_PATH) $(BINARY_PATH)

static_lib: $(CODE)
	ar rcs $(LIB_TARGET).a $^

create_dyn_lib: $(CODE)
	gcc -shared -o $(LIB_TARGET).so $^

dyn_lib: add_dyn_lib_flag create_dyn_lib

add_dyn_lib_flag:
	$(eval CXXFLAGS += -fPIC)
