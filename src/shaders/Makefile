SC = glslc

VERT_SHDR = shader.vert
FRAG_SHDR = shader.frag

all: vert.spv frag.spv

vert.spv:
	$(SC) $(VERT_SHDR) -o $@

frag.spv:
	$(SC) $(FRAG_SHDR) -o $@

.PHONY: clean

clean:
	rm -rf vert.spv frag.spv
