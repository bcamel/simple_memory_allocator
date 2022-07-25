TARGET_EXEC := alloc_driver
BUILD_DIR := ./build
SRC_DIR := ./src

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
ADDITIONAL_CXXFLAGS := $(INC_FLAGS) -MMD -MP -D_GNU_SOURCE -std=c11 -Wall -Wextra -Werror -pedantic -ggdb
LDLIBS := -pthread

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(ADDITIONAL_CXXFLAGS) $(CFLAGS) -c $< -o $@

all: $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	@test -d $(BUILD_DIR) && ( rm -r $(BUILD_DIR) ; echo "removed $(BUILD_DIR)" ) || echo "no such dir: $(BUILD_DIR)"

.PHONY: all clean

-include $(DEPS)