.phony: dts install clean

TARGET_FS ?= /

all: dtb

gpio-key.dtbo: gpio-key.dts
	dtc -f -I dts -O dtb -o $@ $^

dt-blob.bin: dt-blob.dts
	dtc -I dts -O dtb -o $@ $^

dtb: dt-blob.bin gpio-key.dtbo

install: dt-blob.bin gpio-key.dtbo
	install -d $(TARGET_FS)/boot/
	cp -v dt-blob.bin $(TARGET_FS)/boot/
	cp -v gpio-key.dtbo $(TARGET_FS)/boot/overlays/

clean:
	-rm -v dt-blob.bin gpio-key.dtbo
