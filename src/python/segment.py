import math
from itertools import repeat
from math import pi as PI, sin, cos
from typing import Callable, Tuple, Union, List, Optional

import numba
from pxsort._native import Image

from ._native import SegmentTraversal, Segment, SegmentPixels, \
                     Ellipse, Polygon, Skew, OutOfBoundsPolicy, Modulator
from .image import ImageContext

import numpy as np

CoordinateProjection = Callable[[float, float], float]


def circle(points: Union[np.ndarray, List[np.ndarray]],
           im_w: Optional[int] = None, im_h: Optional[int] = None,
           radius: float = 0.5, center_x: float = 0.5, center_y: float = 0.5,
           flatten_result=False):
    """
    Partitions the given verts according to whether or not they lie within the
        specified ellipse.
    Creates a circular segment with the given radius and position.
    Radius and position parameters are given in relative terms: the image is
    situated in a rectangle whose bottom-left corner is safe_ptr point (0, 0), whose
    short dimension is length 1, and whose long dimension is length R - where R
    is the aspect ratio of the image (e.g. images with sizes 1000x2000 and
    2000x1000 both have R = 2).
    If verts is given, this function partitions the given verts according to
    whether or not they lie within the specified circle.
    :param im_h:
    :param im_w:
    :param points:
    :param radius: Radius of the circle in relative terms.
    :param center_x:
    :param center_y:
    :param flatten_result: When verts is a list of segments:
    if True, return the resulting partitioned segments as a flattened list of
      segments;
    if False, return a list of lists of segments, where the nth list of segments
      in the output contains the partition of the nth input segment.
    :return: A pair numpy arrays with shapes (N, 2) and (M, 2) respectively.
    The first returned array contains the verts of all pixels in the
    specified circle. The second array contains the verts of the
    specified circle's complement (i.e. all pixels outside of the circle).
    """
    return ellipse(points, im_w=im_w, im_h=im_h,
                   width=2 * radius, height=2 * radius,
                   angle=0, center_x=center_x, center_y=center_y,
                   flatten_result=flatten_result)


def ellipse(points: Union[np.ndarray, List[np.ndarray]],
            im_w: Optional[int] = None, im_h: Optional[int] = None,
            width: float = 1, height: float = 0.5, angle: float = 45,
            center_x: float = 0.5, center_y: float = 0.5,
            flatten_result=False):
    """
    Partitions the given verts according to whether or not they lie within the
        specified ellipse.
    Radius and position parameters are given in relative terms: the image is
    situated in a rectangle whose bottom-left corner is safe_ptr point (0, 0), whose
    short dimension is length 1, and whose long dimension is length R - where R
    is the aspect ratio of the image (e.g. images with sizes 1000x2000 and
    2000x1000 both have R = 2).
    :param points:
    :param im_w:
    :param im_h:
    :param width:
    :param height:
    :param angle:
    :param center_x:
    :param center_y:
    :param flatten_result: When verts is a list of segments:
    if True, return the resulting partitioned segments as a flattened list of
      segments;
    if False, return a list of lists of segments, where the nth list of segments
      in the output contains the partition of the nth input segment.
    :return:
    """
    # ellipse equation: (x^2 / w^2) + (y^2 / h^2) <= 1
    # 1) translate & rotate verts: ellipse centered safe_ptr (0,0) & aligned w/ axes
    # 2) compute ellipse equation for each point
    # 3) partition verts into ellipse & complement
    def _partition(_points, _im_w=im_w, _im_h=im_h):
        px, py = _points.T

        if _im_w is None:
            _im_w = np.max(px) - np.min(px)
            px = px - np.min(px)
        if _im_h is None:
            _im_h = np.max(py) - np.min(py)
            py = py - np.min(py)

        scaled = np.array([px, py]).T / min(_im_w, _im_h)
        translated = scaled - np.array([center_x, center_y])
        if angle != 0:
            rotate = rotation_matrix_2d(angle)
            x, y = rotate @ translated.T
        else:
            x, y = translated.T

        a = width / 2
        b = height / 2
        ellipse_eqn = (x ** 2 / a ** 2) + (y ** 2 / b ** 2)

        ellipse_points = _points[ellipse_eqn <= 1]
        complement_points = _points[ellipse_eqn > 1]

        return _filter_empty([ellipse_points, complement_points])

    if isinstance(points, np.ndarray):
        return _partition(points)
    else:
        result = [_partition(p) for p in points]
        if flatten_result:
            return sum(result, start=[])
        else:
            return result


def rectangle(width: int, height: int, x_0: int = 0, y_0: int = 0) \
        -> np.ndarray:
    """
    Returns a numpy array with shape [width x height, 2] containing the
    verts of all the pixels in the specified rectangle.
    :param y_0: minimum y-value of rectangle verts
    :param x_0: minimum x-value of rectangle verts
    :param width: width of the rectangle
    :param height: height of the rectangle
    :return:
    """
    offset = np.array([x_0, y_0])
    index = np.arange(0, width * height, dtype='int32')
    points = np.array([index % width, index // width]).T
    return points + offset


def rotation_matrix_2d(rotation_degrees: float):
    """
    Returns the matrix of the given 2d rotation.
    :param rotation_degrees: degrees of rotation (negative is clockwise,
    positive is counter-clockwise).
    :return:
    """
    # t := rotation angle in radians
    t = 2 * PI * math.fmod(rotation_degrees / 360, 1)
    transform = [[cos(t), -sin(t)],
                 [sin(t), cos(t)]]

    return np.array(transform)


def partition_with_key(points: Union[np.ndarray, List[np.ndarray]],
                       key: Callable[[float, float], float],
                       splits: Union[int, List[float]],
                       flatten_result=True):
    """
    Partitions the given the verts using the given key function and the
    specified splitting method.
    :param flatten_result: When verts is a list of segments:
    if True, return the resulting partitioned segments as a flattened list of
      segments;
    if False, return a list of lists of segments, where the nth list of segments
      in the output contains the partition of the nth input segment.
    :param points: An N x 2 array of verts to partition, or a list of such.
    :param key: A function to use to determine point ordering.
    :param splits: Either a positive integer indicating the number of
    partitions to evenly split the given verts amongst, or a list of thresholds
    to use when partitioning verts by their key values.
    :return:
    """
    vec_key = numba.vectorize(['float64(float64, float64)'],
                              nopython=True, target='parallel')(key)

    def _partition(_points):
        x, y = _points.T
        pt_projection = vec_key(x, y)
        return _split_points(_points, pt_projection, splits)

    if isinstance(points, np.ndarray):
        return _partition(points)
    else:
        result = list(map(_partition, points))
        if flatten_result:
            return sum(result, start=[])
        else:
            return result


def partition_with_image_key(points: Union[np.ndarray, List[np.ndarray]],
                             img: Image,
                             key: Callable[[float, ...], float],
                             splits: Union[int, List[float]],
                             flatten_result=True):
    """
    Returns a list of segments that partition the given verts according to the
    given key's value for each o the verts' corresponding pixel values in the
    given image.
    :param flatten_result: When verts is a list of segments:
    if True, return the resulting partitioned segments as a flattened list of
      segments;
    if False, return a list of lists of segments, where the nth list of segments
      in the output contains the partition of the nth input segment.
    :param points:
    :param img:
    :param key:
    :param splits:
    :return:
    """
    _, _, depth = img.shape()
    key_args_sig = ', '.join(repeat('float64', depth))
    key_sig = 'float64({})'.format(key_args_sig)
    vec_key = numba.vectorize([key_sig],
                              nopython=True, target='parallel')(key)

    def _partition(_points):
        point_pixels = img.A()[tuple(_points.T)]
        px_projection = vec_key(*point_pixels.T)

        return _split_points(_points, px_projection, splits)

    if isinstance(points, np.ndarray):
        return _partition(points)
    else:
        result = list(map(_partition, points))
        if flatten_result:
            return sum(result, start=[])
        else:
            return result


def _split_points(points: np.ndarray,
                  point_key: np.ndarray,
                  splits: Union[int, List[float]]):
    # infer thresholds for N even splits
    if isinstance(splits, int):
        sorted_projection = np.sort(point_key)
        split_size = len(sorted_projection) // splits
        residue = len(sorted_projection) % splits

        thresholds = []
        for s in range(splits):
            idx = s * split_size + min(s + 1, residue)
            thresholds.append(sorted_projection[idx])

        splits = thresholds

    splits = sorted(splits)
    lo = points[point_key <= splits[0]]
    hi = points[point_key > splits[-1]]

    parts = [lo]
    for a, b in zip(splits[:-1], splits[1:]):
        parts.append(points[(point_key > a) & (point_key <= b)])
    parts.append(hi)

    return _filter_empty(parts)


def _filter_empty(parts):
    return list(filter(lambda p: len(p) > 0, parts))


def sort_points(points: Union[np.ndarray, List[np.ndarray]],
                key: Callable[[float, float], float]):
    """
    Sorts the given segment(s) verts according to the value of the given key
    for each point.
    :param points: Segment or list of segments to sort verts of.
    :param key: Function imposing a (weak) total ordering on verts.
    :return:
    """
    vec_key = numba.vectorize(['float64(float64, float64)'],
                              nopython=True, target='parallel')(key)

    def _sort(_points):
        x, y = _points.T
        pt_projection = vec_key(x, y)
        idx = np.argsort(pt_projection)
        return _points[idx]

    if isinstance(points, np.ndarray):
        return _sort(points)
    else:
        result = list(map(_sort, points))
        return result
