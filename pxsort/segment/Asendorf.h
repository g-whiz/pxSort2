#ifndef PXSORT2_ASENDORF_H
#define PXSORT2_ASENDORF_H

#include "common.h"
#include "Segment.h"


/**
 * An Asendorf is a Segment that wraps another Segment so as to implement an
 * image segmentation similar to that used in Kim Asendorf's pioneering
 * implementation of a pixel-sorting algorithm.
 *
 * In particular, an Asendorf restricts the pixels in a given Segment to a
 * subset defined by two predicates - loTest and hiTest - in the following
 * manner:
 *   - let S be the base segment, and let S[i] be the i'th pixel in S,
 *       where 0 <= i < S.size()
 *   - let A be the Asendorf of S, using predicates loTest and hiTest
 *   - let i_lo = min { 0 <= i < S.size() | loTest(S[i]) == true}
 *   - let i_hi = max { 0 <= i < S.size() | hiTest(S[i]) == true}
 *   - then A.size() = max { 0, i_hi - i_lo + 1}
 *   - if A.size() > 0, then A[i] = S[i + i_lo], for 0 <= i < A.size()
 */
class pxsort::Asendorf : public Segment {
public:
    Asendorf(std::unique_ptr<Segment> child,
             PixelPredicate  loTest,
             PixelPredicate  hiTest);

private:
    int loIdx;
    int hiIdx;

    PixelPredicate loTest;
    PixelPredicate hiTest;

    std::unique_ptr<Segment> child;

    int size() override;

    Pixel forwardGetPixel(int idx, ChannelSkew &skew) override;

    void forwardSetPixel(int idx, ChannelSkew &skew, const Pixel &px) override;

    static int computeLoIdx(Segment &segment, const PixelPredicate &loTest);
    static int computeHiIdx(Segment &segment, const PixelPredicate &hiTest);
};


#endif //PXSORT2_ASENDORF_H
