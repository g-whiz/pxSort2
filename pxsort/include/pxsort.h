//
// Created by gpg on 2021-04-05.
//

#ifndef PXSORT2_PXSORT_H
#define PXSORT2_PXSORT_H

#include <memory>
#include <utility>

#include <omp.h>

#include <opencv2/core/affine.hpp>
#include <opencv2/core/matx.hpp>

#include <pxsort/Comparator.h>
#include <pxsort/Effect.h>
#include <pxsort/effect/BubbleSort.h>
#include <pxsort/effect/CompareAndMix.h>
#include <pxsort/effect/Heapify.h>
#include <pxsort/Image.h>
#include <pxsort/Mixer.h>
#include <pxsort/Segment.h>
#include <pxsort/segment/Rectangle.h>
#include <pxsort/Segmentation.h>
#include <pxsort/segmentation/Grid.h>

#endif //PXSORT2_PXSORT_H
