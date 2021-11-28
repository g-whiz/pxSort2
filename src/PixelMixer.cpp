#include "PixelMixer.h"

using namespace pxsort;


class Composition : public CloneableImpl<PixelMixer, Composition> {
public:
    Composition(std::unique_ptr<PixelMixer> f, std::unique_ptr<PixelMixer> g)
       : f(std::move(f)), g(std::move(g)) {}

    Composition(const Composition &other)
        : Composition(other.f->clone(), other.g->clone()){}


    std::pair<Pixel, Pixel>
            operator()(const Pixel &px1, const Pixel &px2) override {
        auto g_result = (*g)(px1, px2);
        return (*f)(g_result.first, g_result.second);
    }

private:
    std::unique_ptr<PixelMixer> f, g;
};

std::unique_ptr<PixelMixer>
        PixelMixer::compose(std::unique_ptr<PixelMixer> &other) {
    return std::unique_ptr<PixelMixer>(
            new Composition(this->clone(), std::move(other)));
}


