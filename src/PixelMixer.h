#ifndef PXSORT2_PIXELMIXER_H
#define PXSORT2_PIXELMIXER_H

#include "common.h"

/**
 * A PixelMixer is a callable object that mixes/transforms two pixels to
 *   produce two new pixels.
 * In this sense, a PixelMixer can be considered an endomorphism on the set
 *   [0, 1]^6.
 */
class pxsort::PixelMixer : public CloneableInterface<PixelMixer> {
public:
    virtual std::pair<Pixel, Pixel> operator()(const Pixel&, const Pixel&) = 0;

    virtual ~PixelMixer() = default;

    /**
     * Returns a PixelMixer that is the composition of the given PixelMixers
     *   (applied right-to-left).
     */
    template<template <typename> typename Pointer>
    std::unique_ptr<PixelMixer> operator*(Pointer<PixelMixer> &mixer) {
        return compose(mixer->clone());
    }

private:
    std::unique_ptr<PixelMixer> compose(std::unique_ptr<PixelMixer>&);
};
#endif //PXSORT2_PIXELMIXER_H
