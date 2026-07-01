PROJECT_NAME ?= entry

SRC_DIR      ?= src
OBJ_DIR      ?= build/obj
BUILD_DIR    ?= build
INCLUDE_DIRS ?= include include/vendor/uWebSockets/uSockets
LIBS_DIR     ?= libs

CXX ?= g++

CPPFLAGS := $(addprefix -I,$(INCLUDE_DIRS))
CXXFLAGS ?= -std=c++23 -Wall -Wextra -O2 -MMD -MP

LIBS := \
	-Wl,--start-group \
	$(LIBS_DIR)/libpqxx.a \
	$(LIBS_DIR)/libpq.a \
	$(LIBS_DIR)/libpgcommon_shlib.a \
	$(LIBS_DIR)/libpgport_shlib.a \
	$(LIBS_DIR)/uSockets.a \
	$(LIBS_DIR)/libsodium.a \
	$(LIBS_DIR)/libssl.a \
	$(LIBS_DIR)/libcrypto.a \
	-Wl,--end-group

LDFLAGS ?= -lz -lm -lpthread -ldl

SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := $(BUILD_DIR)/$(PROJECT_NAME)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

