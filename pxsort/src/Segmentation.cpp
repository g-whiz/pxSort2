#include "Segmentation.h"

/*
 * TODO: After implementing a single-threaded prototype algo, adapt for
 *         concurrency using OpenMP (i.e. <omp.h>) as the backbone.
 *       This is easy for embarrassingly-parallel tilings (e.g. grid tiling with
 *         no overlap), but required a dependency graph approach for tilings
 *         with segments that share pixels.
 *       Note: to build execution plan from dependency graph, use a
 *         graph-coloring approach.
 */
#include <omp.h>

using namespace ps;

void Segmentation::addEffect(std::unique_ptr<Effect> e) {
    for (auto &tile : segments) {
        auto clone = e->clone();
        tile->attach(std::move(clone));
    }
}

void Segmentation::addEffect(std::unique_ptr<Effect> e, const int segment) {
    segments[segment]->attach(std::move(e));
}

void Segmentation::applyEffects() {
    for (auto &tile : segments) {
        tile->applyEffects();
    }
}
