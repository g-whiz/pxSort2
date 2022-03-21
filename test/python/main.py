import pxsort
import cv2
from tqdm import tqdm as progress

file_dir = 'img'
file_name = 'marcus'
in_ext = '.jpg'
out_ext = '.png'


def main():
    in_path = file_dir + '/' + file_name + in_ext
    image = cv2.imread(in_path, cv2.IMREAD_ANYCOLOR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2BGRA)
    image = pxsort.Image(image, pxsort.ColorSpace.HLS)

    rows = 18 # image.height
    columns = 24 # image.width // 400
    x0 = 50
    y0 = 100
    grid = pxsort.segmentation.Grid(image, rows, columns, x0, y0)

    loTest = pxsort.PixelPredicate.threshold()
    hiTest = pxsort.PixelPredicate.threshold()
    segments = [pxsort.segment.Asendorf(rect, loTest, hiTest) for rect in grid]
    seg = pxsort.segmentation.Segmentation(segments)


    skew = pxsort.ChannelSkew(10, 20, 3, -30, -2, -10)
    traversal = pxsort.segment.SegmentTraversal.Forward
    # proj = pxsort.PixelProjection.channel(pxsort.Channel.R)
    cmp = pxsort.PixelComparator.channel(pxsort.Channel.G, pxsort.Order.Ascending)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.G)
    n_buckets = 10
    
    iters = 75000

    #effect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    effect = pxsort.effect.PartialHeapify(skew, traversal, cmp, mix)
    seg.add_effect(effect)
    for _ in progress(range(iters)):
    	seg.apply_effects()

    sorted_image = image.to_array()
    sorted_image = cv2.cvtColor(sorted_image, cv2.COLOR_BGRA2BGR)

    out_path = file_dir + '/' + file_name + '_sorted' + out_ext
    cv2.imwrite(out_path, sorted_image)


if __name__ == '__main__':
    main()
