cdef extern from "Image.h" namespace "pxsort":
    cdef enum ColorSpace "Image::ColorSpace":
        RGB = 0,
        XYZ = 1,
        Lab = 2,
        Luv = 3,
        YCrCb = 4,
        HSV = 5,
        HLS = 6

    cdef cppclass Image:
        Image(int, int, ColorSpace, unsigned char*) except +
