.phony: dts install clean

TARGET_FS ?= /

all: dtb

gpio-key.dtbo: gpio-key.dts
	dtc -f -I dts -O dtb -o $@ $^

ar-display.dtbo: ar-display.dts
	dtc -f -I dts -O dtb -o $@ $^

dtb: ar-display.dtbo gpio-key.dtbo

install: ar-display.dtbo gpio-key.dtbo
	install -d $(TARGET_FS)/boot/
	cp -v gpio-key.dtbo $(TARGET_FS)/boot/overlays/
	cp -v ar-display.dtbo $(TARGET_FS)/boot/overlays/

clean:
	-rm -v gpio-key.dtbo
	-rm -v ar-display