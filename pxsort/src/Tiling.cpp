#include "Tiling.h"

/*
 * TODO: After implementing a single-threaded prototype algo, adapt for
 *         concurrency using OpenMP (i.e. <omp.h>) as the backbone.
 *       This is easy for embarrassingly-parallel tilings (e.g. grid tiling with
 *         no overlap), but required a dependency graph approach for tilings
 *         with tiles that share pixels.
 *       Note: to build execution plan from dependency graph, use a
 *         graph-coloring approach.
 */
#include <omp.h>

using namespace ps;

void Tiling::addEffect(std::unique_ptr<Effect> e) {
    for (auto &tile : tiles) {
        auto clone = e->clone();
        tile->attach(std::move(clone));
    }
}

void Tiling::addEffect(std::unique_ptr<Effect> e, const int tile) {
    tiles[tile]->attach(std::move(e));
}

void Tiling::applyEffects() {
    for (auto &tile : tiles) {
        tile->applyEffects();
    }
}
