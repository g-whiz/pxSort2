#ifndef PXSORT_COMPUTE_H
#define PXSORT_COMPUTE_H

#include <vector>
#include <optional>

#include "common.h"
#include "Map.h"
#include "Segment.h"
#include "Sorter.h"

namespace pxsort {

    /**
     * Sorts the given segments of the given image using the specified
     * traversals, sorters, skews, and restrictions.
     * @param img
     * @param segments
     * @param traversals
     * @param sorters
     * @param channelSkews
     * @param asdfRestrictStarts
     * @param asdfRestrictEnds
     */
    void sortSegments(
            Image &img,
            const std::vector<Segment> &segments,
            const std::vector<Segment::Traversal> &traversals,
            const std::vector<Sorter> &sorters,
            const std::vector<std::optional<Map>> &channelSkews = {{}},
            const std::vector<std::optional<Map>> &asdfRestrictStarts = {{}},
            const std::vector<std::optional<Map>> &asdfRestrictEnds = {{}}
    );
}
#endif //PXSORT_COMPUTE_H
