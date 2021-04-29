//
// Created by gpg on 2021-04-28.
//

#ifndef PXSORT2_COMPARATOR_H
#define PXSORT2_COMPARATOR_H

#include <pxsort/Segment.h>

using namespace cv;

namespace pxsort {

    /**
     * A PixelComparator is a callable object that imposes some ordering on
     *   Pixels.
     * The value returned by a PixelComparator is negative, zero, or positive
     *   when the first pixel is less-than, equal-to, or greater-than the second
     *   pixel, respectively.
     */
    typedef std::function<float(const Pixel&, const Pixel&)> PixelComparator;

    namespace comparator {

        enum Channel {
            RED,    // compare the first channel from each pixel
            GREEN,  // compare the second channel from each pixel
            BLUE    // compare the third channel from each pixel
        };

        enum Order {
            ASCENDING,  // sort pixels in ascending order
            DESCENDING  // sort pixels in descending order
        };

        /**
         * Returns a PixelComparator that imposes the specified order on pixels
         * by comparing the specified channel.
         * @param ch
         * @param order
         * @return
         */
        PixelComparator compareChannel(Channel ch, Order order);

        /**
         * Returns a PixelComparator that projects pixels onto the real line
         * using the linear map defined by the given projection matrix, and
         * imposes the given order on pixels using their image under the given
         * projection.
         * @param projection
         * @param order
         * @return
         */
        PixelComparator linearProjection(const Vec3f &projection, Order order);
    }
}

#endif //PXSORT2_COMPARATOR_H
