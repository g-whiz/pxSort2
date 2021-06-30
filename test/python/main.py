import pxsort
import cv2

file_dir = 'img'
file_name = 'ethereal'
file_ext = '.png'


def main():
    in_path = file_dir + '/' + file_name + file_ext
    image = cv2.imread(in_path, cv2.IMREAD_ANYCOLOR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2BGRA)
    image = pxsort.Image(image, pxsort.ColorSpace.RGB)

    rows = 1  # image.height // 25
    columns = image.width // 25
    x0 = 0
    y0 = 0
    grid = pxsort.segmentation.Grid(image, rows, columns, x0, y0)

    loTest = pxsort.PixelPredicate.threshold(g_min=0.9)
    hiTest = pxsort.PixelPredicate.threshold(g_max=0.1)
    segments = [pxsort.segment.Asendorf(rect, loTest, hiTest) for rect in grid]
    seg = pxsort.segmentation.Segmentation(segments)


    skew = pxsort.ChannelSkew(5, 5, 0, 0, -5, -5)
    traversal = pxsort.segment.SegmentTraversal.Forward
    proj = pxsort.PixelProjection.channel(pxsort.Channel.B)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.RGB)
    n_buckets = 100

    effect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    seg.add_effect(effect)
    seg.apply_effects()

    sorted_image = image.to_array()
    sorted_image = cv2.cvtColor(sorted_image, cv2.COLOR_BGRA2BGR)

    out_path = file_dir + '/' + file_name + '_sorted' + file_ext
    cv2.imwrite(out_path, sorted_image)


if __name__ == '__main__':
    main()
