import concurrent.futures
import itertools
import math
import random
from noise.perlin import SimplexNoise

from pxsort import *
from builtins import map
import cv2 as cv
import numpy as np
from numba import types, carray, cfunc, jit
from tqdm.auto import trange, tqdm

from time import time
import os


in_dir = 'img'
in_name = 'frozen_1k'
in_ext = '.png'
in_file = in_dir + '/' + in_name + in_ext

out_dir = 'result' + '/' + in_name


def polar_key(center, radial, rot=0.0, reverse=False):
    cx, cy = center
    c_r = 1e-9
    c_phi = 1e9
    sgn_phi = 1
    if reverse:
        sgn_phi = -1
    if radial:
        c_phi = 1
        c_r = 1e9

    def _key(x, y):
        x -= cx
        y -= cy
        phi = sgn_phi * (np.fmod(math.atan2(y, x) + math.pi + rot, 2 * math.pi) - math.pi)
        r = math.hypot(x, y)
        return (c_r * r) + (c_phi * phi)
    return _key

def read_img(filename):
    in_img = cv.imread(filename, cv.IMREAD_COLOR)
    in_img_fp = np.array(np.transpose(in_img, (1, 0, 2)),
                         copy=False, dtype='float32')

    return Image(in_img_fp / 255)


def write_image(image):
    global count
    count += 1
    filename = out_dir + '/' + '{}.png'.format(int(time()) + count)

    image_array = np.transpose(np.array(image, copy=False), (1, 0, 2))
    cv.imwrite(filename, image_array * 255)

def read_frame(cap):
    ret, img = cap.read()
    img_fp = np.array(np.transpose(img, (1, 0, 2)),
                         copy=False, dtype='float32')

    return ret, Image(img_fp / 255)

def write_frame(writer, img):
    frame = (255 * np.transpose(np.array(img, copy=False), (1, 0, 2))) \
        .astype('uint8')
    writer.write(frame)


def project_channel(cn):
    def _project(px):
        return px[cn]

    return _project


def hue(px):
    b, g, r = px
    if b + g + r == 0:
        return 0

    offset = 0 if g >= b else 2 * math.pi
    sign = 1 if g >= b else -1

    h_num = r - 0.5 * g - 0.5 * b
    h_den = math.sqrt(r**2 + g**2 + b**2 + r*g + r*b + g*b)
    h = math.acos(h_num / h_den)

    return (offset + sign * h) / (2 * math.pi)


def saturation(px):
    b, g, r = px

    hi = max(r, g, b)
    lo = min(r, g, b)
    L = (hi + lo) / 2

    if L == 0 or L == 1:
        return 0.0
    else:
        return (hi - lo) / (1 - abs(2*L - 1))


def lightness(px):
    b, g, r = px
    hi = max(r, g, b)
    lo = min(r, g, b)
    return (hi + lo) / 2


def darkness(px):
    b, g, r = px
    hi = max(r, g, b)
    lo = min(r, g, b)
    return 1 - ((hi + lo) / 2)


def wrap_projection(projection):
    f = jit(projection, nopython=True)

    @cfunc(map_function_signature(), nopython=True)
    def proj_map_cfunc(p_in, n_in, p_out, n_out):
        a_in = carray(p_in, (n_in,))
        a_out = carray(p_out, (n_out,))
        a_out[0] = f(a_in)

    return proj_map_cfunc


def swap_channels(channels):
    channels_array = np.array(list(channels))

    def _swap(px_in, px_out):
        depth = px_in.shape[0] // 2
        for cn in channels_array:
            px_out[cn] = px_in[depth + cn]
            px_out[depth + cn] = px_in[cn]

    return _swap


def wrap_mixer(mixer):
    f = jit(mixer, nopython=True)

    @cfunc(map_function_signature(), nopython=True)
    def proj_map_cfunc(p_in, n_in, p_out, n_out):
        in_array = carray(p_in, (n_in,))
        out_array = carray(p_out, (n_out,))

        for i in range(n_in):
            out_array[i] = in_array[i]

        f(in_array, out_array)

    return proj_map_cfunc


def main():
    os.makedirs(out_dir, exist_ok=True)

    print("Loading video.")

    print("Loading image.")

    in_img = read_img(in_file)
    w, h, _ = in_img.shape()

    anim_seconds = 4
    framerate = 30
    frames = anim_seconds * framerate

    # codec = 'FFV1'  # ffmpeg lossless
    codec = 'avc1'
    # codec = 'H264'
    out_file = out_dir + '/' + '{}.mp4'.format(int(time()))

    out_fourcc = cv.VideoWriter_fourcc(*codec)
    out_writer = cv.VideoWriter(out_file, fourcc=out_fourcc,
                                 fps=framerate, frameSize=(w, h))

    print("Compiling callbacks.")

    project = Map(wrap_projection(lightness).address, 3, 1)

    swap_all = Map(wrap_mixer(swap_channels([0, 1, 2])).address, 6, 6)
    swap_r = Map(wrap_mixer(swap_channels([2])).address, 6, 6)
    swap_rg = Map(wrap_mixer(swap_channels([1, 2])).address, 6, 6)
    swap_g = Map(wrap_mixer(swap_channels([1])).address, 6, 6)
    swap_gb = Map(wrap_mixer(swap_channels([0, 1])).address, 6, 6)
    swap_b = Map(wrap_mixer(swap_channels([0])).address, 6, 6)
    swap_br = Map(wrap_mixer(swap_channels([0, 2])).address, 6, 6)

    # mixers = [swap_all, swap_rg, swap_all, swap_gb, swap_all, swap_br]
    mixers = [swap_all, swap_r, swap_all, swap_g, swap_all, swap_b]


    frames_per_mixer = 60

    print("Building annuli.")

    n_annuli = 270
    radial = False
    buckets = 100

    skew = Skew()

    cx = int(.67 * w)
    cy = int((1 - .625) * h)

    def annulus_key(x, y):
        return math.hypot(x - cx, y - cy)

    verts = rectangle(w, h)
    annuli = partition_with_key(verts, annulus_key, n_annuli)
    n_segs = len(annuli)

    # fraction of each segment to sort
    sort_fraction = 0.15
    # magnitude of noise to add to each segment's fraction param when sorting
    sf_fuzziness = 0.125

    sn = SimplexNoise()
    sn.randomize()
    sf_noise = np.zeros((frames, n_segs))
    for x in range(frames):
        for y in range(n_segs):
            sf_noise[x, y] = sn.noise2(x, y)

    sf_noise *= sf_fuzziness

    for frame_n in tqdm(range(frames), desc="Computing frames"):

        frame = Image(in_img)

        frame_skew = skew

        frame_rot = (frame_n / frames) * 2 * math.pi
        frame_pk = polar_key([cx, cy], radial, frame_rot, reverse=True)

        frame_annuli = sort_points(annuli, frame_pk)

        f_mixers = [mixers[random.randint(0, 5)] for _ in range(n_segs)]

        sorters = list(map(lambda mix, noise: Sorter.create_pseudo_bubble_sorter(
            project, mix, sort_fraction + noise, buckets), f_mixers, sf_noise[frame_n]))

        topology = ImageTopology.Torus
        traversal = SegmentTraversal.Forward

        def sort_segment(i):
            s = Segment(frame_annuli[i])
            seg_px = s.get_pixels(frame, traversal, None, topology)
            skew_px = s.get_pixels(frame, traversal, frame_skew, topology)
            sorted_seg_px = sorters[i](seg_px, skew_px)
            # this line mutates the frame
            s.put_pixels(frame, SegmentTraversal.Forward,
                         sorted_seg_px, topology)

        futures = [executor.submit(sort_segment, i) for i in range(n_segs)]
        for _ in concurrent.futures.as_completed(futures):
            pass

        write_frame(out_writer, frame)


if __name__ == '__main__':
    executor = concurrent.futures.ThreadPoolExecutor(max_workers=40)
    main()
