#ifndef PXSORT2_PARAMETERIZATION_H
#define PXSORT2_PARAMETERIZATION_H

#include "common.h"

/**
 * A parameterization over a category T maps D-dimensional vectors to objects
 * in T.
 * @tparam T
 * @tparam D
 */
template<typename T, int D>
    requires (D > 0)
class pxsort::Parameterization {
public:
    virtual std::shared_ptr<T> operator()(cv::Vec<float, D>) = 0;
};



#endif //PXSORT2_PARAMETERIZATION_H
