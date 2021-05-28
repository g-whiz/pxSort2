#ifndef PXSORT2_COMPARATOR_H
#define PXSORT2_COMPARATOR_H

#include "common.h"

namespace pxsort::comparator {

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
    PixelComparator channel(Channel ch, Order order);

    /**
     * Returns a PixelComparator that projects pixels onto the real line
     * using the linear map defined by the given projection matrix, and
     * imposes the given order on pixels using their image under the given
     * projection.
     * @param projection
     * @param order
     * @return
     */
    PixelComparator linear(const cv::Vec3f &projection, Order order);
}

#endif //PXSORT2_COMPARATOR_H
