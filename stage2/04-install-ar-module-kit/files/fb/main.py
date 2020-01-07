from ctypes import *
from time import gmtime, strftime
import sys, time

class container(Structure):
    _fields_ = [
   ( "handle",                     c_int   ),
   ( "x",                          c_int   ),
   ( "y",                          c_int   ),
   ( "width",                      c_uint),
   ( "height",                     c_uint),
   ( "bits_per_pixel",             c_int   ),
   ( "padding",                    c_int   ),
   ( "transparent",                c_bool  ),
   ( "visible",                    c_bool  ),
   ( "frame",                      c_bool  ),
   ( "framesize_mem",              c_long  ),
   ( "bg_color",                   c_int   ),
   ( "description",                c_char_p),
   ( "(*data)[]",                  c_void_p)
   ]

class containers(Structure):
    _fields_ = [
   ( "n",                     c_int   ),
   ( "c",                     container * 10  ),
   ]

class lmx_lib():
    global lib
    def __init__(self):
        self.lib = CDLL("lib/liblmx.so")
        n = 0
        c = containers()
        c.c[n] = container(n,1,1,419,150,32,0,1,1,0,0,0,"Master Copy of FB",None)
        n += 1
        c.c[n] = container(n,1,1,419,70,32,0,1,1,0,0,0,"Clock frame",None)
        n += 1
        c.c[n] = container(n,1,71,419,65,32,0,1,1,0,0,0,"Build date",None)
        n += 1
        c.c[n] = container(n,419-159,1,159,47,32,0,1,1,0,0,0,"Python logo",None)
        n += 1
        c.c[n] = container(n,1,110,419,65,15,0,1,1,0,0,0,"Buttons",None)
        n += 1
        c.n = n
        self.lib.container_new(pointer(c))
        self.lib.text_init()

    def __del__(self):
        self.lib.container_close()
        self.lib.text_close()

class lmx_button(object):
    KEY_CHAR_LEFT = 105
    KEY_CHAR_RIGHT = 106

    def callback(self, presstime, key, handle):

        if key == self.KEY_CHAR_LEFT:
            s = "Left key"
        elif key == self.KEY_CHAR_RIGHT:
            s = "Right key"

        s2 = s + " {:03f} s".format(presstime)
        print(s2)
        self.lib.lib.container_reset(4)
        self.lib.lib.text_put(4, s2)

    def __init__(self, lib):
        CMPFUNC = CFUNCTYPE(None, c_double, c_int, c_int)
        self.button_callback = CMPFUNC(self.callback)
        self.lib = lib

        lib.lib.keys_register_cb(self.button_callback, self.KEY_CHAR_LEFT, 1, 0);
        lib.lib.keys_register_cb(self.button_callback, self.KEY_CHAR_RIGHT, 2, 1);

        lib.lib.keys_start()

    def __del__(self):
        lib.lib.keys_close()

class show_bmp():
    def __init__(self, lib, filename, layer):
        pyth_bmp = c_void_p()
        lib.bmp_open(filename, pointer(pyth_bmp))
        lib.container_fill_bitmap(3,pyth_bmp,1)
        lib.bmp_free(pointer(pyth_bmp))

def main():
    print("Starting LMX sample application")

    lmx = lmx_lib()
    button = lmx_button(lmx)
    lmx.lib.fwatch_start();

    t = cast(lmx.lib.version_date_get(), c_char_p).value

    lmx.lib.text_put(2, "Lib build\n" + t);

    show_bmp(lmx.lib, "graphics/python.bmp", 3)

    while True:
        try:
            timenow = strftime("%Y %m %d\n%H:%M:%S", gmtime())
            lmx.lib.text_put(1, timenow);
            lmx.lib.container_compile()
            lmx.lib.container_reset(1)
            lmx.lib.fwatch_do(1000)

        except KeyboardInterrupt:
            print("Cleaning up and closing down\n")
            sys.exit()

if __name__ == '__main__':
    main()
