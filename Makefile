CXX=$(shell sh -c "which gcc-7 || which gcc")
CXXFLAGS=-O3 -Wall -fPIC -fmessage-length=0 -D_GNU_SOURCE=1 -g3 -ggdb -Iinclude
LDFLAGS=-shared
LDLIBS=-lm -lX11 -lXext -lrt -lpthread -lXfixes
SRC_DIR=src
BIN32_DIR=bin32
BIN64_DIR=bin64
SOURCES=$(shell find $(SRC_DIR) -name "*.c" -print)
OBJECTS=$(SOURCES:.c=.o)
DEPENDS=$(SOURCES:.c=.d)

TARGET32=$(BIN32_DIR)/liboverlay.so.0
TARGET64=$(BIN64_DIR)/liboverlay.so.0
TARGET=undefined

.PHONY: clean directories

ifeq ($(ARCH),32)
CXXFLAGS+=-m32
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
	$(MAKE) clean
	$(MAKE) $(TARGET64) -e ARCH=64
	$(MAKE) clean
	$(MAKE) $(TARGET32) -e ARCH=32
endif
	
install:
	cp $(TARGET64) /lib/x86_64-linux-gnu
	cp $(TARGET32) /lib/i386-linux-gnu
	
.c.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean:
	find . -type f -name '*.o' -delete