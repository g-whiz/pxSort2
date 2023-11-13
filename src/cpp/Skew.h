#ifndef PXSORT_SKEW_H
#define PXSORT_SKEW_H

#include <vector>
#include <memory>

#include "fwd.h"
#include "geometry/Point.h"

struct pxsort::Skew {
    class SkewImpl;

    using SkewFunction = std::function<void(const int32_t*, int32_t, int32_t*)>;
    using SkewFnPtr = void(*)(const int32_t*, int32_t, int32_t*);

    /**
     * The policy to apply to determine the skew of points lying outside of
     * a Skew's configured bounds.
     */
    enum OutOfBoundsPolicy {
        /** Skew determined by wrapping verts around grid edges. */
        WRAP,
        /** Skew determined by clamping verts to grid edges. */
        CLAMP
    };

    /**
     * Default constructor: creates a nil Skew (i.e. functions as an identity
     * operation on given verts).
     */
    Skew();

    /**
     * Returns a Skew that applies a coordinate-invariant translation to the
     * verts in each channel of a segment.
     * @param skews The translation vectors to apply to each channel.
     * @param p The policy to apply for skewing a channel out of the range of
     * the given skews. This policy applies only to the channel in the case of
     * constant skews.
     */
    explicit
    Skew(const std::vector<Point>& skews, OutOfBoundsPolicy p = WRAP);


    /**
     * Returns a Skew that applies a channel-specific coordinate transform to
     * the verts in each channel of a segment.
     * @param f A coordinate transform that takes as input:
     *          (int xy_in[2], int channel, int xy_out[2])
     * and computes a channel-specific coordinate transform on xy_in, returning
     * the result by pointer using xy_out.
     * @param im_w The width of the image being skewed.
     * @param im_h The height of the image being skewed.
     * @param im_d The depth of the image being skewed.
     * @param p The policy to apply for skewing a channel out of the range of
     * the given skews.
     */
    Skew(SkewFunction skew, int im_w, int im_h, int im_d,
         OutOfBoundsPolicy p = WRAP);

    /**
     * Returns a new Skew that applies this Skew's transformation, scaled by
     *   the given amount.
     * @param sx Horizontal scale factor.
     * @param sy Vertical scale factor.
     * @return
     */
    Skew scale(float sx, float sy) const;

    /**
     * Returns a new Skew that applies this Skew's transformation, rotated by
     *   the given amount.
     * @param degrees Rotation amount. Positive is counterclockwise
     * @return
     */
    Skew rotate(float degrees) const;

    /**
     * Returns a new Skew that adds a translate to this Skew's transformation.
     * @param dx Horizontal translate.
     * @param dy Vertical translate.
     * @return
     */
    Skew translate(int dx, int dy) const;

    /**
     * Applies a coordinate transform to the given verts and returns the
     * result.
     * @param pt
     * @param chan
     * @return
     */
    Point operator()(const Point &pt, int32_t chan);

private:
    explicit Skew(std::shared_ptr<SkewImpl> pImpl);

    std::shared_ptr<SkewImpl> pImpl;
};


#endif //PXSORT_SKEW_H
