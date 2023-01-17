from typing import NamedTuple

from ._native import Image


ImageContext = NamedTuple('ImageContext', [
    ('width', int),
    ('height', int),
    ('depth', int)
])
