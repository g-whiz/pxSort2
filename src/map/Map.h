#ifndef PXSORT2_MAP_H
#define PXSORT2_MAP_H

#include <utility>
#include <boost/preprocessor.hpp>

#include "common.h"

// Construct a Boost preprocessor list of the range:
//   [start, start + 1, ..., finish]
#define RANGE(start, finish)                                   \
    BOOST_PP_IF(BOOST_PP_LESS_EQUAL(start, finish),            \
                (start, RANGE(BOOST_PP_ADD(start, 1), finish)), \
                BOOST_PP_LIST_NIL)

#define SCALAR_TYPES (float, (double, (int, BOOST_PP_NIL)))
#define DIMENSIONS RANGE(1, PXSORT_MAX_DIMENSIONS)

// DSL for defining native mathematical functions on the fly.
namespace pxsort::map {

    template <typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    class Map {
    private:
        const std::function<cv::Vec<T, n> (const cv::Vec<T, m>&)> f;

    public:
        Map() = delete;

        Map(const Map<T, m, n, S>& other);

        Map(std::function<cv::Vec<T, n>(const cv::Vec<T, m>&)>  f);

        cv::Vec<S, n> operator()(const cv::Vec<T, m>& x) const;

        Map<T, m, 1, S> operator[](int i) const;

        // Returns a map that computes the vector sum of the results of this map
        // and the given map.
        Map<T, m, n, S> operator+(const Map<T, m, n, S>& other) const;

        // Returns a map that is the concatenation of this map and the given
        // map.
        template<int mo, int no>
        Map<T, m + mo, n + no, S> operator|(const Map<T, mo, no, S>& other) const;

        /**
         * Split operator:
         * Takes two maps with input dimension m and returns a map that
         * evaluates both input maps on the same m-vector and returns the
         * concatenation of their outputs.
         * @tparam no
         * @param other
         * @return
         */
        template<int no>
        Map<T, m, n + no, S> operator^(const Map<T, m, no, S>& other) const;

        /**
         * Composition operator:
         * If f: R^m -> R^n and g: R^l -> R^m
         * Then (f << g): R^l -> R^n
         * @tparam U
         * @tparam l
         * @param other
         * @return
         */
        template<typename U, int l>
        Map<U, l, n, S> operator<<(const Map<U, l, m, T>& other) const;
    };

#define INSTANTIATE(r, params) \
    template class Map<     \
        BOOST_PP_TUPLE_ELEM(0, params), \
        BOOST_PP_TUPLE_ELEM(1, params), \
        BOOST_PP_TUPLE_ELEM(2, params), \
        BOOST_PP_TUPLE_ELEM(3, params)>;
    BOOST_PP_LIST_FOR_EACH_PRODUCT(INSTANTIATE, 4, (SCALAR_TYPES, DIMENSIONS,
                                                    DIMENSIONS, SCALAR_TYPES))
#undef INSTANTIATE

    // Define a linear map using a matrix representation.
    template <typename T, int m, int n>
    Map<T, m, n> linear_transformation(const cv::Matx<T, n, m>&);

    // Define an affine map using a linear map and a translation.
    template <typename T, int m, int n>
    Map<T, m, n> affine_transformation(
            const cv::Matx<T, n, m>&, const cv::Vec<T, n>&);

    // Define a map f: R^m -> R^n from n component
    // functions f_i: R^m -> R
    template <typename T, int m, int n>
    Map<T, m, n> componentwise(const std::array<const Map<T, m, 1>, n>& F);

    // Returns the ith projection map of R^m
    template<typename T, int m>
    Map<T, m, 1> projection(int i);

#define INSTANTIATE(z, dim, type) \
    template Map<type, dim, 1> projection<type>(int i);

#define INSTANTIATE_M(r, data, type) \
    BOOST_PP_REPEAT_FROM_TO(1, PXSORT_MAX_DIMENSIONS, INSTANTIATE, type)

FOREACH_SCALAR_TYPE(INSTANTIATE_M, _)

#undef INSTANTIATE
#undef INSTANTIATE_M

    // Returns a map that casts vector components to a new data type
    // (e.g. int to double)
    template<typename T, typename S, int m>
    Map<T, m, m, S> type_cast();

    // Returns a map that clamps vector components to a specified interval.
    template<typename T, int m>
    Map<T, m, m> clamp(T min, T max);

    // returns a map that maps cartesian coordinates
    // to polar coordinates for R^2
    template<typename T>
    requires floating_point<T>
    Map<T, 2, 2> cartesian_to_polar();

    template Map<float, 2, 2> cartesian_to_polar<float>();
    template Map<double, 2, 2> cartesian_to_polar<double>();

    // Returns a dummy map that returns a constant
    template <typename T, int n>
    requires arithmetic<T>
    Map<T, 0, n> constant(const cv::Vec<T, n>&);



    // Returns a dummy map that returns a constant
    template <typename T>
    requires arithmetic<T>
    Map<T, 0, 1> constant(T alpha);

    // Returns a map that adds a scalar to a vector
    template<typename T, int m, typename S = T>
    requires arithmetic<S>
    Map<T, m, m> scalar_add(S scalar);

    // Returns a map that multiplies a vector by a scalar
    template<typename T, int m, typename S = T>
    requires arithmetic<S>
    Map<T, m, m> scalar_mult(S scalar);

    // Returns a map that returns the absolute value of its input.
    template <typename T>
    requires arithmetic<T>
    Map<T, 1, 1> abs();

    // Returns a map: (x, y) -> x % y, where 0 <= x % y < y
    template <typename T>
    requires integral<T>
    Map<T, 2, 1> modulo();

    // Returns a map that computes the remainder of the floating point division
    // operation
    template <typename T>
    requires floating_point<T>
    Map<T, 2, 1> fmod();

    // Returns a map that computes the signed remainder of the floating point
    // division operation
    template <typename T>
    requires floating_point<T>
    Map<T, 2, 1> remainder();

    // Returns a map that returns the maximum of its two inputs
    template <typename T>
    requires arithmetic<T>
    Map<T, 2, 1> max();

    // Returns a map that returns the minimum of its two inputs
    template <typename T>
    requires arithmetic<T>
    Map<T, 2, 1> min();

    // Returns a map: (x) -> e^x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> exp();

    // Returns a map: (x) -> 2^x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> exp2();

    // Returns a map: (x) -> ln(x) [i.e. natural log]
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> log();

    // Returns a map: (x) -> lg(x) [i.e. base-2 log]
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> log2();

    // Returns a map: (x, y) -> x^y
    template <typename T>
    requires floating_point<T>
    Map<T, 2, 1> pow();

    // Returns a map: (x) -> x^(1/2)
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> sqrt();

    // Returns a map: (x) -> x^(1/3)
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> cbrt();

    // Returns a map: (x) -> sin x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> sin();

    // Returns a map: (x) -> cos x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> cos();

    // Returns a map: (x) -> tan x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> tan();

    // Returns a map: (x) -> arcsin x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> asin();

    // Returns a map: (x) -> arccos x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> acos();

    // Returns a map: (x) -> arctan x
    template <typename T>
    requires floating_point<T>
    Map<T, 1, 1> atan();

    // Returns a map: (y, x) -> arctan (y / x)
    template <typename T>
    requires floating_point<T>
    Map<T, 2, 1> atan2();

    // Returns a map: (v) -> ||v|| where v is a vector in R^m
    template <typename T, int m>
    requires floating_point<T>
    Map<T, m, 1> norm();

    // Returns a map: (a, b) -> a & b
    template <typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_and();

    // Returns a map: (a, b) -> a | b
    template <typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_or();

    // Returns a map: (a) -> ~a
    template <typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_not();

    // Returns a map: (a, b) -> a ^ b
    template <typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_xor();

    // Returns a map: (a) -> a >> shift
    template <typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_right_shift(int shift);

    // Returns a map: (a) -> a << shift
    template <typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_left_shift(int shift);

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    Map<T, m, n, S>::Map(const Map<T, m, n, S> &other)  : f(other.f) {}

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    Map<T, m, n, S>::Map(std::function<cv::Vec<T, n>(const cv::Vec<T, m> &)> f)
            : f(std::move(f)) {}

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    cv::Vec<S, n> Map<T, m, n, S>::operator()(const cv::Vec<T, m> &x) const  {
        return f(x);
    }

    template <typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    Map<T, m, 1, S> Map<T, m, n, S>::operator[](int i) const {
        return projection<S, n>(i) << *this;
    }

    template <typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    Map<T, m, n, S> Map<T, m, n, S>::operator+(const Map<T, m, n, S>& other) const {
        const Map<T, m, n, S> self = *this;
        return Map<T, m, n, S>(
                [=](const cv::Vec<T, m>& x) {
                    return self(x) + other(x);
                }
        );
    }

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    template<typename U, int l>
    Map<U, l, n, S> Map<T, m, n, S>::operator<<(const Map<U, l, m, T>& other) const{
        const Map<T, m, n> self = *this;
        return Map<U, l, n, S>(
                [=](const cv::Vec<U, l>& x) {
                    return self(other(x));
                }
        );
    }

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    template<int mo, int no>
    Map<T, m + mo, n + no, S>
    Map<T, m, n, S>::operator|(const Map<T, mo, no, S> &other) const {
        const Map<T, m, n, S> self = *this;
        return Map<T, m + mo, n + no, S>(
                [=](const cv::Vec<T, m + mo>& x) {
                    cv::Vec<T, m> x_self;
                    for (int i = 0; i < m; i++)
                        x_self[i] = x[i];

                    cv::Vec<T, mo> x_other;
                    for(int i = 0; i < mo; i++)
                        x_other[i] = x[i + m];

                    auto y_self = self(x_self);
                    auto y_other = other(x_other);

                    cv::Vec<S, n + no> y;
                    for(int i = 0; i < n; i++)
                        y[i] = y_self[i];
                    for(int i = 0; i < no; i++)
                        y[i + n] = y_other[i];

                    return y;
                }
        );
    }

    template<typename T, int m, int n, typename S>
    requires arithmetic<T, S> && valid_dimensions<m, n>
    template<int no>
    Map<T, m, n + no, S> Map<T, m, n, S>::operator^(const Map<T, m, no, S> &other) const {
        const Map<T, m, n, S> self = *this;
        return Map<T, m, n + no, S>(
                [=](const cv::Vec<T, m>& x) {
                    auto y_self = self(x);
                    auto y_other = other(x);

                    cv::Vec<S, n + no> y;
                    for(int i = 0; i < n; i++)
                        y[i] = y_self[i];
                    for(int i = 0; i < no; i++)
                        y[i + n] = y_other[i];

                    return y;
                }
        );
    }

    template <typename T, int m, int n>
    Map<T, m, n> linear_transformation(const cv::Matx<T, n, m>& A){
        return Map<T, m, n>(
                [=](const cv::Vec<T, m>& x) {
                    return A * x;
                }
        );
    }

    template <typename T, int m, int n>
    Map<T, m, n> affine_transformation(
            const cv::Matx<T, n, m>& A, const cv::Vec<T, n>& b){
        return Map<T, m, n>(
                [=](const cv::Vec<T, m>& x) {
                    return (A * x) + b;
                }
        );
    }

    template <typename T, int m, int n>
    Map<T, m, n> componentwise(const std::array<const Map<T, m, 1>, n>& F){
        return Map<T, m, n>(
                [=](const cv::Vec<T, m>& x){
                    cv::Vec<T, n> y;
                    for (int i = 0; i < n; i++) {
                        y[i] = F[i](x)[0];
                    }
                    return y;
                }
        );
    }

    template<typename T, int m>
    Map<T, m, 1> projection(int i){
        int idx = PXSORT_MODULO(i, m);
        return Map<T, m, 1>(
            [=](const cv::Vec<T, m>& x){
                return cv::Vec<T, 1>(x[i]);
            }
        );
    }

    template<typename T, typename S, int m>
    Map<T, m, m, S> type_cast(){
        return Map<T, m, m, S>(
            [](const cv::Vec<T, m>& x){
                return static_cast<cv::Vec<S, m>>(x);
            }
        );
    }

    template<typename T, int m>
    Map<T, m, m> clamp(T min, T max){
        return Map<T, m, m>(
            [=](const cv::Vec<T, m>& x){
                cv::Vec<T, m> y;
                for (int i = 0; i < m; i++) {
                    y[i] = MIN(max, MAX(min, x[i]));
                }
                return y;
            }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 2, 2> cartesian_to_polar() {
        return Map<T, 2, 2>(
            [=](const cv::Vec<T, 2> & x){
                T r = hypot(x[0], x[1]);
                T phi = std::atan2(x[1], x[0]);
                return cv::Vec<T, 2>(r, phi);
            }
        );
    }

    template<typename T, int m, typename S>
    requires arithmetic<S>
    Map<T, m, m> scalar_add(S scalar) {
        return Map<T, m, m>(
                [=](const cv::Vec<T, m>& x) {
                    return self(x) + scalar;
                }
        );
    }

    template<typename T, int m, typename S>
    requires arithmetic<S>
    Map<T, m, m> scalar_mult(S scalar) {
        return Map<T, m, m>(
                [=](const cv::Vec<T, m>& x) {
                    return self(x) * scalar;
                }
        );
    }

    template<typename T, int n>
    requires arithmetic<T>
    Map<T, 0, n> constant(const cv::Vec<T, n>& x) {
        return Map<T, 0, n>(
                [=](const cv::Vec<T, 0>&) {
                    return x;
                }
        );
    }

    template<typename T>
    requires arithmetic<T>
    Map<T, 0, 1> map::constant(T alpha) {
        return Map<T, 0, 1>(
                [=](const cv::Vec<T, 0>&) {
                    return Vec<T, 1>(alpha);
                }
        );
    }

    template<>
    Map<float, 1, 1> abs<float>() {
        return {
                [=](const cv::Vec<float, 1>& x) {
                    return cv::Vec<float, 1>(std::fabs(x[0]));
                }
        };
    }

    template<>
    Map<double, 1, 1> abs<double>() {
        return {
                [=](const cv::Vec<double, 1>& x) {
                    return cv::Vec<double, 1>(std::fabs(x[0]));
                }
        };
    }

    template<>
    Map<int, 1, 1> abs<int>() {
        return {
                [=](const cv::Vec<int, 1>& x) {
                    return cv::Vec<int, 1>(std::abs(x[0]));
                }
        };
    }

    template<typename T>
    requires integral<T>
    Map<T, 2, 1> modulo() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(PXSORT_MODULO(x[0], x[1]));
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 1, 1> modulo(unsigned int modulus) {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(PXSORT_MODULO(x[0], modulus));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 2, 1> fmod() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(std::fmod<T>(x[0], x[1]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 2, 1> remainder() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(std::remainder<T>(x[0], x[1]));
                }
        );
    }

    template<typename T>
    requires arithmetic<T>
    Map<T, 2, 1> max() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(MAX(x[0], x[1]));
                }
        );
    }
    template<typename T>
    requires arithmetic<T>
    Map<T, 2, 1> min() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(MIN(x[0], x[1]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> exp() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::exp<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> exp2() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::exp2<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> log() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::log<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> log2() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::log2<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 2, 1> pow() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(std::pow<T>(x[0], x[1]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> sqrt() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::sqrt<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> cbrt() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::cbrt<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> sin() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::sin<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> cos() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::cos<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> tan() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::tan<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> asin() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::asin<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> acos() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::acos<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 1, 1> atan() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(std::atan<T>(x[0]));
                }
        );
    }

    template<typename T>
    requires floating_point<T>
    Map<T, 2, 1> atan2() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(std::atan2<T>(x[0], x[1]));
                }
        );
    }

    template<typename T, int m>
    requires floating_point<T>
    Map<T, m, 1> norm() {
        return Map<T, m, 1>(
                [=](const cv::Vec<T, m>& x) {
                    return Vec<T, 1>(std::sqrt<T>(x.dot(x)));
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_and() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(x[0] & x[1]);
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_or() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(x[0] | x[1]);
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_not() {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(~(x[0]));
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 2, 1> bitwise_xor() {
        return Map<T, 2, 1>(
                [=](const cv::Vec<T, 2>& x) {
                    return Vec<T, 1>(x[0] ^ x[1]);
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_right_shift(int shift) {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(x[0] >> shift);
                }
        );
    }

    template<typename T>
    requires integral<T>
    Map<T, 1, 1> bitwise_left_shift(int shift) {
        return Map<T, 1, 1>(
                [=](const cv::Vec<T, 1>& x) {
                    return Vec<T, 1>(x[0] << shift);
                }
        );
    }
} // pxsort::map

#endif //PXSORT2_MAP_H
