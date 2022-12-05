from skbuild import setup

setup(
    setup_requires=['cmake'],
    packages=['pxsort'],
    package_dir={'pxsort': 'src/python'},
    cmake_install_dir="src",
    cmake_args=['-DCMAKE_INSTALL_DIR=.']
)