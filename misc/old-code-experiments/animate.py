import math
import os

import gc

import numpy as np
import pxsort
import cv2
from time import time
from tqdm import tqdm as progress
from random  import shuffle

# Aliases
from pxsort import \
    ChannelSkew as Skew, \
    PixelMixer as Mixer, \
    Swap, \
    PixelPredicate as Predicate, \
    PixelProjection as Project, \
    PixelComparator as Compare, \
    ColorSpace, \
    Channel, \
    Order

Grid = pxsort.segmentation.Grid
Asendorf = pxsort.segment.Asendorf
Segmentation = pxsort.segmentation.Segmentation
Traverse = pxsort.segment.SegmentTraversal

BucketSort = pxsort.effect.BucketSort
BubblePass = pxsort.effect.PartialBubbleSort
HeapifyPass = pxsort.effect.PartialHeapify


# Global Variables
color_space = ColorSpace.RGB

rotate_90 = False

anim_fps = 24
anim_seconds = 8
anim_frames = int(anim_seconds * anim_fps)

src_dir = 'img'
src_name = 'maple_leaf_gardens_1k'
src_ext = '.png'
src_file = src_dir + '/' + src_name + src_ext

out_dir = 'animations/' + src_name + '/' + '{}/'.format(int(time()))
out_fmt = '{:0>3}.png'


def linear_mix(r=0.9, g=0.9, b=0.9):
    t = np.array(
    [[1 - r, 0, 0, r, 0, 0],
     [0, 1 - g, 0, 0, g, 0],
     [0, 0, 1 - b, 0, 0, b],
     [r, 0, 0, 1 - r, 0, 0],
     [0, g, 0, 0, 1 - g, 0],
     [0, 0, b, 0, 0, 1 - b]]
    )
    return Mixer.linear(t)


def no_skew():
    return Skew(0, 0, 0, 0, 0, 0)


def triangle(theta):
    fraction = (theta / math.pi) - int(theta / math.pi)
    if fraction > 0.5:
        abs_value = 1 - fraction
    else:
        abs_value = fraction
    coefficient = -2 if int(theta / math.pi) % 2 == 1 else 2

    return coefficient * abs_value



def write_frame(frame_n, img):
    out_file = out_dir + out_fmt.format(frame_n)
    out_image = img.to_array()
    out_image = cv2.cvtColor(out_image, cv2.COLOR_BGRA2BGR)

    if rotate_90:
        out_image = cv2.rotate(out_image, cv2.ROTATE_90_COUNTERCLOCKWISE)

    cv2.imwrite(out_file, out_image)
    del out_image


def shifty_sort(frame_n, img, skew_hz=1/3,
                r=27, shift_hz=0.25, osc_hz=0,
                min_iters=2, max_iters=10):
    # r_scale = [1, 2, 4, 2]
    # grid_r = base_r * r_scale[int((frame_n / anim_frames) * 4)]

    segments = shifty_grid(frame_n, img, grid_r=r,
                           shift_hz=shift_hz, osc_hz=osc_hz)

    skew_phi = (2 * math.pi * skew_hz / anim_fps) * frame_n
    rx = int(3 * math.cos(skew_phi))
    ry = int(3 * math.sin(skew_phi))
    gx = int(5 * math.cos(skew_phi + 0.66 * math.pi))
    gy = int(5 * math.sin(skew_phi + 0.66 * math.pi))
    bx = int(7 * math.cos(skew_phi - 0.66 * math.pi))
    by = int(7 * math.sin(skew_phi - 0.66 * math.pi))
    skew = Skew(rx,  gx,  bx,
                ry,  gy,  by)

    # mix = Mixer.swapper(Swap.RGB)
    mix = linear_mix(r = 0.7, g = 0.7, b = 0)
    cmp = Compare.channel(pxsort.Channel.R, pxsort.Order.Ascending)
    sorter = BubblePass(skew, Traverse.Forward, cmp, mix)

    segmentation = Segmentation(segments)
    segmentation.add_effect(sorter)

    bubble_phi = (2 * math.pi * shift_hz / anim_fps) * frame_n
    iters = math.fabs(triangle(2 * bubble_phi)) \
            * (max_iters - min_iters) + min_iters
    for _ in range(abs(int(iters))):
        segmentation.apply_effects()


def osc_random_grid_tiles(frame_n, img, hz=0.0625, r_min=20, r_max=100,
                          sorted_frac=0.25):
    phi = (2 * math.pi * hz / anim_fps) * frame_n

    radius = int(math.fabs(math.sin(0.5 * phi)) * (r_max - r_min) + r_min)
    x0, y0 = [radius * math.cos(phi), 0]
    rows, columns = [1, img.width // radius]

    grid = Grid(img, rows, columns, int(x0), int(y0))

    asdf_lo = Predicate.linear(1, 1, 1, -2.5
                               + 1 * math.fabs(math.sin(0.5 * phi)))
    asdf_hi = Predicate.linear(1, 1, 1, -2.5
                               + 1 * math.fabs(math.sin(0.5 * phi)))

    asdf_segments = [Asendorf(r, asdf_lo, asdf_hi) for r in grid]
    shuffle(asdf_segments)
    n_segments = int(sorted_frac * len(asdf_segments))
    segmentation = Segmentation(asdf_segments[:n_segments])

    rx = int(3 * math.cos(- 0.7 * phi + math.pi))
    ry = int(3 * math.sin(- 0.7 * phi + math.pi))
    gx = int(5 * math.cos(phi + 0.36 * math.pi))
    gy = int(5 * math.sin(phi + 0.36 * math.pi))
    bx = int(7 * math.cos(1.1 * phi - 0.29 * math.pi))
    by = int(7 * math.sin(1.1 * phi - 0.29 * math.pi))
    skew = Skew(rx,  gx,  bx,
                ry,  gy,  by)

    p = Project.channel(Channel.R)
    m = linear_mix(r = 0.9, g = 0.9, b = 0.9)
    # m = Mixer.swapper(Swap.RGB)
    buckets = 25
    sorter = BucketSort(skew, Traverse.Forward, p, m, buckets)

    segmentation.add_effect(sorter)
    segmentation.apply_effects()


def oscillating_grid(frame_n, img, hz=0.25, r_min=11, r_max=108):
    phi = (2 * math.pi * hz / anim_fps) * frame_n

    radius = int(math.fabs(triangle(2 * phi)) * (r_max - r_min) + r_min)
    x0, y0 = [radius * math.cos(phi), radius * math.sin(phi)]
    # x0, y0 = [0, 0]
    rows, columns = [1, img.width // radius]

    grid = Grid(img, rows, columns, int(x0), int(y0))
    # segmentation = grid

    asdf_lo = Predicate.linear(0, 0, -1, 0.3
                               - 0.3 * math.fabs(math.sin(0.5 * phi)))
    asdf_hi = Predicate.linear(0, 0, -1, 0.3
                               - 0.3 * math.fabs(math.sin(0.5 * phi)))
    asdf_segments = [Asendorf(r, asdf_lo, asdf_hi) for r in grid]
    segmentation = Segmentation(asdf_segments)

    rx = int(3 * math.cos(- 0.7 * phi + math.pi))
    ry = int(3 * math.sin(- 0.7 * phi + math.pi))
    gx = int(5 * math.cos(phi + 0.36 * math.pi))
    gy = int(5 * math.sin(phi + 0.36 * math.pi))
    bx = int(7 * math.cos(1.1 * phi - 0.29 * math.pi))
    by = int(7 * math.sin(1.1 * phi - 0.29 * math.pi))
    skew = Skew(rx,  gx,  bx,
                ry,  gy,  by)
                # 0, 0, 0)

    # p = Project.linear(1, 0.5, -1, 0)
    # p = Project.linear(0.5, 0.25, 1, 0)
    p = Project.channel(Channel.R)
    mix_T = np.array(
        [[0, 0, 0, 1, 0, 0],
         [0, 0.5, 0, 0, 0.5, 0],
         [0, 0, 1, 0, 0, 0],
         [1, 0, 0, 0, 0, 0],
         [0, 0.5, 0, 0, 0.5, 0],
         [0, 0, 0, 0, 0, 1]]
    )
    # m = Mixer.linear(mix_T)
    m = Mixer.swapper(Swap.R)
    buckets = 100
    sorter = BucketSort(skew, Traverse.Reverse, p, m, buckets)

    segmentation.add_effect(sorter)
    segmentation.apply_effects()


def oscillating_bubble(frame_n, img, hz=0.125, r_min=50, r_max=100, min_iters=2, max_iters=8):
    phi = (2 * math.pi * hz / anim_fps) * frame_n
    radius = int(math.fabs(math.sin(0.5 * phi)) * (r_max - r_min) + r_min)
    x0, y0 = [radius * math.cos(phi), radius * math.sin(phi)]
    rows, columns = [img.height // radius, img.width // radius]

    # rows, columns = [1, 1]#[img.height // 25, img.width // 25]
    # x0, y0 = [0, 0]
    segmentation = Grid(img, rows, columns, int(x0), int(y0))

    rx = int(3 * math.cos(- 0.7 * phi + math.pi))
    ry = int(3 * math.sin(- 0.7 * phi + math.pi))
    gx = int(5 * math.cos(1.4 * phi + 0.36 * math.pi))
    gy = int(5 * math.sin(1.4 * phi + 0.36 * math.pi))
    bx = int(7 * math.cos(2.1 * phi - 0.29 * math.pi))
    by = int(7 * math.sin(2.1 * phi - 0.29 * math.pi))
    skew = Skew(rx,  gx,  bx,
                ry,  gy,  by)


    # mix_T = np.array(
    #     [[0.1, 0, 0, 0.9, 0, 0],
    #      [0, 0.1, 0, 0, 0.9, 0],
    #      [0, 0, 0.1, 0, 0, 0.9],
    #      [0.9, 0, 0, 0.1, 0, 0],
    #      [0, 0.9, 0, 0, 0.1, 0],
    #      [0, 0, 0.9, 0, 0, 0.1]]
    # )
    # mix = Mixer.linear(mix_T)
    mix = Mixer.swapper(Swap.RGB)
    cmp = Compare.channel(pxsort.Channel.R, pxsort.Order.Ascending)
    sorter = BubblePass(skew, Traverse.Forward, cmp, mix)
    segmentation.add_effect(sorter)

    iters = math.fabs(math.sin(2 * phi)) \
            * (max_iters - min_iters) + min_iters
    for _ in range(abs(int(iters))):
        segmentation.apply_effects()


def oscillating_heapify(frame_n, img, hz=0.25,
                        r_min=1, r_max=1080,
                        min_progress=1.0, max_progress=1.0):
    phi = (2 * math.pi * hz / anim_fps) * frame_n

    radius = int(math.fabs(triangle(0.5 * phi)**2) * (r_max - r_min) + r_min)
    if radius == 1:
        return
    x0, y0 = [radius * math.cos(phi), radius * math.sin(phi)]
    rows, columns = [img.height // radius, img.width // radius]

    # x0, y0 = [0, 0]
    # rows, columns = [1, 1]
    segmentation = Grid(img, rows, columns, int(x0), int(y0))

    skew = no_skew()

    # mix = linear_mix(r = 0.7, g = 0.7, b = 0.7)
    mix = Mixer.swapper(Swap.RGB)
    # cmp = Compare.channel(pxsort.Channel.R, Order.Ascending)
    cmp = Compare.linear(0.0, 1.0, 1.0, Order.Descending)
    sorter = HeapifyPass(skew, Traverse.Forward, cmp, mix)
    segmentation.add_effect(sorter)

    grid_tile_pixels = (img.width // columns + 1) * (img.height // rows + 1)
    max_iters = int(max_progress * grid_tile_pixels)
    min_iters = int(min_progress * grid_tile_pixels)

    iters = math.fabs(math.sin(2 * phi)) \
            * (max_iters - min_iters) + min_iters
    for _ in range(abs(int(iters))):
        segmentation.apply_effects()

def shifty_heapify(frame_n, img, hz=0.25, r = 108,
                        min_progress=1.0, max_progress=1.0):
    segments = shifty_grid(frame_n + 24, img, grid_r=r, shift_hz=0.25)

    skew = no_skew()

    mix = linear_mix(r = 0.7, g = 0.7, b = 0.7)
    # mix = Mixer.swapper(Swap.RGB)
    # cmp = Compare.channel(pxsort.Channel.R, Order.Ascending)
    cmp = Compare.linear(0.0, 1.0, 1.0, Order.Descending)
    sorter = HeapifyPass(skew, Traverse.Forward, cmp, mix)

    segmentation = Segmentation(segments)
    segmentation.add_effect(sorter)

    iters = (r + 1) ** 2
    for _ in range(iters):
        segmentation.apply_effects()


def progressive_bubble(frame_n, img, min_iters=0, max_iters=5):
    rows, columns = [img.height // 75, img.width // 75]
    x0, y0 = [0, 0]
    segmentation = Grid(img, rows, columns, int(x0), int(y0))

    skew = Skew(12,  -6,  3,
                -9,  -6,  15)

    mix_T = np.array(
        [[0.3, 0, 0, 0.7, 0, 0],
         [0, 0.3, 0, 0, 0.7, 0],
         [0, 0, 0.3, 0, 0, 0.7],
         [0.7, 0, 0, 0.3, 0, 0],
         [0, 0.7, 0, 0, 0.3, 0],
         [0, 0, 0.7, 0, 0, 0.3]]
    )
    mix = Mixer.linear(mix_T)
    cmp = Compare.linear(1.0, 0.0, 0.0, Order.Ascending)
    traversal = Traverse.Forward \
        if (frame_n / anim_frames) <= 0.5 \
        else Traverse.Reverse
    sorter = BubblePass(skew, traversal, cmp, mix)
    segmentation.add_effect(sorter)

    hz = 1 / anim_seconds
    phi = (2 * math.pi * hz / anim_fps) * frame_n
    iters = math.fabs(math.sin(phi)) \
            * (max_iters - min_iters) + min_iters
    for _ in range(abs(int(iters))):
        segmentation.apply_effects()


def asdf_vert_pass(frame_n, img):
    rows, columns = [1, img.width]
    x0, y0 = [0, frame_n * 200]
    grid = Grid(img, rows, columns, x0, y0)

    asdf_lo = Predicate.linear(0, 0, -1, 0.3)
    asdf_hi = Predicate.linear(0, 0, -1, 0.3)
    asdf_segments = [Asendorf(r, asdf_lo, asdf_hi) for r in grid]
    segmentation = Segmentation(asdf_segments)

    skew = Skew(0, 0, 0,
                0, 0, 0)
    p = Project.linear(1.0, 0.0, 0.0, 0)
    m = Mixer.swapper(Swap.RGB)
    buckets = 6
    sorter = BucketSort(skew, Traverse.Forward, p, m, buckets)

    segmentation.add_effect(sorter)
    segmentation.apply_effects()
    del segmentation
    
    
def shifty_grid(frame_n, img, shift_hz=0.5, grid_r=108, osc_hz=0):
    shift_phi = (2 * math.pi * shift_hz / anim_fps) * frame_n
    osc_phi = (2 * math.pi * osc_hz / anim_fps) * frame_n

    rows = img.height // grid_r
    columns = img.width // grid_r

    cycle = int(2 * shift_phi / math.pi) % 4

    # shift bias params based on where in the animation we are
    xb = [0, 0, grid_r // 2, grid_r // 2][cycle]
    yb = [0, grid_r // 2, grid_r // 2, 0][cycle]

    shift_ay = int(0.5 * grid_r * math.sin(shift_phi))
    shift_by = -shift_ay

    shift_ax = int(0.5 * grid_r * math.cos(shift_phi))
    shift_bx = -shift_ax

    x0 = int(grid_r * math.cos(osc_phi) - grid_r)
    y0 = int(grid_r * math.sin(osc_phi))

    segments = []
    if cycle % 2 == 1:  # horizontal shift
        grid_a = Grid(img, rows, columns, x0 + shift_ax + xb, y0 + yb)
        grid_b = Grid(img, rows, columns, x0 + shift_bx + xb, y0 + yb)

        for row in range(rows):
            for col in range(columns):
                if row % 2 == 0:
                    segments.append(grid_a[col + row * columns])
                else:
                    segments.append(grid_b[col + row * columns])

    else:  # vertical shift
        grid_a = Grid(img, rows, columns, x0 + xb, y0 + shift_ay + yb)
        grid_b = Grid(img, rows, columns, x0 + xb, y0 + shift_by + yb)

        for col in range(columns):
            for row in range(rows):
                if col % 2 == 0:
                    segments.append(grid_a[col + row * columns])
                else:
                    segments.append(grid_b[col + row * columns])

    return segments


def compute_frame(frame_n, img=None):
    if img is None:
        img = read_img()

    # osc_random_grid_tiles(frame_n, img)
    # oscillating_grid(frame_n, img)
    # oscillating_heapify(frame_n, img)
    # oscillating_bubble(frame_n, img)
    #progressive_bubble(frame_n, img)
    #asdf_vert_pass(frame_n, img)

    shifty_sort(frame_n, img, r=54)
    # shifty_heapify(frame_n, img)
    # shifty_sort(frame_n, img, r=27, shift_hz=1)


    write_frame(frame_n, img)
    # del img


def read_img():
    img = cv2.imread(src_file, cv2.IMREAD_ANYCOLOR)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)

    if rotate_90:
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)

    img = pxsort.Image(img, color_space)
    return img


if __name__ == '__main__':
    os.makedirs(out_dir, exist_ok=True)

    # image = read_img()
    image = None

    for frame_n in progress(range(anim_frames), desc='Computing frames: '):
        compute_frame(frame_n, image)

        if (frame_n + 1) % 16 == 0:
            gc.collect()

