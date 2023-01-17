#include <execution>

#include "Compute.h"

using namespace pxsort;

void pxsort::sortSegments(
        Image &img,
        const std::vector<Segment> &segments,
        const std::vector<Segment::Traversal> &traversals,
        const std::vector<Sorter> &sorters,
        const std::vector<std::optional<Map>> &channelSkews,
        const std::vector<std::optional<Map>> &asdfRestrictStarts,
        const std::vector<std::optional<Map>> &asdfRestrictEnds) {
    std::vector<SegmentPixels> results(segments.size());

    std::vector<int> idx(segments.size());
    for (int i = 0; i < segments.size(); i++)
        idx[i] = i;

    auto sortSegment = [&](int i) {
        auto const &segment = segments[i];
        auto const &traversal = traversals[i % traversals.size()];
        auto const &sortPixels = sorters[i % sorters.size()];
        auto const &skew = channelSkews[i % channelSkews.size()];
        auto const &startTest =
                asdfRestrictStarts[i % asdfRestrictStarts.size()];
        auto const &endTest =
                asdfRestrictEnds[i % asdfRestrictEnds.size()];

        auto segPx = segment.getPixels(img, traversal, {});
        if (startTest.has_value() && endTest.has_value()) {
            segPx = segPx.asdfRestriction(startTest.value(), endTest.value());
        }

        auto skewPx = segPx;
        if (skew.has_value()) {
            skewPx = segment.getPixels(img, traversal, skew)
                            .restrictToIndices(segPx.restrictionIndices());
        }

        auto result = sortPixels(segPx, skewPx);
        results[i] = result;
    };

    std::for_each(std::execution::par, idx.begin(), idx.end(), sortSegment);

    for (int i = 0; i < segments.size(); i++) {
        auto const &traversal = traversals[i % traversals.size()];
        segments[i].putPixels(img, traversal, results[i]);
    }
}