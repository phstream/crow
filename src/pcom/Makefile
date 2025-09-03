# ##############################################################################
# \\  __   
# \ \(o >  
# \/ ) |   
#  // /    
#   || CROW - Communicatio Retis Omni Via
#
# FILE:         Makefile
# DESCRIPTION:  CROW PCOM Module Makefile.
# AUTHOR:       phstream
# COPYRIGHT:    2025, phstream
# LICENSE:      MIT
# DATE:         4 jul 2025
# ###############################################################################
BUILD_DIR   := build
INCL_DIRS   := . ..
TEST_DIR    := test

C_SRCS      := $(filter-out example_%.c, $(wildcard *.c))
CPP_SRCS    := $(wildcard *.cpp)
TEST_SRCS   := $(wildcard $(TEST_DIR)/test_*.c)

C_OBJS      := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
CPP_OBJS    := $(addprefix $(BUILD_DIR)/, $(CPP_SRCS:.cpp=.o))
LIBS        := $(addprefix lib, $(C_SRCS:.c=.so))
DLLS        := $(addprefix lib, $(C_SRCS:.c=.dll))
TEST_BINS   := $(addprefix $(BUILD_DIR)/, $(notdir $(TEST_SRCS:.c=)))

CC          := gcc
CXX         := g++
WCC         := x86_64-w64-mingw32-gcc # i686-w64-mingw32-gcc for 32 bit
CFLAGS      := -Wall -Wextra -O2 -fPIC $(addprefix -I, $(INCL_DIRS))
WCFLAGS     := $(addprefix -I, $(INCL_DIRS))
CXXFLAGS    := -Wall -Wextra -O2 -fPIC $(addprefix -I, $(INCL_DIRS))
LDFLAGS     :=

.PHONY: all libs dlls objs tests examples clean

# ===== Targets =====

all: libs objs

win: dlls

libs: $(LIBS)

dlls: $(DLLS)

objs: $(C_OBJS) $(CPP_OBJS)

tests: $(TEST_BINS)
	@echo "--- Running C/C++ Unittests ------------------------------------------"
	@for bin in $(TEST_BINS); do echo "Running $$bin"; ./$$bin; done
	@echo "--- Running Python Unittests -----------------------------------------"
	@python3 -m unittest discover -s $(TEST_DIR) -p "test_*.py"

examples: example_client example_server

example_client: example_client.c $(BUILD_DIR)/pcom.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

example_server: example_server.c $(BUILD_DIR)/pcom.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR) *.so *.dll
	find . -type d -name __pycache__ -exec rm -rf {} +
	rm -f example_client example_server

# ===== Rules =====

lib%.so: %.c
	$(CC) $(CFLAGS) -DBUILD_LIB -shared -o $@ $<

lib%.dll: %.c
	$(WCC) $(WCFLAGS) -DBUILD_LIB -shared -o $@ $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%: $(TEST_DIR)/test_%.c $(BUILD_DIR)/%.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Create build dir if missing
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

