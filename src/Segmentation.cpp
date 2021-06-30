#include "Segmentation.h"

#include <utility>
#include "Segment.h"
#include "Sorter.h"

/*
 * TODO: After implementing a single-threaded prototype algo, adapt for
 *         concurrency using OpenMP (i.e. <omp.h>) as the backbone.
 *       This is easy for embarrassingly-parallel tilings (e.g. grid tiling with
 *         no overlap), but required a dependency graph approach for tilings
 *         with segments that share pixels.
 *       Note: to build execution plan from dependency graph, use a
 *         graph-coloring approach.
 */

using namespace pxsort;

void Segmentation::addEffect(std::shared_ptr<Sorter> e) {
    for (auto &tile : segments) {
        std::unique_ptr<Sorter> clone = e->clone();
        tile->addEffect(std::move(clone));
    }
}

void Segmentation::addEffect(std::shared_ptr<Sorter> e, int segment) {
    segments[segment]->addEffect(std::move(e));
}

void Segmentation::applyEffects() {
#pragma omp parallel for default(none)
    for (int i = 0; i < segments.size(); i++) {
        auto const& tile = segments[i];
        tile->applyEffects();
    }
}

Segmentation::Segmentation()
    : segments() {}

std::shared_ptr<Segment> Segmentation::getSegment(int i) {
    return segments.at(i);
}

size_t Segmentation::size() {
    return segments.size();
}

std::shared_ptr<Segmentation>
Segmentation::fromSegments(std::vector<std::shared_ptr<Segment>> segments) {
    auto s = std::make_shared<Segmentation>();
    s->segments = std::move(segments);
    return s;
}
