CC = clang

CCFLAGS =
LDFLAGS =

INCFLAGS = -iquotesrc -I.

CCFLAGS += -std=gnu2x
CCFLAGS += -g
CCFLAGS += -O2
CCFLAGS += -fsanitize=undefined,address,leak
CCFLAGS += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs)

all: build

build:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o main main.c
