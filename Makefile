CC = gcc
CPPFLAGS = -std=c++17 -O2 -xc++
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lstdc++ -shared-libgcc

BUILD_DIR = build
SRC_DIR = src

OUTPUT = vulkat

SRCS = $(shell find $(SRC_DIR) -name *.cpp)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

PCH_HEADER = $(SRC_DIR)/pch.hpp
PCH = $(PCH_HEADER).gch

$(BUILD_DIR)/$(OUTPUT): $(OBJS)
	@tput setaf 1; echo "\tBuilding output" && tput sgr0
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(PCH): $(PCH_HEADER)
	@tput setaf 1; echo "\tBuilding precompiled header" && tput sgr0
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(PCH)
	@tput setaf 1; echo "\tBuilding source files" && tput sgr0
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -include $(PCH_HEADER) -c $< -o $@ $(LDFLAGS)

.PHONY: test clean

test: $(BUILD_DIR)/$(OUTPUT)
	$< -d

clean:
	rm -rf $(BUILD_DIR)
