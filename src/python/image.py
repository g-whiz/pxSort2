from typing import NamedTuple

from ._native import Image, ImageTopology


ImageContext = NamedTuple('ImageContext', [
    ('width', int),
    ('height', int),
    ('depth', int)
])
