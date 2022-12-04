import pxsort
import cv2
from tqdm import tqdm as progress

file_dir = 'img'
file_name = 'maple_leaf_gardens'
in_ext = '.png'
out_ext = '.png'


def main():
    in_path = file_dir + '/' + file_name + in_ext
    image = cv2.imread(in_path, cv2.IMREAD_ANYCOLOR)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2BGRA)
    image = pxsort.Image(image, pxsort.ColorSpace.HLS)

    rows = 1  # image.height
    columns = image.width // 10
    grid = pxsort.segmentation.Grid(image, rows, columns, 0, 0)
    vrects = [rect for rect in grid]

    left, mid, right = [0.35, 1.0, 0.73]
    left_rects = vrects[:int(len(vrects) * left)]
    mid_rects = vrects[int(len(vrects) * left)
                           : int(len(vrects) * mid)]
    right_rects = vrects[int(len(vrects) * right):] + left_rects

    leftLoTest = pxsort.PixelPredicate.linear(1, 0, 0, -0.3)
    leftHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.2)
    leftSegments = [pxsort.segment.Asendorf(rect, leftLoTest, leftHiTest) for rect in left_rects]

    midLoTest = pxsort.PixelPredicate.linear(1, 0, 0, -0.4)
    midHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.8)
    midSegments = [pxsort.segment.Asendorf(rect, midLoTest, midHiTest) for rect in mid_rects]

    rightLoTest = pxsort.PixelPredicate.linear(-1, -1, -1, 0.3)
    rightHiTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.3)
    rightSegments = [pxsort.segment.Asendorf(rect, rightLoTest, rightHiTest) for rect in right_rects]
    rSeg = pxsort.segmentation.Segmentation(rightSegments)

    skew = pxsort.ChannelSkew(0, 0, 0,
                              30, 0, 60)
    traversal = pxsort.segment.SegmentTraversal.Forward
    proj = pxsort.PixelProjection.linear(1.0, 0.0, 1.0, 0)
    # cmp = pxsort.PixelComparator.channel(pxsort.Channel.G, pxsort.Order.Ascending)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.RB)
    n_buckets = 100
    rEffect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    rSeg.add_effect(rEffect)



    segments = leftSegments + midSegments
    seg = pxsort.segmentation.Segmentation(segments)

    skew = pxsort.ChannelSkew(-60, 20, 0,
                              0, 0, 50)
    traversal = pxsort.segment.SegmentTraversal.Reverse
    proj = pxsort.PixelProjection.linear(0.5, 0.1, 0.1, 0)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.RB)
    n_buckets = 10
    effect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    seg.add_effect(effect)


    rows = 56
    columns = 37
    grid = pxsort.segmentation.Grid(image, rows, columns, 0, 0)
    hRects = [rect for rect in grid]

    hLoTest = pxsort.PixelPredicate.linear(1, 1, 1, -0.2)
    hHiTest = pxsort.PixelPredicate.threshold()
    hSegments = [pxsort.segment.Asendorf(rect, hLoTest, hHiTest) for rect in hRects[::2]]
    hseg = pxsort.segmentation.Segmentation(hSegments)

    skew = pxsort.ChannelSkew(-6, 20, 11,
                              4, -20, 13)
    traversal = pxsort.segment.SegmentTraversal.Reverse
    proj = pxsort.PixelProjection.linear(0.0, 1.0, 0.0, 0)
    mix = pxsort.PixelMixer.swapper(pxsort.Swap.RGB)
    n_buckets = 100
    hEffect = pxsort.effect.BucketSort(skew, traversal, proj, mix, n_buckets)
    hseg.add_effect(hEffect)

    seg.apply_effects()
    hseg.apply_effects()
    rSeg.apply_effects()
    hseg.apply_effects()
    seg.apply_effects()
    rSeg.apply_effects()

    sorted_image = image.to_array()
    sorted_image = cv2.cvtColor(sorted_image, cv2.COLOR_BGRA2BGR)

    out_path = file_dir + '/' + file_name + '_sorted' + out_ext
    cv2.imwrite(out_path, sorted_image)


if __name__ == '__main__':
    main()
