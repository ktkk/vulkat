CC = gcc
CFLAGS = -std=c++17 -O2 -xc++
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lstdc++ -shared-libgcc

OUTPUT = vulkat
OBJS = main.o core.o window.o

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

main.o: $(shell find -type f -name "main.cpp")
	$(CC) $(CFLAGS) -c $(shell find -type f -name "main.cpp")

core.o: $(shell find -type f -name "core.cpp") $(shell find -type f -name "core.hpp")
	$(CC) $(CFLAGS) -c $(shell find -type f -name "core.cpp")

window.o: $(shell find -type f -name "window.cpp") $(shell find -type f -name "window.hpp")
	$(CC) $(CFLAGS) -c $(shell find -type f -name "window.cpp")

.PHONY: test clean

test: $(OUTPUT)
	./$(OUTPUT)

clean:
	rm -f *.o $(OUTPUT)
