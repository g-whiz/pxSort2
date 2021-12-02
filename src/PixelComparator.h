#ifndef PXSORT2_PIXELCOMPARATOR_H
#define PXSORT2_PIXELCOMPARATOR_H

#include "common.h"

/**
 * A PixelComparator is a callable object that imposes some ordering on
 *   Pixels.
 * The value returned by a PixelComparator is negative, zero, or positive
 *   when the first pixel is less-than, equal-to, or greater-than the second
 *   pixel, respectively.
 */
class pxsort::PixelComparator : CloneableInterface<PixelComparator> {
public:
    enum Order {
        ASCENDING,  // sort pixels in ascending order
        DESCENDING  // sort pixels in descending order
    };

    /**
     * Creates a PixelComparator that imposes the specified order on pixels
     * by comparing the specified channel.
     * @param channel The chanel to compare
     * @param order The direction to order pixels in
     */
    PixelComparator(Channel channel, Order order);

    /**
     * Returns a PixelComparator that projects pixels onto the real line
     * using the linear map defined by the given projection matrix, and
     * imposes the given order on pixels using their image under the given
     * projection.
     * @param projection The projection to apply to pixels
     * @param order The direction to order pixels in
     */
    PixelComparator(const cv::Vec3f &projection, Order order);

    PixelComparator(const PixelComparator& other);

    float operator()(const Pixel &left, const Pixel &right);

    [[nodiscard]] std::unique_ptr<PixelComparator> clone() const override;

private:
    const cv::Vec3f M;
    const Order order;
};

#endif //PXSORT2_PIXELCOMPARATOR_H
