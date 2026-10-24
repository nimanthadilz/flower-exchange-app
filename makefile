# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := app

BUILD_DIR := ./build
SRC_DIRS := ./src
TEST_OUT_DIR := $(BUILD_DIR)/tests
LOCAL_TEST_DIR := data/local_tests
SYSTEM_TEST_DIR := data/system_tests

MAIN_BIN := $(BUILD_DIR)/$(TARGET_EXEC)
TEST_BIN := $(BUILD_DIR)/test

LOCAL_TEST_NAMES := $(sort $(patsubst %_answer.csv,%,$(notdir $(wildcard $(LOCAL_TEST_DIR)/*_answer.csv))))
LOCAL_TEST_OUTPUT_FILES := $(addprefix $(TEST_OUT_DIR)/local/,$(addsuffix _output.csv,$(LOCAL_TEST_NAMES)))
LOCAL_TEST_STAMPS := $(addprefix $(TEST_OUT_DIR)/local/,$(addsuffix .passed,$(LOCAL_TEST_NAMES)))
SYSTEM_TEST_NAMES := $(sort $(patsubst %_answer.csv,%,$(notdir $(wildcard $(SYSTEM_TEST_DIR)/*_answer.csv))))
SYSTEM_TEST_OUTPUT_FILES := $(addprefix $(TEST_OUT_DIR)/system/,$(addsuffix _output.csv,$(SYSTEM_TEST_NAMES)))
SYSTEM_TEST_STAMPS := $(addprefix $(TEST_OUT_DIR)/system/,$(addsuffix .passed,$(SYSTEM_TEST_NAMES)))

LOCAL_TEST_INPUT_FOR = $(firstword $(wildcard $(LOCAL_TEST_DIR)/$1_input_*.csv))
SYSTEM_TEST_INPUT_FOR = $(firstword $(wildcard $(SYSTEM_TEST_DIR)/$1_input_*.csv))

.SECONDEXPANSION:
.SECONDARY: $(LOCAL_TEST_OUTPUT_FILES) $(SYSTEM_TEST_OUTPUT_FILES)

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP -g
CXXFLAGS := -std=c++17
LDFLAGS := -lpthread

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: all binaries test test-local test-system clean

all: binaries

binaries: $(MAIN_BIN) $(TEST_BIN)

test-local: $(LOCAL_TEST_STAMPS)
	@echo All local tests passed

test-system: $(SYSTEM_TEST_STAMPS)
	@echo All system tests passed

test: test-local test-system
	@echo "All tests passed."

$(TEST_BIN): tests/test.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(TEST_OUT_DIR)/local:
	mkdir -p $@

$(TEST_OUT_DIR)/system:
	mkdir -p $@

$(TEST_OUT_DIR)/local/%_output.csv: $$(call LOCAL_TEST_INPUT_FOR,$$*) $(MAIN_BIN) | $(TEST_OUT_DIR)/local
	$(MAIN_BIN) $< $@

$(TEST_OUT_DIR)/system/%_output.csv: $$(call SYSTEM_TEST_INPUT_FOR,$$*) $(MAIN_BIN) | $(TEST_OUT_DIR)/system
	$(MAIN_BIN) $< $@

$(TEST_OUT_DIR)/local/%.passed: $(TEST_OUT_DIR)/local/%_output.csv $(LOCAL_TEST_DIR)/%_answer.csv $(TEST_BIN)
	$(TEST_BIN) $(TEST_OUT_DIR)/local/$*_output.csv $(LOCAL_TEST_DIR)/$*_answer.csv
	@touch $@

$(TEST_OUT_DIR)/system/%.passed: $(TEST_OUT_DIR)/system/%_output.csv $(SYSTEM_TEST_DIR)/%_answer.csv $(TEST_BIN)
	$(TEST_BIN) $(TEST_OUT_DIR)/system/$*_output.csv $(SYSTEM_TEST_DIR)/$*_answer.csv
	@touch $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
