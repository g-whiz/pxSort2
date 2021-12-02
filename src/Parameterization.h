#ifndef PXSORT2_PARAMETERIZATION_H
#define PXSORT2_PARAMETERIZATION_H

#include "common.h"

/**
 * A parameterization over a category Base maps D-dimensional vectors to objects
 * in Base.
 * @tparam T
 * @tparam ParamTypes... List of types for each parameter.
 */
template<typename T, typename... ParamTypes>
class pxsort::Parameterization {
public:
    virtual T operator()(ParamTypes...) = 0;
};

// Generic specialization for case where our object is static
//   (i.e. null parameterization).
template<typename T>
class pxsort::Parameterization<T> {
public:
    explicit Parameterization(T &staticT) : staticT(staticT) {}

    T operator()() {
        return staticT;
    }

private:
    T staticT;
};

#endif //PXSORT2_PARAMETERIZATION_H
