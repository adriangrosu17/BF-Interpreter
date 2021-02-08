TARGET_WIN ?= n
ifeq ($(TARGET_WIN), y)
	CC = x86_64-w64-mingw32-gcc
	CXX = x86_64-w64-mingw32-g++
	EXT = exe
else
	CC = gcc
	CXX = g++
	EXT = elf
endif

TARGET_EXEC ?= interpreter.$(EXT)
BUILD_DIR ?= build
SRC_DIRS ?= src
DEBUG ?= n
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS += inc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CFLAGS ?= -Wall -Wextra -Werror -pedantic -std=c11
ifeq ($(DEBUG), y)
	CFLAGS += -g
endif
LDFLAGS += -Xlinker -Map=$(BUILD_DIR)/$(TARGET_EXEC).map
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
		$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
-include $(DEPS)

MKDIR_P ?= mkdir -p

