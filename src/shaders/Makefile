SC = glslc

VERT_SHDR = shader.vert
FRAG_SHDR = shader.frag

OUT_DIR = ../../build/src/$(notdir $(CURDIR))

all: vert.spv frag.spv

vert.spv:
	mkdir -p $(dir $(OUT_DIR)/$@)
	$(SC) $(VERT_SHDR) -o $(OUT_DIR)/$@

frag.spv:
	mkdir -p $(dir $(OUT_DIR)/$@)
	$(SC) $(FRAG_SHDR) -o $(OUT_DIR)/$@

.PHONY: clean

clean:
	rm -rf $(OUT_DIR)
