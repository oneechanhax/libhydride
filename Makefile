CC=$(shell sh -c "which gcc-7 || which gcc")
CFLAGS=-O3 -Wall -fPIC -fmessage-length=0 -D_GNU_SOURCE=1 -g3 -ggdb -Iinclude 
LDFLAGS=-shared -Wl,--no-undefined
LDLIBS=-lm -lX11 -lXext -lrt -lpthread -lXfixes -lGL -lGLEW
SRC_DIR=src
BIN32_DIR=bin32
BIN64_DIR=bin64
SOURCES=$(shell find $(SRC_DIR) -name "*.c" -print)
OBJECTS=$(SOURCES:.c=.o)

TARGET32=$(BIN32_DIR)/libxoverlay.so
TARGET64=$(BIN64_DIR)/libxoverlay.so
TARGET=undefined

.PHONY: clean clean_objects

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
	$(MAKE) -e ARCH=32
	$(MAKE) clean_objects
	$(MAKE) -e ARCH=64
else
	$(MAKE) clean_objects
	$(MAKE) $(TARGET)
endif
	
install:
	cp $(TARGET32) /lib/i386-linux-gnu
	cp $(TARGET64) /lib/x86_64-linux-gnu

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean_objects:
	find . -type f -name '*.o' -delete

clean:
	find . -type f -name '*.d' -delete
	rm -f bin32/*.so
	rm -f bin64/*.so
