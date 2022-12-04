import pxsort
import cv2
from tqdm import tqdm as progress

file_dir = 'img'
file_name = 'sleeping-beauty'
in_ext = '.png'
out_ext = '.png'


def main():
    in_path = file_dir + '/' + file_name + in_ext
    image = cv2.imread(in_path, cv2.IMREAD_ANYCOLOR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2BGRA)
    image = pxsort.Image(image, pxsort.ColorSpace.RGB)

    rows = 1  # image.height
    columns = image.width // 10
    x0 = 0
    y0 = 0
    grid = pxsort.segmentation.Grid(image, rows, columns, x0, y0)
    rectangles = [rect for rect in grid]

    left, mid, right = [0.35, 0.8, 1]
    left_rects = rectangles[:int(len(rectangles) * left)]
    mid_rects = rectangles[int(len(rectangles) * left)
                           : int(len(rectangles) * mid)]
    right_rects = rectangles[int(len(rectangles) * mid):] + rectangles[:int(len(rectangles) * 0.15)]

    leftLoTest = pxsort.PixelPredicate.linear(1, 0, 0, -0.3)
    leftHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.2)
    leftSegments = [pxsort.segment.Asendorf(rect, leftLoTest, leftHiTest) for rect in left_rects]

    midLoTest = pxsort.PixelPredicate.linear(1, 0, 0, -0.4)
    midHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.8)
    midSegments = [pxsort.segment.Asendorf(rect, midLoTest, midHiTest) for rect in mid_rects]

    # loTest = pxsort.PixelPredicate.threshold()
    # hiTest = pxsort.PixelPredicate.threshold(b_min=0.3)
    rightLoTest = pxsort.PixelPredicate.linear(-1, -1, -1, 0.3)
    rightHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.3)
    rightSegments = [pxsort.segment.Asendorf(rect, rightLoTest, rightHiTest) for rect in right_rects]
    rSeg = pxsort.segmentation.Segmentation(rightSegments)

    skew = pxsort.ChannelSkew(0, 0, 0,
                              60, -60, 20)
    traversal = pxsort.segment.SegmentTraversal.Forward
    proj = pxsort.PixelProjection.linear(0.0, 0.0, 1.0, 0)
    # cmp = pxsort.PixelComparator.channel(pxsort.Channel.G, pxsort.Order.Ascending)
    mix = pxsort.PixelMixer.copier(pxsort.InputChannel.IN2_B,
                                   pxsort.InputChannel.IN1_G,
                                   pxsort.InputChannel.IN2_B,
                                   pxsort.InputChannel.IN1_B,
                                   pxsort.InputChannel.IN2_G,
                                   pxsort.InputChannel.IN1_B)
    n_buckets = 100
    rEffect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    rSeg.add_effect(rEffect)
    rSeg.apply_effects()


    segments = leftSegments + midSegments
    seg = pxsort.segmentation.Segmentation(segments)

    skew = pxsort.ChannelSkew(0, 0, 0,
                              -60, -20, 20)
    traversal = pxsort.segment.SegmentTraversal.Reverse
    proj = pxsort.PixelProjection.linear(1.0, 0.0, 1.0, 0)
    # cmp = pxsort.PixelComparator.channel(pxsort.Channel.G, pxsort.Order.Ascending)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.RB)
    n_buckets = 100

    iters = 1

    effect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    # effect = pxsort.effect.PartialHeapify(skew, traversal, cmp, mix)
    seg.add_effect(effect)
    for _ in progress(range(iters)):
        seg.apply_effects()

    sorted_image = image.to_array()
    sorted_image = cv2.cvtColor(sorted_image, cv2.COLOR_BGRA2BGR)

    out_path = file_dir + '/' + file_name + '_sorted' + out_ext
    cv2.imwrite(out_path, sorted_image)


if __name__ == '__main__':
    main()
