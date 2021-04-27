#include "Effect.h"
#include "src/effect/BubbleSort.h"
#include "src/effect/Heapify.h"

#include <utility>

using namespace ps;

Effect::Effect(const ChannelSkew &skew, Segment::Traversal traversal)
    : skew(skew), traversal(traversal){}
