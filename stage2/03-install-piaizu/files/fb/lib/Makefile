.phony: clean format install dbg doc all analyze

VERSION = $(shell git rev-parse HEAD)
DATE = $(shell date)

SOURCE = $(wildcard *.c)
OBJS   =  $(SOURCE:.c=.o)
HEADERS = $(wildcard ./*.h) $(wildcard ../cgi-src/*.h)

CFLAGS +=-Wall -std=c11 -D_DEFAULT_SOURCE -D_GNU_SOURCE  -Wextra -Werror -fPIC
CFLAGS += -DGITSHA='"$(VERSION)"' -DDATE='"$(DATE)"' -I../cgi-src/ $(DBG_FLAGS) $(SETTING)

include ../settings.mk

LDFLAGS = -shared
PREFIX ?= /

LIBLMX=liblmx.so

CC ?= gcc

all: $(LIBLMX)

dbg:
	$(MAKE)  $(LIBLMX) DBG_FLAGS="-DDBG -O0 -save-temps -g"

$(OBJS): $(SOURCE) $(HEADERS)

$(LIBLMX): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

format: $(SOURCE) $(HEADERS)
	clang-format -i $^

install: $(LIBLMX)
	install -vm 755 $(LIBLMX) $(PREFIX)/usr/lib/
	install -vm 444 *.h $(PREFIX)/usr/include/
	ldconfig

analyze: clean
	scan-build -analyze-headers -enable-checker alpha -enable-checker unix -enable-checker security -enable-checker deadcode -enable-checker nullability make

clean:
	-rm  $(LIBLMX) *.o *.i *.s
