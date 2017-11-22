.phony: clean format install dbg doc all analyze liblmx cgi

include settings.mk

VERSION = $(shell git rev-parse HEAD)
DATE = $(shell date)

SOURCE = $(wildcard *.c)
OBJS   =  $(SOURCE:.c=.o)
HEADERS = $(wildcard lib/*.h) $(wildcard cgi-src/*.h) $(wildcard ./*.h)

CFLAGS +=-Wall -std=c11 -D_DEFAULT_SOURCE -D_GNU_SOURCE  -Wextra -Werror
CFLAGS += -DGITSHA='"$(VERSION)"' -DDATE='"$(DATE)"' -I./cgi-src/ -I./lib/ $(DBG_FLAGS)

LDFLAGS = -L./lib/ -llmx $(LDEXTRA)
PREFIX ?= /

CC ?= gcc

all: liblmx cgi lmx

liblmx:
	$(MAKE) -C ./lib/ all

cgi:
	$(MAKE) -C ./cgi-src/ all

dbg:
	$(MAKE) -C ./cgi-src/ $@
	$(MAKE) -C ./lib/ $@
	$(MAKE)  lmx DBG_FLAGS="-DDBG -O0 -save-temps -g"

$(OBJS): $(SOURCE) $(HEADERS)

lmx: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

format: $(SOURCE) $(HEADERS)
	clang-format -i $^

$(PREFIX)/usr/bin/graphics/:
	mkdir -p $@

install: clean all $(PREFIX)/usr/bin/graphics/
	$(MAKE) -C ./lib/ install  PREFIX="$(PREFIX)"
	$(MAKE) -C ./cgi-src/ install  PREFIX="$(PREFIX)"
	cp -rfv graphics/* $(PREFIX)/usr/bin/graphics/
	install -m 755 lmx $(PREFIX)/usr/bin
	install -m 755 lmx001 $(PREFIX)/etc/init.d
	update-rc.d lmx001 defaults
	update-rc.d lmx001 enable

analyze: clean
	scan-build -analyze-headers -enable-checker alpha -enable-checker unix -enable-checker security -enable-checker deadcode -enable-checker nullability $(MAKE)

doc:
	doxygen doxy.cfg

clean:
	-rm  lmx *.o *.i *.s
	$(MAKE) -C ./lib/ $@
	$(MAKE) -C ./cgi-src/ $@
