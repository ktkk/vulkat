CC = gcc
CPPFLAGS = -std=c++17 -O2 -xc++
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lstdc++ -shared-libgcc

BUILD_DIR = build
SRC_DIR = src

OUTPUT = vulkat

SRCS = $(shell find $(SRC_DIR) -name *.cpp)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(OUTPUT): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

.PHONY: test clean

test: $(BUILD_DIR)/$(OUTPUT)
	$< -d

clean:
	rm -rf $(BUILD_DIR)
