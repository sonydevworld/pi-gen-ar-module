# ROTATE_180 flag will rotate the whole screen 180 degrees, to be used when use left eye LMX-001 for right side eye.

#SETTING+=-DROTATE_180


#Gray scale calculation, either use Luminosity, Average or Monochrome, commenting out both will use RGB

#SETTING+=-DGRAYSCALE_LUMINOSITY
#SETTING+=-DGRAYSCALE_AVERAGE
#SETTING+=-DGREEN_MONOCHROME

#Extra linker flags, this will tell the linker to use the "local" liblmx.so file from ./lib/

#LDEXTRA=-Wl,-rpath,./lib/
