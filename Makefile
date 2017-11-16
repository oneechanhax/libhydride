CC=$(shell sh -c "which gcc-7 || which gcc")
CFLAGS=-O3 -Wall -fPIC -fmessage-length=0 -D_GNU_SOURCE=1 -g3 -ggdb -Iinclude -isystemftgl -isystem/usr/local/include/freetype2 -isystem/usr/include/freetype2
LDFLAGS=-shared -Wl,--no-undefined
LDLIBS=-lm -lX11 -lXext -lrt -lpthread -lXfixes -lGL -lfreetype -lGLEW -lpng
SRC_DIR=src
BIN32_DIR=bin32
BIN64_DIR=bin64
SOURCES=$(shell find $(SRC_DIR) -name "*.c" -print)
SOURCES+=$(shell find "ftgl" -name "*.c" -print)
OBJECTS=$(SOURCES:.c=.o)
DEPENDS=$(SOURCES:.c=.d)

TARGET32=$(BIN32_DIR)/liboverlay.so
TARGET64=$(BIN64_DIR)/liboverlay.so
TARGET=undefined

.PHONY: clean clean_objects directories

ifeq ($(ARCH),32)
CFLAGS+=-m32
LDFLAGS+=-m32
TARGET=$(TARGET32)
endif
ifeq ($(ARCH),64)
TARGET=$(TARGET64)
endif

all:
	mkdir -p $(BIN32_DIR)
	mkdir -p $(BIN64_DIR)
ifndef ARCH
	$(MAKE) clean_objects
	$(MAKE) $(TARGET64) -e ARCH=64
	$(MAKE) clean_objects
	$(MAKE) $(TARGET32) -e ARCH=32
endif
	
install:
	cp $(TARGET64) /lib/x86_64-linux-gnu
	cp $(TARGET32) /lib/i386-linux-gnu

ftgl/distance-field.o : CFLAGS+=-w
ftgl/edtaa3func.o : CFLAGS+=-w
ftgl/font-manager.o : CFLAGS+=-w
ftgl/mat4.o : CFLAGS+=-w
ftgl/platform.o : CFLAGS+=-w
ftgl/shader.o : CFLAGS+=-w
ftgl/text-buffer.o : CFLAGS+=-w
ftgl/texture-atlas.o : CFLAGS+=-w
ftgl/utf8-utils.o : CFLAGS+=-w
ftgl/texture-font.o : CFLAGS+=-w
ftgl/vector.o : CFLAGS+=-w
ftgl/vertex-attribute.o : CFLAGS+=-w
ftgl/vertex-buffer.o : CFLAGS+=-w
ftgl/makefont.o : CFLAGS+=-w

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.c.d:
	@$(CC) -M $(CXXFLAGS) $< > $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean_objects:
	find . -type f -name '*.o' -delete

clean:
	find . -type f -name '*.o' -delete
	find . -type f -name '*.d' -delete
	rm -f bin32/*.so
	rm -f bin64/*.so
	
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif