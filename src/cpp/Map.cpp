#include "Map.h"

#include <utility>
#include <numeric>

using namespace pxsort;


class Map::MapImpl {
public:
    const uint32_t in_dim;
    const uint32_t out_dim;

    MapImpl(uint32_t in_dim, uint32_t out_dim)
      : in_dim(in_dim), out_dim(out_dim) {}

    virtual
    void operator()(float *in, float *out) const = 0;

    virtual ~MapImpl() = default;
};

class FuncPtrImpl : public Map::MapImpl {
    using fp_t = void(*)(float*, uint32_t, float*, uint32_t);
    const fp_t f;

public:
    FuncPtrImpl() = delete;

    FuncPtrImpl(void(*f_ptr)(float*, uint32_t, float*, uint32_t), uint32_t in_dim, uint32_t out_dim)
        : Map::MapImpl(in_dim, out_dim), f(f_ptr) {}

    ~FuncPtrImpl() override = default;

    inline
    void operator()(float *in, float *out) const override {
        f(in, in_dim, out, out_dim);
    }
};

class FuncObjImpl : public Map::MapImpl {
    const std::function<void(float*, uint32_t, float*, uint32_t)> f;

public:
    FuncObjImpl() = delete;

    explicit FuncObjImpl(
            std::function<void(float*, uint32_t, float*, uint32_t)> f_obj,
            uint32_t in_dim, uint32_t out_dim)
        : Map::MapImpl(in_dim, out_dim), f(std::move(f_obj)) {}

    ~FuncObjImpl() override = default;

    inline
    void operator()(float *in, float *out) const override {
        f(in, in_dim, out, out_dim);
    }
};

class CompositionImpl : public Map::MapImpl {
    const std::shared_ptr<MapImpl> f;
    const std::shared_ptr<MapImpl> g;

public:
    CompositionImpl() = delete;

    ~CompositionImpl() override = default;

    CompositionImpl(std::shared_ptr<MapImpl> f, std::shared_ptr<MapImpl> g)
        : Map::MapImpl(g->in_dim, g->out_dim),
          f(std::move(f)), g(std::move(g)) {}

private:
    inline
    void operator()(float *in, float *out) const override {
        float g_out[g->out_dim];
        (*g)(in, g_out);
        (*f)(g_out, out);
    }
};

class ConcatenationImpl : public Map::MapImpl {
public:
    using ImplPtr = std::shared_ptr<MapImpl>;

    ConcatenationImpl() = delete;

    ~ConcatenationImpl() override = default;

    ConcatenationImpl(ImplPtr f, ImplPtr g)
        : Map::MapImpl(f->in_dim + g->in_dim,
                       f->out_dim + g->out_dim), impls({f, g}) {}

    explicit ConcatenationImpl(std::vector<ImplPtr> impls)
        : Map::MapImpl(
                std::accumulate(impls.begin(), impls.end(), 0,
                                [](int d, ImplPtr i){return d + i->in_dim;}),
                std::accumulate(impls.begin(), impls.end(), 0,
                                [](int d, ImplPtr i){return d + i->out_dim;})
                ), impls(std::move(impls)){}

private:
    const std::vector<ImplPtr> impls;

    void operator()(float *in, float *out) const override {
        int in_idx = 0;
        int out_idx = 0;
        for (auto &impl: impls){
            (*impl)(&in[in_idx], &out[out_idx]);
            in_idx += impl->in_dim;
            out_idx += impl->out_dim;
        }
    }
};

class ForkImpl : public Map::MapImpl {
    const std::shared_ptr<MapImpl> f;
    const std::shared_ptr<MapImpl> g;
public:
    ForkImpl() = delete;

    ~ForkImpl() override = default;

    ForkImpl(std::shared_ptr<MapImpl> f, std::shared_ptr<MapImpl> g)
        : Map::MapImpl(f->in_dim, f->out_dim + g->out_dim),
          f(std::move(f)), g(std::move(g)) {}

private:
    void operator()(float *in, float *out) const override {
        (*f)(in, out);
        (*g)(in, &out[f->out_dim]);
    }
};

class ProjectionImpl : public Map::MapImpl {
    const std::shared_ptr<MapImpl> f;
    const int i;

public:
    ProjectionImpl() = delete;

    ~ProjectionImpl() override = default;

    ProjectionImpl(std::shared_ptr<MapImpl> f, int i)
        : Map::MapImpl(f->in_dim, 1), f(std::move(f)), i(i) {}

private:
    void operator()(float *in, float *out) const override {
        float f_out[f->out_dim];
        (*f)(in, f_out);
        out[0] = f_out[i];
    }
};

class ConstantImpl : public Map::MapImpl {
    const std::vector<float> values;

public:
    ConstantImpl() = delete;

    ~ConstantImpl() override = default;

    ConstantImpl(std::vector<float> values, uint32_t in_dim)
      : Map::MapImpl(in_dim, values.size()), values(std::move(values)) {}

private:
    void operator()(float *in, float *out) const override {
        for (int i = 0; i < values.size(); i++) {
            out[i] = values[i];
        }
    }
};

Map::Map(std::function<void(float *, uint32_t, float *, uint32_t)> f,
         uint32_t in_dim, uint32_t out_dim)
    : pImpl(std::make_shared<FuncObjImpl>(f, in_dim, out_dim)),
      inDim(in_dim), outDim(out_dim){}

pxsort::Map::Map(void(*f)(float*, uint32_t, float*, uint32_t), uint32_t in_dim, uint32_t out_dim)
        : pImpl(std::make_shared<FuncPtrImpl>(f, in_dim, out_dim)),
          inDim(in_dim), outDim(out_dim){}

pxsort::Map::Map(std::shared_ptr<MapImpl> pImpl, uint32_t in_dim, uint32_t out_dim)
    : pImpl(std::move(pImpl)), inDim(in_dim), outDim(out_dim) {}

std::vector<float> pxsort::Map::operator()(const std::vector<float> &x) const {
#ifdef PXSORT_DEBUG
    // skip this check in non-debug builds since it will be executed potentially
    // millions of times
    if (x.size() != inDim)
        throw std::invalid_argument(
                "pxsort::Map: dimension mismatch in given vector");
#endif
    auto in_ptr = const_cast<float *>(x.data());
    std::vector<float> y(outDim);
    auto out_ptr = y.data();
    (*pImpl)(in_ptr, out_ptr);
    return y;
}

Map pxsort::Map::operator<<(const Map &that) const {
    if (that.outDim != this->inDim)
        throw std::invalid_argument(
                "pxsort::Map: nPixels does not match this Map's inDim ");

    auto compPImpl = std::make_shared<CompositionImpl>(this->pImpl, that.pImpl);
    return {compPImpl, that.inDim, this->outDim};
}

Map pxsort::Map::operator|(const Map &that) const {
    auto catPImpl =
            std::make_shared<ConcatenationImpl>(this->pImpl, that.pImpl);
    return {catPImpl, this->inDim + that.inDim, this->outDim + that.outDim};
}

Map pxsort::Map::operator^(const Map &that) const {
    if (this->inDim != that.inDim)
        throw std::invalid_argument(
                "pxsort::Map: inDim does not match this Map's inDim ");

    auto forkPImpl = std::make_shared<ForkImpl>(this->pImpl, that.pImpl);
    return {forkPImpl, this->inDim, this->outDim + that.outDim};
}

Map pxsort::Map::operator[](int i) const {
    int safe_i = PXSORT_MODULO(i, outDim);

    return {std::make_shared<ProjectionImpl>(pImpl, safe_i), inDim, 1};
}

Map pxsort::Map::concatenate(const std::vector<Map> &maps) {
    std::vector<ConcatenationImpl::ImplPtr> impls(maps.size());
    uint32_t in_dim = 0;
    uint32_t out_dim = 0;
    for (int i = 0; i < maps.size(); i++) {
        in_dim += maps[i].inDim;
        out_dim += maps[i].outDim;
        impls[i] = maps[i].pImpl;
    }

    auto pImpl = std::make_shared<ConcatenationImpl>(impls);
    return {pImpl, in_dim, out_dim};
}

void pxsort::Map::operator()(float *in, float *out) const {
    (*pImpl)(in, out);
}

bool pxsort::Map::operator==(const Map &that) const {
    return this->pImpl == that.pImpl;
}

Map pxsort::Map::constant(std::vector<float> c, uint32_t in_dim) {
    auto pImpl = std::make_shared<ConstantImpl>(c, in_dim);
    return {pImpl, in_dim, static_cast<uint32_t>(c.size())};
}
