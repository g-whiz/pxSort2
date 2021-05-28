from skbuild import setup
from Cython.Build import cythonize

setup(
    name='pxsort',
    ext_modules=cythonize(
        'image.pyx',
        sources=["Image.cpp"],
        language='c++'
    ),
    zip_safe=False
)
