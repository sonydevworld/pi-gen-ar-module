.phony: clean format install dbg all analyze

VERSION = $(shell git rev-parse HEAD)
DATE = $(shell date)

SOURCE = lmxjson.c
OBJS   =  $(SOURCE:.c=.o)
HEADERS = $(wildcard *.h)

CFLAGS +=-Wall  -std=c11 -D_DEFAULT_SOURCE -D_GNU_SOURCE -pthread -Wextra -Werror -save-temps -I../src
CFLAGS += -DGITSHA='"$(VERSION)"' -DDATE='"$(DATE)"' $(DBG_FLAGS)

LDFLAGS = -lfcgi -ljansson
PREFIX ?= /

CC ?= gcc

all: lmx.cgi

dbg:
	$(MAKE) lmx.cgi DBG_FLAGS="-DDBG -O0 -save-temps -g"

$(OBJS): $(SOURCE) $(HEADERS)

lmx.cgi: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

format: $(SOURCE) $(HEADERS)
	clang-format -i $^

install: clean lmx.cgi
	-mkdir -p $(PREFIX)/var/www/html/cgi-bin/
	cp -rfv lmxindex.html $(PREFIX)/var/www/html/
	install -m 755 lmx.cgi $(PREFIX)/var/www/html/cgi-bin/

analyze: clean
	scan-build -analyze-headers -enable-checker alpha -enable-checker unix -enable-checker security -enable-checker deadcode -enable-checker nullability make

clean:
	-rm lmx.cgi *.o *.i *.s

