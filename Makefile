CC := gcc
CXX := g++
LD := ld
AS := as

CC_FLAGS :=-g

ifdef DEBUG
CC_FLAGS += -Og
else ifdef RELEASE
CC_FLAGS += -O3
endif

INCLUDE_DIR := include
CC_INCLUDE_FLAGS := -I$(INCLUDE_DIR)

SRC_DIR := src
# List of .c file names to compile (Does NOT include the src/ part!)
SRCS := $(wildcard $(SRC_DIR)/*.c)

TARGETS := $(patsubst $(SRC_DIR)/%,%,$(patsubst %.c,%,$(SRCS)))

BIN_DIR := bin
# Construct the relative path to each object file inside the build directory
# with each .c file having its extension replaced by .o
OBJS = $(patsubst %.c,%.o,$(SRCS))

CC_COMMAND := $(CC) $(CC_FLAGS) $(CC_INCLUDE_FLAGS) $(LD_INCLUDE_FLAGS)

.PHONY: all raw-eth-frame

all: $(TARGETS)
	@echo $(CC_COMMAND)

$(TARGETS): $(OBJS)
	@echo "Linking $@..."
	@echo "Making output binary directory in $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)
	$(CC_COMMAND) $^ -o $(BIN_DIR)/$@

# See gcc man(1) page for what these do.
DEP_FLAGS := -MD
MAGIC_SAUCE := -MP

# Dynamically creates a compile target based on the particular OBJ passed in
$(OBJS): %.o: %.c
	@echo "Compiling $@..."
	$(CC_COMMAND) $(DEP_FLAGS) $(MAGIC_SAUCE) -c $< -o $@

.PHONY: clean

clean: $(BIN_DIR)
	rm -rf $(BIN_DIR)
