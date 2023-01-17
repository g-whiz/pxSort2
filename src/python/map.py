from ._native import Map

from numba import types


def map_function_signature():
    return types.void(types.CPointer(types.float32), types.uint32,
                      types.CPointer(types.float32), types.uint32)
