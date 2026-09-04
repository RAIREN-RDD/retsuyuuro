PROJECT_NAME := entry

SRC_DIR     := src
INCLUDE_DIR := include
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj

CCACHE := ccache
CXX ?= g++

COMPILER := $(CCACHE) $(CXX)

CXX_FLAGS := -std=c++26 -Wall


# ---   DEPENDENCIES   --- #

NLOHMANN_INCLUDE_DIR := $(RAIREN_SDK_CPP23_INCLUDE)/nlohmann/v3.12.0-55f9368

UWEBSOCKETS_INCLUDE_DIR := $(RAIREN_SDK_CPP23_INCLUDE)/uWebSockets/v29.79.0-fe7c01a

USOCKETS_VERSION := v0.8.8-17-g86097c4
USOCKETS_INCLUDE_DIR := $(RAIREN_SDK_C11_INCLUDE)/uSockets/$(USOCKETS_VERSION)/uSockets
USOCKETS_LIB_DIR := $(RAIREN_SDK_C11_LIB)/uSockets/$(USOCKETS_VERSION)
USOCKETS_LIB := uSockets

RAIREN_VERSION := v1.0.0
RAIREN_INCLUDE_DIR := $(RAIREN_SDK_CPP26_INCLUDE)/rairen/$(RAIREN_VERSION)

# --- DEPENDENCIES END --- #


INCLUDE := \
	-I$(INCLUDE_DIR)                     \
	-isystem $(NLOHMANN_INCLUDE_DIR)     \
	-isystem $(UWEBSOCKETS_INCLUDE_DIR)  \
	-isystem $(USOCKETS_INCLUDE_DIR)     \
	-isystem $(RAIREN_INCLUDE_DIR)

LD_FLAGS := \
	-L$(USOCKETS_LIB_DIR) \
	-l$(USOCKETS_LIB) \
	-Wl,-rpath,$(USOCKETS_LIB_DIR)


# ---   SOURCES   --- #

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET := $(BUILD_DIR)/$(PROJECT_NAME)

# --- SOURCES END --- #


.PHONY: all clean run

all: $(TARGET)


$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(COMPILER) -o $@ $^ $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(COMPILER) $(CXX_FLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) compile_commands.json

run: $(TARGET)
	cd $(BUILD_DIR) && ./$(PROJECT_NAME)

