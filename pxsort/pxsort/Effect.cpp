#include "Effect.h"
#include "pxsort/effect/BubbleSort.h"
#include "pxsort/effect/Heapify.h"

#include <utility>

using namespace pxsort;

Effect::Effect(const ChannelSkew &skew, Segment::Traversal traversal)
    : skew(skew), traversal(traversal){}
