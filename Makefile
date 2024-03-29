UNAME=$(shell uname -s)
ifeq ($(UNAME),Linux)
	CC=gcc
	CPPFLAGS=-std=c++17 -O2 -x c++ -g
	LDFLAGS=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lstdc++ -shared-libgcc
endif
ifeq ($(UNAME),Darwin)
	CC=/usr/local/Cellar/gcc/10.2.0_4/bin/gcc-10 # use gnu gcc instead of mac alias to clang
	CPPFLAGS=-std=c++17 -O2 -x c++ -g -I/usr/local/include
	LDFLAGS=-L/usr/local/lib -lglfw -lvulkan -ldl -lpthread -lstdc++ -shared-libgcc
endif

BUILD_DIR=build
SRC_DIR=src

OUTPUT=vulkat

SRCS=$(shell find $(SRC_DIR) -name "*.cpp")
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

SHADER_DIR=$(shell find $(SRC_DIR) -type d -name "shaders")

PCH_HEADER=$(SRC_DIR)/pch.hpp
PCH=$(PCH_HEADER).gch

all: $(BUILD_DIR)/$(OUTPUT)

$(BUILD_DIR)/$(OUTPUT): $(OBJS)
	@tput setaf 1 ; echo -e "Building output" ; tput sgr0
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(PCH): $(PCH_HEADER)
	@tput setaf 1 ; echo -e "Building precompiled header" ; tput sgr0
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(PCH)
	@tput setaf 1 ; echo -e "Building source files" ; tput sgr0
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -include $(PCH_HEADER) -c $< -o $@

.PHONY: shaders clangd test clean

shaders:
	@tput setaf 1 ; echo -e "Building shaders" ; tput sgr0
	$(MAKE) -C $(SHADER_DIR)

clangd: clean
	bear -- make

test: $(BUILD_DIR)/$(OUTPUT) shaders
	$< -d

clean:
	rm -rf $(BUILD_DIR)
